

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef URL_PARSE_H
#define URL_PARSE_H

typedef struct uri{
    char auth[1000];
    char path[1000];
} uri_t;


uri_t *parse_uri(char *url);

#endif