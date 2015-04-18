#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"


static int
in_same_graph(MYSQL *mysql, char *source_ip, char *dest_ip) {

    char *query;
    MYSQL_RES *result;
    unsigned int num_rows;

    query = (char *) malloc(sizeof(char) * QUERY_MAX_SIZE);
    snprintf(query, QUERY_MAX_SIZE,
             "SELECT DISTINCT id FROM graph WHERE ip = '%s' OR ip = '%s';",
             source_ip, dest_ip);
    if (mysql_query(mysql, query)) {
        fprintf(stderr, "Failed to make query %s\n%s\n",
                query, mysql_error(mysql));
    } else {
        if ( (result = mysql_store_result(mysql)) &&
             (1 == (num_rows = mysql_num_rows(result))) ) {
            return 1;
        } else {
            return 0;
        }
    }
    return 0;
}

static void
new_graph(MYSQL *mysql, char *source_ip, char *dest_ip) {
    
    char *query;

    query = (char *) malloc(sizeof(char) * QUERY_MAX_SIZE);
    
    snprintf(query, QUERY_MAX_SIZE,
             "INSERT INTO graph VALUES (NULL, '%s');", source_ip);
    if (mysql_query(mysql, query)) {
        fprintf(stderr, "Failed to make query %s\n%s\n",
                query, mysql_error(mysql));
    }
    snprintf(query, QUERY_MAX_SIZE,
             "INSERT INTO graph VALUES (last_insert_id(), '%s');", dest_ip);
    if (mysql_query(mysql, query)) {
        fprintf(stderr, "Failed to make query %s\n%s\n",
                query, mysql_error(mysql));
    }
    free(query);
}


static void 
move_graph(MYSQL *mysql, 
           unsigned int source_graph_id, unsigned int dest_graph_id,
           char *source_ip, char *dest_ip) {
    
    char *query;
    char *ip;
    MYSQL_RES *result;
    MYSQL_ROW  row;
    unsigned int num_rows;
    
    query = (char *) malloc(sizeof(char) * QUERY_MAX_SIZE);
    ip = (char *) malloc(sizeof(char) * 16);
    
    /* 
     * create new graph and insert source and dest ips 
     */
    new_graph(mysql, source_ip, dest_ip);

    /* 
     * search for ips in source and dest graphs and insert
     * in the new graph
     */
    snprintf(query, QUERY_MAX_SIZE,
            "SELECT ip FROM graph WHERE id = '%u' OR id = '%u';",
            source_graph_id, dest_graph_id);
    puts(query);
    if (mysql_query(mysql, query)) {
       fprintf(stderr, "Failed to make query %s\n%s\n",
               query, mysql_error(mysql));
    } else {
        if ( (result = mysql_store_result(mysql)) &&
             (num_rows = mysql_num_rows(result)) ) 
        {
            /* get ips in source and dest graphs */
            while ( (row = mysql_fetch_row(result)) ) {
                strncpy(ip, row[0], 15);
                
                /* don't insert source and dest ip */
                if (!strcmp(ip, source_ip) && !strcmp(ip, dest_ip)) {
                    snprintf(query, QUERY_MAX_SIZE,
                            "INSERT INTO graph VALUES (last_insert_id(), '%s';",
                            ip);
                    if (mysql_query(mysql, query)) {
                        fprintf(stderr, "Failed to make query %s\n%s\n",
                                query, mysql_error(mysql));
                    }
                }
            }
            mysql_free_result(result);
        }
    }
    
    /*
     * delete old graphs
     */
    snprintf(query, QUERY_MAX_SIZE,
             "DELETE FROM graph WHERE id = '%u' OR id = '%u';",
             source_graph_id, dest_graph_id);
    if (mysql_query(mysql, query)) {
        fprintf(stderr, "Failed to make query %s\n%s\n",
                query, mysql_error(mysql));
    }
    
    free(query);
    free(ip);
}


static void 
insert_ip_into_graph(MYSQL *mysql, unsigned int graph_id, char *ip) {

    char *query;

    query = (char *) malloc(sizeof(char) * QUERY_MAX_SIZE);
    snprintf(query, QUERY_MAX_SIZE,
             "INSERT INTO graph VALUES ('%u', '%s');", graph_id, ip);
    if (mysql_query(mysql, query)) {
        fprintf(stderr, "Failed to make query %s\n%s\n",
                query, mysql_error(mysql));
    }
    
    free(query);
}

static int get_graph_id(MYSQL *mysql, char *ip, int *graph_id) {
    
    MYSQL_RES   *result;
    MYSQL_ROW    row;
    unsigned int num_rows;
    char        *query;
    int          success = 0;

    query = (char *) malloc(sizeof(char) * QUERY_MAX_SIZE);
    snprintf(query, QUERY_MAX_SIZE, 
             "SELECT id FROM graph WHERE ip = '%s';", ip);
    
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
            *graph_id = atoi(row[0]);
            mysql_free_result(result);
            success = 1;
        } else {
            *graph_id = 0;
            success = 0;
        }
    }

    free(query);
    return success;
}



static void link(MYSQL *mysql, char *source_ip, char *dest_ip) {

    char         *query;
    MYSQL_RES    *result;
    unsigned int  num_rows;

    query = (char *) malloc (sizeof(char) * QUERY_MAX_SIZE);
    snprintf(query, QUERY_MAX_SIZE, 
             "SELECT * FROM link WHERE source = '%s' AND dest = '%s';", 
             source_ip, dest_ip);
    if (mysql_query(mysql, query)) {
        fprintf(stderr, "Failed to make query %s\n%s\n",
                query, mysql_error(mysql));
    } else {
        if ((result = mysql_store_result(mysql)) && 
             (num_rows = mysql_num_rows(result)) ) {
            snprintf(query, QUERY_MAX_SIZE,
                     "UPDATE link SET occurrences = occurrences + 1\
                      WHERE source = '%s' AND dest = '%s';",
                     source_ip, dest_ip);
            if (mysql_query(mysql, query)) {
                fprintf(stderr, "Failed to make query %s\n%s\n",
                        query, mysql_error(mysql));
            }
        } else {
            snprintf(query, QUERY_MAX_SIZE,
                     "INSERT INTO link VALUES ('%s', '%s', 1);",
                     source_ip, dest_ip);
            if (mysql_query(mysql, query)) {
                fprintf(stderr, "Failed to make query %s\n%s\n",
                        query, mysql_error(mysql));
            }
        }
    }
}

void graph (MYSQL *mysql, char *source_ip, char *dest_ip) {

    unsigned int source_graph_id = 0;
    unsigned int dest_graph_id = 0;
    
    /*
     * update links between source and dest ips
     */
    link(mysql, source_ip, dest_ip);

    get_graph_id(mysql, source_ip, &source_graph_id);
    get_graph_id(mysql, dest_ip, &dest_graph_id);

    /* 
     * source ip and dest ip are in the same graph 
     */
    if (source_graph_id && dest_graph_id) {
        if (!in_same_graph(mysql, source_ip, dest_ip)) {
            printf("Moving graphs %u & %u...\n", 
                    source_graph_id, dest_graph_id);
            move_graph(mysql, source_graph_id, dest_graph_id, 
                       source_ip, dest_ip);
        }
    }

    /*
     * only source ip is in a graph
     */
    else if (source_graph_id) {
        printf("Inserting %s in graph %d...\n", dest_ip, source_graph_id);
        insert_ip_into_graph(mysql, source_graph_id, dest_ip);
    } 

    /*
     * only dest ip is in a graph
     */
    else if (dest_graph_id) {
        printf("Inserting %s in graph %d...\n", source_ip, dest_graph_id);
        insert_ip_into_graph(mysql, dest_graph_id, source_ip);
    } 

    /*
     * neither source ip nor dest ip are in a graph
     */
    else {
        printf("Creating new graph with %s & %s...\n", source_ip, dest_ip);
        new_graph(mysql, source_ip, dest_ip);
    }
    
}


