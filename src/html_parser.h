
#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/gumbo/gumbo.h"
#include "deque.h"
#include "url_parse.h"

#define HREF "href"
#define HTTP "http://"
#define IMPLIED_PROTO 1
#define IMPLIED_AUTH 2
#define ABSOLUTE 3

void search_for_links(GumboNode* node, deque_t *deque, char *current_page_url, deque_t *fetched_links);

int validate_url_syn(const char* url);

int validate_url_authority(uri_t *base, char *href_value);

void add_to_queue(deque_t *deque, const char *href_value);

int queue_check(deque_t *fetched_links, const char *href_value);

void check_EOS(char *url);

void merge(char *b_auth, char *b_path, char *r_path, char *t);

void rfc_func(char *b_auth, char *b_path, const char *href_value, char *t);

void build_t(char *t, char *scheme, char *auth, char *path);

int determine_relative_type(const char *r, char **r_auth, char *r_path);


#endif


