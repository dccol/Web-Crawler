
#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/url_parser.h"

void generate_request(parsed_url_t *host_server, char *request);

#endif