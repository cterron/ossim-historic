#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

int get_priority (MYSQL *mysql, char *query, int *priority) 
{
    MYSQL_RES   *result;
    MYSQL_ROW    row;
    unsigned int num_rows;
    int          success = 0;

    if (mysql_query(mysql, query)) {
       /* query failed */
       fprintf(stderr, "Failed to make query %s\n%s\n",
               query, mysql_error(mysql));
       
    } else {
        /* query succesded */
        if ( (result = mysql_store_result(mysql)) &&
             (num_rows = mysql_num_rows(result)) ) 
        {
            row = mysql_fetch_row(result);
            *priority = atoi(row[0]);
            success = 1;
            mysql_free_result(result);
        }
    }
    return success;
}

int get_asset(MYSQL *mysql, char *ip, int *asset) {
    
    MYSQL_RES   *result;
    MYSQL_ROW    row;
    unsigned int num_rows;
    char        *query;
    int          success = 0;

    query = (char *) malloc(sizeof(char) * QUERY_MAX_SIZE);
    snprintf(query, QUERY_MAX_SIZE, 
             "SELECT asset FROM host WHERE ip = '%s';", ip);

    if (mysql_query(mysql, query)) {
       /* query failed */
       fprintf(stderr, "Failed to make query %s\n%s\n",
               query, mysql_error(mysql));
    } else {
        /* query succesded */
        if ( (result = mysql_store_result(mysql)) &&
             (num_rows = mysql_num_rows(result)) ) 
        {
            row = mysql_fetch_row(result);
            *asset = atoi(row[0]);
            mysql_free_result(result);
        } else {
            *asset = MED_ASSET;
        }
        success = 1;
    }
    
    free(query);
    return success;
}

int get_level(MYSQL *mysql, char *ip, char *type, int *level) {

    MYSQL_RES   *result;
    MYSQL_ROW    row;
    unsigned int num_rows;
    char        *query;
    int          success = 0;
    
    char        *A = "attack";
    char        *C = "compromise";

    query = (char *) malloc(sizeof(char) * 1024);

    if (!strcmp(type, A))
        snprintf(query, QUERY_MAX_SIZE, 
            "SELECT attack FROM host_qualification WHERE host_ip = '%s';", ip);
    else if (!strcmp(type, C))
        snprintf(query, QUERY_MAX_SIZE,
            "SELECT compromise FROM host_qualification WHERE host_ip = '%s';", ip);
    else {
        printf("Error: argument type must be \"attack\" or \"compromise\"");
        return 0;
    }
    
    if (mysql_query(mysql, query)) {
       /* query failed */
       fprintf(stderr, "Failed to make query %s\n%s\n",
               query, mysql_error(mysql));
    } else {
        /* query succesded */
        if ( (result = mysql_store_result(mysql)) &&
             (num_rows = mysql_num_rows(result)) ) 
        {
            row = mysql_fetch_row(result);
            *level = atoi(row[0]);
            success = 1;
            mysql_free_result(result);
        } else {
            *level = MED_ASSET;
            success = 0;
        }
    }
    
    free(query);
    return success;
}

int update_level(MYSQL *mysql, char *ip, char *type, int level) {

    char        *query;
    int          success = 0;
    
    char        *A = "attack";
    char        *C = "compromise";

    query = (char *) malloc(sizeof(char) * 1024);

    if (!strcmp(type, A))
        snprintf(query, QUERY_MAX_SIZE,
                "UPDATE host_qualification SET attack = attack + %d \
                 WHERE host_ip = '%s';", level, ip);
    else if (!strcmp(type, C))
        snprintf(query, QUERY_MAX_SIZE,
                "UPDATE host_qualification SET compromise = compromise + %d \
                 WHERE host_ip = '%s';", level, ip);
    else {
        printf("Error: argument type must be \"attack\" or \"compromise\"");
        return 0;
    }
    
    if (mysql_query(mysql, query)) {
       /* query failed */
       fprintf(stderr, "Failed to make query%s\n%s\n",
               query, mysql_error(mysql));
    } else {
        success = 1;
    }

    free(query);
    return success;
}

static int update_net_level(MYSQL *mysql, char *net, char *type, int level) {

    char        *query;
    int          success = 0;
    
    char        *A = "attack";
    char        *C = "compromise";

    query = (char *) malloc(sizeof(char) * 1024);

    if (!strcmp(type, A))
        snprintf(query, QUERY_MAX_SIZE,
                "UPDATE net_qualification SET attack = attack + %d \
                 WHERE net_name = '%s';", level, net);
    else if (!strcmp(type, C))
        snprintf(query, QUERY_MAX_SIZE,
                "UPDATE net_qualification SET compromise = compromise + %d \
                 WHERE net_name = '%s';", level, net);
    else {
        printf("Error: argument type must be \"attack\" or \"compromise\"");
        return 0;
    }
    
    if (mysql_query(mysql, query)) {
       /* query failed */
       fprintf(stderr, "Failed to make query: %s\n%s\n",
               query, mysql_error(mysql));
    } else {
        success = 1;
    }

    free(query);
    return success;
}

int update_nets_level(MYSQL *mysql, char *ip, char *type, int level) {

    MYSQL_RES   *result;
    MYSQL_ROW    row;
    char        *query;
    int          success = 0;

    query = (char *) malloc(sizeof(char) * 1024);
    snprintf(query, QUERY_MAX_SIZE, 
        "SELECT net_name FROM net_host_reference WHERE host_ip = '%s';", ip);
    
    if (mysql_query(mysql, query)) {
       /* query failed */
       fprintf(stderr, "Failed to make query: %s\n%s\n",
               query, mysql_error(mysql));
    } else {
        /* query succesded */
        if ( (result = mysql_store_result(mysql)) )
        {
            while ( (row = mysql_fetch_row(result)) ) {
                if (!update_net_level(mysql, row[0], type, level))
                    return 0;
            }
            success = 1;
            mysql_free_result(result);
        }
    }
    
    free(query);
    return success;
}

int insert_level(MYSQL *mysql, char *ip, int levelC, int levelA) {

    char *query;
    int   success = 0;

    query = (char *) malloc(sizeof(char) * 1024);

    snprintf(query, QUERY_MAX_SIZE,
            "INSERT INTO host_qualification VALUES ('%s', %d, %d);", 
            ip, levelC, levelA);
    if (mysql_query(mysql, query)) {
       fprintf(stderr, "Failed to make query: %s\n%s\n",
               query, mysql_error(mysql));
    } else {
        success = 1;
    }
    
    free(query);
    return success;
}




int get_sensor_id(MYSQL *mysql, char *sensor_fw, unsigned long int *sensor_id) 
{
    MYSQL_RES    *result;
    MYSQL_ROW     row;
    unsigned int  num_rows;
    char         *query;
    int           success = 0;

    query = (char *) malloc(sizeof(char) * 1024);
    snprintf(query, QUERY_MAX_SIZE,
             "SELECT sid FROM sensor WHERE hostname = '%s';", sensor_fw);
    
    if (mysql_query(mysql, query)) {
       fprintf(stderr, "Failed to make query: %s\n%s\n",
               query, mysql_error(mysql));        
    } else {
        /* query succesded */
        if ( (result = mysql_store_result(mysql)) &&
             (num_rows = mysql_num_rows(result)) ) 
        {
            row = mysql_fetch_row(result);
            *sensor_id = atoi(row[0]);
            success = 1;
            mysql_free_result(result);
        } else {
            printf("No firewall(%s) definied in sensor table\n", sensor_fw);
        }
    }
 
    free(query);
    return success;
}

int get_sig_id(MYSQL *mysql, char *action, unsigned long int *sig_id)
{
    MYSQL_RES    *result;
    MYSQL_ROW     row;
    unsigned int  num_rows;
    char         *query;
    int           success = 0;

    query = (char *) malloc(sizeof(char) * 1024);
    
    if (!strcmp(action, "accept")) {
        snprintf(query, QUERY_MAX_SIZE,
                 "SELECT sig_id FROM signature WHERE sig_name = '%s';",
                 FW1_SIG_ACCEPT_NAME);
    } else if (!strcmp(action, "deny")) {
        snprintf(query, QUERY_MAX_SIZE,
                 "SELECT sig_id FROM signature WHERE sig_name = '%s';",
                 FW1_SIG_DROP_NAME);
    } else if (!strcmp(action, "reject")) {
        snprintf(query, QUERY_MAX_SIZE,
                 "SELECT sig_id FROM signature WHERE sig_name = '%s';",
                 FW1_SIG_REJECT_NAME);
    }

    if (mysql_query(mysql, query)) {
       fprintf(stderr, "Failed to make query: %s\n%s\n",
               query, mysql_error(mysql));
    } else {
        /* query succesded */
        if ( (result = mysql_store_result(mysql)) &&
             (num_rows = mysql_num_rows(result)) ) 
        {
            row = mysql_fetch_row(result);
            *sig_id = atoi(row[0]);
            success = 1;
            mysql_free_result(result);
        } 
    }
   
    free(query);
    return success;
}


int get_sig_class_id(MYSQL *mysql, unsigned long int *sig_class_id)
{
    MYSQL_RES    *result;
    MYSQL_ROW     row;
    unsigned int  num_rows;
    char         *query;
    int           success = 0;

    query = (char *) malloc(sizeof(char) * 1024);
    
    snprintf(query, QUERY_MAX_SIZE,
             "SELECT sig_class_id FROM sig_class WHERE sig_class_name = '%s';",
             FW1_SIG_CLASS_NAME);

    if (mysql_query(mysql, query)) {
       fprintf(stderr, "Failed to make query: %s\n%s\n",
               query, mysql_error(mysql));
    } else {
        /* query succesded */
        if ( (result = mysql_store_result(mysql)) &&
             (num_rows = mysql_num_rows(result)) ) 
        {
            row = mysql_fetch_row(result);
            *sig_class_id = atoi(row[0]);
            success = 1;
            mysql_free_result(result);
        } 
    }
   
    free(query);
    return success;
}


static int get_last_cid (MYSQL *mysql, 
                         unsigned int sid, 
                         unsigned long int *cid) 
{
    MYSQL_RES    *result;
    MYSQL_ROW     row;
    unsigned int  num_rows;
    char         *query;
    int           success = 0;

    query = (char *) malloc(sizeof(char) * 1024);
    snprintf(query, QUERY_MAX_SIZE, 
             "select last_cid from sensor where sid = %u;", sid);
        
    if (mysql_query(mysql, query)) {
       fprintf(stderr, "Failed to make query: %s\n%s\n",
               query, mysql_error(mysql));
    } else {
        /* query succesded */
        if ( (result = mysql_store_result(mysql)) &&
             (num_rows = mysql_num_rows(result)) ) 
        {
            row = mysql_fetch_row(result);
            *cid = atoi(row[0]);
            success = 1;
            mysql_free_result(result);
        } 
    }
   
    free(query);
    return success;
}

int insert_event(MYSQL *mysql, unsigned long int sid, unsigned long int sig_id,
                 unsigned long int sig_class_id, char *new_sig_name,
                 unsigned long int src_ip, unsigned long int dst_ip,
                 unsigned int src_port, unsigned int dst_port,
                 char *protocol) 
{
    char *query_snort;
    char *query_acid;
    char *query_sensor;
    int   success = 0;
    unsigned long int cid = 0;
    unsigned int proto;

    query_snort  = (char *) malloc(sizeof(char) * QUERY_MAX_SIZE);
    query_acid   = (char *) malloc(sizeof(char) * QUERY_MAX_SIZE);
    query_sensor = (char *) malloc(sizeof(char) * QUERY_MAX_SIZE);
    
    get_last_cid(mysql, sid, &cid);

    if (!strcmp(protocol, "icmp"))
        proto = 1;
    else if (!strcmp(protocol, "tcp"))
        proto = 6;
    else if (!strcmp(protocol, "udp"))
        proto = 17;
    else
        proto = 0; /* ip */


    snprintf(query_snort, QUERY_MAX_SIZE,
            "INSERT INTO event (sid, cid, signature, timestamp)\
             VALUES (%lu, %lu, %lu, now());", sid, cid + 1, sig_id);
    
    snprintf(query_acid, QUERY_MAX_SIZE,
            "INSERT INTO acid_event VALUES\
             (%lu, %lu, %lu, '%s', %lu, 2, now(), %lu, %lu,\
              %u, %u, %u);",
            sid, cid + 1, sig_id, new_sig_name, sig_class_id, 
            src_ip, dst_ip, proto, src_port, dst_port);
    
    snprintf(query_sensor, QUERY_MAX_SIZE,
             "UPDATE sensor SET last_cid = last_cid + 1 WHERE sid=%lu;", sid);

    if (mysql_query(mysql, query_snort)) {
       fprintf(stderr, "failed to make query: %s\n%s\n",
               query_snort, mysql_error(mysql));
       return 0;
    } else {
        if (mysql_query(mysql, query_sensor)) {
            fprintf(stderr, "failed to make query: %s\n%s\n",
                   query_snort, mysql_error(mysql));
            return 0;
        }
        success = 1;
    }

    if (mysql_query(mysql, query_acid)) {
        fprintf(stderr, "Failed to make query: %s\n%s\n",
                query_acid, mysql_error(mysql));
        return 0;
    } else {
        success = 1;
    }
    
    free(query_snort);
    free(query_acid);
    free(query_sensor);
    return success;
}

