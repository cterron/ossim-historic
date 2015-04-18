#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include "common.h"

void log_rrd(MYSQL *mysql, char source_ip[16], char what[128], 
             unsigned int priority) 
{
    int asset;
    int value;
    int C, A;
    
    if (!strcmp(source_ip, "global")) {
        printf("Works... alpha\n");
    } else {
    
        /* get asset from host table */
        if (get_asset(mysql, source_ip, &asset)) 
            value = priority * asset;
        else
            value = priority * RRD_DEFAULT_PRIORITY;
        
        /* compromise */
        if (get_level(mysql, source_ip, "compromise", &C)) {
            update_level(mysql, source_ip, "compromise", value);
            update_nets_level(mysql, source_ip, "compromise", value);
#ifdef VERBOSE
            printf("rrd:compromise of ip %s is %d\n", source_ip, C + value);
#endif
        } else {
            insert_level(mysql, source_ip, value, 1);
        }
        
        /* attack */
        if (get_level(mysql, source_ip, "attack", &A)) {
            update_level(mysql, source_ip, "attack", value);
            update_nets_level(mysql, source_ip, "attack", value);
#ifdef VERBOSE
            printf("rrd:attack of ip %s is %d\n", source_ip, A + value);
#endif
        } else {
            insert_level(mysql, source_ip, 1, value);
        }
    }
}
