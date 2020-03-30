#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include <unistd.h>

#include "../lib/url_parser/url_parser.h"
#include "../lib/gumbo/gumbo.h"

#include "connection.h"

// TO DO
// Parse URL into into domain name and path X
// Use this struct to build the initial request X
// Parse the HTML response for <a> tags
// If path is different to current page -> request the new page




int main(int argc, char *argv[]) {
    parsed_url_t *host_server = parse_url(argv[1]);

    int client_socket;
    //char request[1024];
    //char response[MAX_RESPONSE_SIZE+1];
    deque_t *links = new_deque();

    set_up_connection(argv[1], client_socket, links);

    //send_receive(client_socket, request, host_server, response);
    //printf("The request:\n%s", request);
    //printf("Response from the server:\n%s\n", response);
    return 0;
}



