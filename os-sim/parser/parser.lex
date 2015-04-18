D   [0-9]
L   [A-Za-z\-\_]

%x SENSOR
%x PROG
%x PLUGIN
%x TPLUGIN
%x DESCR
%x PROTOCOL
%x SOURCE_IP
%x SOURCE_PORT
%x DEST_IP
%x DEST_PORT
%x FIREWALL
%x FIREWALL_SENSOR

%{
#include "common.h"
#include <netdb.h>

/* globals! */

int     plugin;
int     tplugin;

char    protocol[5];
char    source_ip[16]; 
char    dest_ip[16];
int     source_port;
int     dest_port;

/* snort specific */
int     is_icmp;
unsigned int priority_snort = SNORT_DEFAULT_PRIORITY;

/* fw-1 specific */
char    service[64];
char    action[24];
int     action_type;
char    sensor_fw[16];
int     rule;
unsigned int priority_fw1 = FW1_DEFAULT_PRIORITY;

struct servent *serv;

MYSQL   mysql;

%}

%%

<INITIAL>^{L}+" "+{D}{D}?" "+{D}{D}:{D}{D}:{D}{D} {
#ifdef VERBOSE
    printf("\ndate:     %s\n", yytext);
#endif
    BEGIN(SENSOR);
}

<SENSOR>{L}+|({D}|\.)+ {
#ifdef VERBOSE
    printf("sensor     %s\n", yytext);
#endif

#ifdef FW1
    if (!strcmp(yytext, "logger")) {
        BEGIN(FIREWALL);
    } else {
        BEGIN(PROG);
    }
#else
    BEGIN(PROG);
#endif
}

<PROG>{L}+ {
#ifdef VERBOSE
    printf("program:  %s\n", yytext);
#endif

    if (strcmp(yytext, "snort")) {
        BEGIN(INITIAL);
        continue;
    }
        
    BEGIN(PLUGIN);
}

<PLUGIN>{D}+ {
#ifdef VERBOSE
    printf ("plugin:    %s\n", yytext);
#endif
    plugin = atoi(yytext);

    switch(plugin) {
    case GENERATOR_SPP_SPADE:    /* spade */
        BEGIN(PROTOCOL);
        break;
    case GENERATOR_SPP_SCAN2:    /* portscan */
        BEGIN(PROTOCOL);
        break;
    case GENERATOR_SNORT_ENGINE: /* snort */
        BEGIN(TPLUGIN);
        break;
    default:
        BEGIN(INITIAL);
        break;
    }
}

<TPLUGIN>{D}+ {
#ifdef VERBOSE
    printf("tplugin:   %s\n", yytext);
#endif
    tplugin = atoi(yytext);
    BEGIN(DESCR);
}

<DESCR>\]([^\[])+ {
#ifdef VERBOSE
    printf("desc:      %s\n", yytext + 1);
#endif
    BEGIN(PROTOCOL);
}

<PROTOCOL>"Priority: "{D}+ {
#ifdef VERBOSE
    printf("snort priority: %s\n", yytext + strlen("Priority: "));
#endif
    /*
     * snort priorities are order desc 
     * 1 is high priority
     */
    priority_snort = abs(atoi(yytext + strlen("Priority: ")) - 
                         (SNORT_MAX_PRIORITY + 1));
}

<PROTOCOL>\{{L}+ {
#ifdef VERBOSE
    printf("protocol:  %s\n", yytext + 1);
#endif
    snprintf(protocol, sizeof(protocol), "%s", yytext + 1);
    if (strcmp(yytext + 1, "ICMP"))
        is_icmp = 0;
    else
        is_icmp = 1;
    BEGIN(SOURCE_IP);
}

<SOURCE_IP>({D}|\.)+ {
#ifdef VERBOSE
    printf("src_ip:    %s\n", yytext);
#endif
    snprintf(source_ip, sizeof(source_ip), "%s", yytext);
    /* ICMP alerts doesn't have ports */
    if (is_icmp)
        BEGIN(DEST_IP);
    else
        BEGIN(SOURCE_PORT);
}

<SOURCE_PORT>{D}+ {
#ifdef VERBOSE
    printf("src_port:  %s\n", yytext);
#endif
    source_port = atoi(yytext);
    BEGIN(DEST_IP);
}


<DEST_IP>({D}|\.)+ {
#ifdef VERBOSE
    printf("dest_ip:   %s\n", yytext);
#endif
    snprintf(dest_ip, sizeof(dest_ip), "%s", yytext);
    /* ICMP alerts don't have ports */
    if (is_icmp) {
    
        calculate(&mysql, plugin, tplugin, priority_snort, protocol, 
                  source_ip, dest_ip, ANY_PORT, ANY_PORT);
                  
        BEGIN(INITIAL);
    } else {
        BEGIN(DEST_PORT);
    }
}

<DEST_PORT>{D}+ {
#ifdef VERBOSE
    printf("dest_port: %s\n", yytext);
#endif
    dest_port = atoi(yytext);

    calculate(&mysql, plugin, tplugin, priority_snort, protocol, 
              source_ip, dest_ip, source_port, dest_port);

    BEGIN(INITIAL);
}


<FIREWALL>"src: "({D}|\.)+ {
    snprintf(source_ip, sizeof(source_ip), "%s", yytext + strlen("src: "));
}

<FIREWALL>"s_port: "{D}+ {
    source_port = atoi(yytext + strlen("s_port: "));
}

<FIREWALL>"dst: "({D}|\.)+ {
    snprintf(dest_ip, sizeof(dest_ip), "%s", yytext + strlen("dst: "));
}

<FIREWALL>"service: "({L}|{D})+ {
    snprintf(service, sizeof(service), "%s", yytext + strlen("service: "));
    dest_port = getportbyservice(service);
}

<FIREWALL>"proto: "{L}+ {
    snprintf(protocol, sizeof(protocol), "%s", yytext + strlen("proto: "));
}

<FIREWALL>accept|reject|drop {
    snprintf(action, sizeof(action), "%s", yytext);
    BEGIN(FIREWALL_SENSOR);
}

<FIREWALL_SENSOR>({D}|\.)+ {
    snprintf(sensor_fw, sizeof(sensor_fw), "%s", yytext);
    BEGIN(FIREWALL);
}

<FIREWALL>"rule: "{D}+ {
    rule = atoi(yytext + strlen("rule: "));
#ifdef VERBOSE
    printf("src:       %s (%lu)\n", source_ip, acidIP2long(source_ip));
    printf("s_port:    %d\n", source_port);
    printf("dst:       %s (%lu)\n", dest_ip, acidIP2long(dest_ip));
    printf("d_port:    %d\n", dest_port);
    printf("service:   %s\n", service);
    printf("protocol:  %s\n", protocol);
    printf("action:    %s\n", action);
    printf("sensor-fw: %s\n", sensor_fw);
    printf("rule:      %d\n", rule);
#endif

//    insert_fw1_alert(acidIP2long(source_ip), source_port,
//                     acidIP2long(dest_ip), dest_port, 
//                     service, protocol, action, sensor_fw, rule);

    if (!strcmp(action, "accept")) {
        action_type = FW1_ACCEPT_TYPE;
        priority_fw1 = FW1_ACCEPT_PRIORITY;
    } else if (!strcmp(action, "drop")) {
        action_type = FW1_DROP_TYPE;
        priority_fw1 = FW1_DROP_PRIORITY;
    } else if (!strcmp(action, "reject")) {
        action_type = FW1_REJECT_TYPE;
        priority_fw1 = FW1_REJECT_PRIORITY;
    }

    if (priority_fw1) {
        calculate(&mysql, GENERATOR_FW1, action_type, priority_fw1, protocol, 
                  source_ip, dest_ip, source_port, dest_port);
    }

    BEGIN(INITIAL);
}


<INITIAL,SENSOR,PROG,PLUGIN,TPLUGIN,DESCR,PROTOCOL,SOURCE_IP,SOURCE_PORT,DEST_IP,DEST_PORT,FIREWALL>.|\n /* crap */

%%


int main (int argc, char **argv)
{
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
    } else {
        yyin = stdin;
    }

    /* establish a connection to a MySQL database engine */
    mysql_init(&mysql);
    mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "parser_syslog");
    if (!mysql_real_connect(&mysql,
                            get_conf("ossim_host"),
                            get_conf("ossim_user"),
                            get_conf("ossim_pass"),
                            get_conf("ossim_base"),
                            0,
                            NULL,
                            0))
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n",
                mysql_error(&mysql));
    }

     yylex();

    mysql_close(&mysql);

    return 0;
}

