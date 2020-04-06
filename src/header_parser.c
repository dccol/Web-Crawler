

#include "header_parser.h"

#include <string.h>
#include <stdlib.h>

int get_content_length(char *head){
    // get content-length from head

    char *where_content_length_is = strstr(head, "Content-Length:");
    if(where_content_length_is == NULL){
        where_content_length_is = strstr(head, "content-length:");
    }

    char *token = strtok(where_content_length_is, "\n");
    //printf("\n%s\n", token);

    token = strtok(token, " ");
    //printf("\n%s\n", token);

    token = strtok(NULL, " ");
    //printf("\n%s\n\n", token);

    if(token != NULL){
        int content_length = atoi(token);
        return content_length;
    }
    //printf("Content Length: %d\n", content_length);

    return -1;
}

char *get_content_type(char *head){
    char *where_content_type_is = strstr(head, "Content-Type:");
    if(where_content_type_is == NULL){
        where_content_type_is = strstr(head, "content-type:");
    }
    char *token = strtok(where_content_type_is, "\n");
    //printf("\n%s\n", token);

    token = strtok(token, " ");
    //printf("\n%s\n", token);

    token = strtok(NULL, " ");
    //printf("\n%s\n\n", token);

    return token;
}

int get_response_code(char *head){
    char *token = strtok(head, " ");

    token = strtok(NULL, " ");

    int code = atoi(token);

    return code;
}