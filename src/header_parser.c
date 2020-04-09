
#include "header_parser.h"

#include <string.h>
#include <stdlib.h>

/*
 * Parses through HTTP headers to retrieve the content length of the response
 */
int get_content_length(char *head){

    char *where_content_length_is = strstr(head, "Content-Length:");
    if(where_content_length_is == NULL){
        where_content_length_is = strstr(head, "content-length:");
    }

    char *token = strtok(where_content_length_is, "\n");

    token = strtok(token, " ");

    token = strtok(NULL, " ");

    if(token != NULL){
        int content_length = atoi(token);
        return content_length;
    }

    return -1;
}

/*
 * Parses through HTTP headers to retrieve the content type of the response
 */
char *get_content_type(char *head){

    char *where_content_type_is = strstr(head, "Content-Type:");
    if(where_content_type_is == NULL){
        where_content_type_is = strstr(head, "content-type:");
    }
    char *token = strtok(where_content_type_is, "\n");

    token = strtok(token, " ");

    token = strtok(NULL, " ");

    return token;
}

/*
 * Parses through HTTP headers to retrieve the code of the response
 */
int get_response_code(char *head){
    char *token = strtok(head, " ");

    token = strtok(NULL, " ");

    int code = atoi(token);

    return code;
}