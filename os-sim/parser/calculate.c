#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include "common.h"

static int is_attack_responses (int plugin, int tplugin) {

    FILE *fd;
    int   sid;
    int   is_attack_responses = 0;
    
    if (plugin == 1) {
        if (NULL == (fd = fopen(ATTACK_RESPONSES_SIDS_FILE, "r"))) {
            printf("Can't open file %s\n", ATTACK_RESPONSES_SIDS_FILE);
            exit (-1);
        }
        while (!feof(fd)) {
            fscanf(fd, "%d", &sid);
            if (sid == tplugin) {
                is_attack_responses = 1;
                break;
            }
        }
        fclose(fd);
    }
    
    return is_attack_responses;
}


static char *get_signature(int sid) {

    FILE *fd;
    int   sf = 0;
    char *sig;

    sig = (char *) malloc(sizeof(char) * SIGNATURE_MAX_SIZE);

    if (NULL == (fd = fopen(SIDS_FILE, "r"))) {
        printf("Can't open file %s\n", SIDS_FILE);
        exit (-1);
    }

    while(!feof(fd)) {
        fscanf(fd, "%d", &sf);
        fscanf(fd, "%s", sig);
        if (sid == sf) break;
    }
    fclose(fd);
    
    
    return sig;
}


void calculate(MYSQL *mysql, int plugin, int tplugin, 
               unsigned int priority_snort, 
               char protocol[5], char source_ip[16], char dest_ip[16], 
               int source_port, int dest_port)
{
    int     is_atckrsp = 0;
    char   *signature;
    int     priority = 1;
    char   *query_l1;
    char   *query_l2;
    int     source_asset = 0, dest_asset = 0;
    int     impactC = 0, impactA = 0;
    int     sourceC = 0, destA = 0, destC = 0;
    
    /* 
     * is an attack-responses? 
     */
    is_atckrsp = is_attack_responses(plugin, tplugin);

    /*
     * Get snort signature
     */
    if (plugin == GENERATOR_SPP_SPADE) {
        signature = (char *) malloc(sizeof(char) * 6);
        sprintf(signature, "spade");
        
    } else if (plugin == GENERATOR_FW1) {
        if (tplugin == FW1_ACCEPT_TYPE) {
            signature = (char *) malloc(sizeof(char) * strlen("fw1-accept")+1);
            sprintf(signature, "fw1-accept");
        } else if (tplugin == FW1_DROP_TYPE) {
            signature = (char *) malloc(sizeof(char) * strlen("fw1-drop")+1);
            sprintf(signature, "fw1-drop");
        } else if (tplugin == FW1_REJECT_TYPE) {
            signature = (char *) malloc(sizeof(char) * strlen("fw1-reject")+1);
            sprintf(signature, "fw1-reject");
        } else {
            printf("Unexpected error getting signature\n");
            exit (-1);
        }

    } else {
        signature = get_signature(tplugin);
    }


    /*
     * Level 1
     *
     * source_ip, source_dest, dest_port and signature 
     * match with the snort alert.
     */
    query_l1 = (char *) malloc(sizeof(char) * QUERY_MAX_SIZE);
    snprintf(query_l1, QUERY_MAX_SIZE, 
            "SELECT DISTINCT p.priority FROM \n\
                policy p, \n\
                policy_net_reference pns, policy_host_reference phs,\n\
                policy_net_reference pnd, policy_host_reference phd, \n\
                policy_port_reference pp, policy_sig_reference ps, \n\
                signature_group_reference sg, port_group_reference pg, \n\
                net_host_reference nh \n\
            WHERE \n\
            ((phs.host_ip = '%s' AND phs.direction = 'source') OR \n\
             (pns.net_name = nh.net_name AND \n\
              nh.host_ip = '%s' AND pns.direction = 'source')) AND \n\
            ((phd.host_ip = '%s' AND phd.direction = 'dest') OR \n\
             (pnd.net_name = nh.net_name AND \n\
              nh.host_ip = '%s' AND pnd.direction = 'dest')) AND \n\
            (pp.port_group_name = pg.port_group_name AND \n\
             ((pg.port_number = %d) OR (pg.port_number = %d)) AND \n\
             ((pg.protocol_name = LCASE('%s') OR\
              pg.protocol_name = '%s'))) AND \n\
            ((ps.sig_group_name = sg.sig_group_name) AND \n\
             (sg.sig_name = '%s')) AND \n\
            p.id = pns.policy_id AND \n\
            p.id = pnd.policy_id AND \n\
            p.id = phs.policy_id AND \n\
            p.id = phd.policy_id AND \n\
            p.id = pp.policy_id AND \n\
            p.id = ps.policy_id;",
            source_ip, source_ip, dest_ip, dest_ip, dest_port, ANY_PORT,
            protocol, protocol, signature);

    /*
     * Level 2
     *
     * dest_ip and dest_port match with the snort alert.
     */
    query_l2 = (char *) malloc(sizeof(char) * QUERY_MAX_SIZE);
    snprintf(query_l2, QUERY_MAX_SIZE, 
            "SELECT DISTINCT p.priority FROM \n\
                policy p, \n\
                policy_net_reference pnd, policy_host_reference phd, \n\
                policy_port_reference pp, policy_sig_reference ps, \n\
                port_group_reference pg, net_host_reference nh \n\
            WHERE \
            ((phd.host_ip = '%s' AND phd.direction = 'dest') OR \n\
             (pnd.net_name = nh.net_name AND \n\
              nh.host_ip = '%s' AND pnd.direction = 'dest')) AND \n\
            (pp.port_group_name = pg.port_group_name AND \n\
             ((pg.port_number = %d) OR (pg.port_number = %d)) AND \n\
             ((pg.protocol_name = LCASE('%s') OR\
              pg.protocol_name = '%s'))) AND \n\
            p.id = pnd.policy_id AND \n\
            p.id = phd.policy_id AND \n\
            p.id = pp.policy_id;",
            dest_ip, dest_ip, dest_port, ANY_PORT, protocol, protocol);

    if (get_priority(mysql, query_l1, &priority)) {
#ifdef VERBOSE
        printf("priority at level 1: %d\n", priority);
#endif
    } 
    else if (get_priority(mysql, query_l2, &priority)) {
#ifdef VERBOSE
        printf("priority at level 2: %d\n", priority);
#endif
    }

    
    /*
     * Level 3
     *
     * Get asset from ips
     */
    if (get_asset(mysql, source_ip, &source_asset))
        impactC = priority * priority_snort * source_asset;
    if (get_asset(mysql, dest_ip, &dest_asset))
        impactA = priority * priority_snort * dest_asset;

#ifdef VERBOSE
    printf("\nPriority: %d\n", priority);
    printf("Source asset: %d, Dest asset: %d\n", source_asset, dest_asset);
    printf("C impact: %d, A impact: %d\n\n", impactC, impactA);
#endif


    /* C level */
    if (get_level(mysql, source_ip, "compromise", &sourceC)) {
#ifdef VERBOSE
        printf("compromise of ip %s is %d\n", source_ip, sourceC);
#endif
        update_level(mysql, source_ip, "compromise", impactC);
        update_nets_level(mysql, source_ip, "compromise", impactC);
    } else {
        insert_level(mysql, source_ip, sourceC, 1);
    }

    /* A level */
    if (get_level(mysql, dest_ip, "attack", &destA)) {
#ifdef VERBOSE
        printf("attack of ip %s is %d\n", dest_ip, destA);
#endif
        update_level(mysql, dest_ip, "attack", impactA);
        update_nets_level(mysql, dest_ip, "attack", impactA);
    } else {
        insert_level(mysql, dest_ip, 1, destA);
    }

    /* attack-responses */
    if (is_atckrsp) {        
        if (get_level(mysql, dest_ip, "compromise", &destC)) {
#ifdef VERBOSE
            printf("compromiso de la ip %s es %d\n", dest_ip, destC);
#endif
            update_level(mysql, dest_ip, "compromise", impactC);
            update_nets_level(mysql, dest_ip, "compromise", impactC);
        }
    }
#ifdef VERBOSE
    printf("\n");
#endif

    free(query_l1);
    free(query_l2);
    free(signature);
    
}


