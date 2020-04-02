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

// appends a trailing '/' to the given url
void check_EOS(char *url, parsed_url_t *host_info);

int main(int argc, char *argv[]) {
    char *url = argv[1];
    parsed_url_t *host_server = parse_url(url);

    check_EOS(url, host_server);
    printf("The updated url :%s\n", url);

    int client_socket;
    //char request[1024];
    //char response[MAX_RESPONSE_SIZE+1];
    deque_t *links = new_deque();

    set_up_connection(url, client_socket, links);

    //send_receive(client_socket, request, host_server, response);
    //printf("The request:\n%s", request);
    //printf("Response from the server:\n%s\n", response);
    return 0;
}

// not true for urls which have paths, fix this
void check_EOS(char *url, parsed_url_t *host_info){

    // if the host name appears in the right most '/' segment => signifies there is no trailing '/'
    if(strstr(strrchr(url, '/'), host_info->host)){
        // add trailing '/'
        strcat(url, "/");
    }

}




