#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_conf(char *option) {

    FILE *fd;
    char *line;
    char *val;

    line = (char *) malloc(sizeof(char) * 128);
    
    if (NULL == (fd = fopen(OSSIM_CONF, "r"))) {
        printf("Can't open file %s\n", OSSIM_CONF);
        exit (-1);
    }

    while(!feof(fd)) {
        fscanf(fd, "%s", line);
        if (strncmp(line, "#", 1)) {
            if (NULL != (val = strchr(line, '='))) {
                val++;
                if (!strncmp(option, line, val - line - 1)) {
                    return val;
                }
            }
        }
    }

    fclose(fd);
    
    return NULL;
}


