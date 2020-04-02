
#include "html_parser.h"

void search_for_links(GumboNode* node, deque_t *deque, char *current_page_url) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    GumboAttribute *href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, HREF))) {
        //printf("Link Found: %s\n", href->value);

        // do some checks for ignorable characters
        // "./" , "../" , "?" , "#" , "%"
        if(validate_url_syn(href->value)) {

            // fill in the rest of the url
            // if the url does not contain the scheme, build upon the current url
            if(!strstr(href->value, "http")) {
                char built_url[1000];
                build_url(current_page_url, built_url, href->value);

                // validate authority match
                if(validate_url_authority(parse_url(current_page_url), built_url) == 1) {

                    // insert the link into the queue
                    add_to_queue(deque, built_url);
                }
            }
            else {
                char href_copy[1000];
                strcpy(href_copy, href->value);
                if(validate_url_authority(parse_url(current_page_url), href_copy) == 1) {

                    // insert the link into the queue
                    add_to_queue(deque, href->value);
                }
            }
        }
    }

    // continue search
    GumboVector *children = &node->v.element.children;
    for (int i = 0; i < children->length; ++i) {
        search_for_links((GumboNode*)children->data[i], deque, current_page_url);
    }
}

int validate_url_syn(const char* url){
    // check for ignorable characters in url
    if(strstr(url, "./")){
        return 0;
    }
    if(strstr(url, "..")){
        return 0;
    }
    if(strstr(url, "?")){
        return 0;
    }
    if(strstr(url, "#")){
        return 0;
    }
    if(strstr(url, "%")){
        return 0;
    }
    if(strstr(url, "mailto")){
        return  0;
    }
    if(strstr(url, "ftp://")){
        return 0;
    }
    return 1;
}

// if all but the first component match, return 1
int validate_url_authority(parsed_url_t *current_url_info, char *href_value){

    parsed_url_t *href_info = parse_url(href_value);

    //printf("Current host name: %s\n", current_url_info->host);
    //printf("Href host name: %s\n", href_info->host);


    // split remove the first component
    char *current_split = strchr(current_url_info->host, '.');
    char *href_split = strchr(href_info->host, '.');

    //printf("The current: %s\n", current_split);
    //printf("The href: %s\n", href_split);

    // if the rest of the components are the same => success
    if((strcmp(current_split, href_split) == 0)){
        return 1;
    }
    // otherwise failure
    else{
        return -1;
    }
}

void build_url(char *current_page_url, char *built_url, const char *href){

    // take the current page url and subtract everything after the right most '/'
    char *output = strrchr(current_page_url, '/');
    //printf("The fluff at the end is: %s\n", output);

    // add the path if incomplete
    char *current_copy = (char *)malloc(sizeof(current_copy));
    strcpy(built_url, current_page_url);
    strcat(built_url, href);
    //printf("The built url: %s\n", built_url);

}

void add_to_queue(deque_t *deque, const char *href_value){
    // insert the link into the queue
    data_t *data = (data_t *) malloc(sizeof(data));
    char *url = malloc(sizeof(url) * strlen(href_value) + 1);
    strcpy(url, href_value);

    data->url = url;
    //printf("Inserting %s\n\n", data->url);
    deque_insert(deque, *data);
}





