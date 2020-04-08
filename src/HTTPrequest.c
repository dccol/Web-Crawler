
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
    strcat(request, "User-Agent: ");
    strcat(request, "dccol\r\n");

    // add Connection: header
    strcat(request, "Connection: ");
    strcat(request, "close\r\n\r\n");
}
