
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


#include "../lib/gumbo/gumbo.h"
#include "HTTPrequest.h"
#include "html_parser.h"

#define MAX_RESPONSE_SIZE 100000
#define REQUEST_SIZE 300
#define BLANK_LINE_DELIM "\r\n\r\n"



// creates a TCP connection between the client socket and the host server socket
void set_up_connection(char *url, deque_t *links, deque_t *fetched_links);

int get_content_length(char *head);

char *get_content_type(char *head);

int get_response_code(char *head);

#endif