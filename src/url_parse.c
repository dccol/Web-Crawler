
#include "url_parse.h"

/*
 * Take a url and parse it into its authority and path components
 */
uri_t *parse_uri(char *url){

    char regex_str[] = "http://%[^/]%[^\n]";
    uri_t *base = (uri_t*)malloc(sizeof(*base));
    assert(base != NULL);

    sscanf(url, regex_str, base->auth, base->path);

    if(base->path[0] == '\0'){
        strcat(base->path, "/");
    }
    return base;
}

