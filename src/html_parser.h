
#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/gumbo/gumbo.h"
#include "../lib/deque/deque.h"

#define HREF "href"

void search_for_links(GumboNode* node, deque_t *deque, char *current_page_url);

int validate_url_syn(const char* url);

int validate_url_authority(parsed_url_t *current_url_info, char *href_value);

void build_url(char *current_page_url, char *built_url, const char *href);

void add_to_queue(deque_t *deque, const char *href_value);


#endif


