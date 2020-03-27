
#include "HTTPrequest.h"


void generate_request(parsed_url_t *host_server, char *request){
    strcat(request, "GET /");
    if(host_server->path != NULL){
        strcat(request, host_server->path);
    }
    strcat(request, " HTTP/1.1\r\n");

    // add Host: header
    char* host_name = host_server->host;
    strcat(request, "Host: ");
    strcat(request, host_name);
    strcat(request, "\r\n");

    // add User-Agent: header
    char * user_agent = "dccol";
    strcat(request, "User-Agent: ");
    strcat(request, user_agent);
    strcat(request, "\r\n\r\n");

}

