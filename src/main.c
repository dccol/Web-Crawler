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

#include "../lib/url_parser.h"

#include "connection.h"

// TO DO
// Parse URL into into domain name and path
// Use this struct to build the initial request
// Parse the HTML response for <a> tags
// If path is different to current page -> request the new page


//parsed_url_t* url_parse(char *inputURL);
//void generateRequest();
//void parseHTML();

int main(int argc, char *argv[]) {
    parsed_url_t *urlT = parse_url(argv[1]);
    set_up_connection(urlT);
    return 0;
}

/*parsed_url_t* url_parse(char *inputURL) {

    parsed_url_t *url = parse_url(inputURL);

    // print host server information
    printf("The Host Name: %s\n", url->host);
    if(url->path != NULL){
        printf("The Path: %s\n\n", url->path);
    }
    if(url->port){
        printf("The Port: %s\n\n", url->port);
    }

    return url;
}*/



