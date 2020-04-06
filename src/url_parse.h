

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef URL_PARSE_H
#define URL_PARSE_H

typedef struct uri{
    char auth[100];
    char path[100];
} uri_t;


uri_t *parse_uri(char *url);

#endif