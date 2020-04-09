
#ifndef HEADER_PARSER_H
#define HEADER_PARSER_H

int get_content_length(char *head);

char *get_content_type(char *head);

int get_response_code(char *head);

#endif