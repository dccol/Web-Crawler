
#include "HTTPrequest.h"


void generate_request(uri_t *base, char *request){
    strcat(request, "GET ");
    strcat(request, base->path);
    strcat(request, " HTTP/1.1\r\n");

    // add Host: header
    strcat(request, "Host: ");
    strcat(request, base->auth);
    strcat(request, "\r\n");

    // add User-Agent: header
    char * user_agent = "dccol";
    strcat(request, "User-Agent: ");
    strcat(request, user_agent);
    strcat(request, "\r\n");

    char * connection = "close";
    strcat(request, "Connection: ");
    strcat(request, connection);
    strcat(request, "\r\n\r\n");
}
