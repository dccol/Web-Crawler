
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

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

void generate_request(parsed_url_t *host_server, char *request);

void send_receive(int client_socket, char *request, parsed_url_t *host_server, char *response);

#endif