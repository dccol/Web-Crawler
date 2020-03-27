
#ifndef CONNECTION_H
#define CONNECTION_H

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
#include "HTTPrequest.h"

#define MAX_RESPONSE_SIZE 100000

// creates a TCP connection between the client socket and the host server socket
void set_up_connection(parsed_url_t *host_server);

#endif