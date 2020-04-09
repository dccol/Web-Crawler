
#include "html_parser.h"

/*
 * Searches DOM for A tags and adds the resolved (absolute) urls into a queue to be fetched
 */
void search_for_links(GumboNode* node, deque_t *queued_links, char *current_page_url, deque_t *fetched_links) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    GumboAttribute *href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, HREF))) {

        // check whether href value has 'ignorable' characters in the content of this assignment
        if(validate_url_syn(href->value) == 1) {

            // the result of the resolution
            char t[1000];
            bzero(t, sizeof(t));

            uri_t *base = parse_uri(current_page_url);

            // resolve the href value into an absolute url
            rfc_func(base->auth, base->path, href->value, t);

            // check whether all but the first component of authority of resolved url matches base
            if(validate_url_authority(base, t) == 1) {

                // if the url has not already been fetched AND the url is not currently in the queue
                if((queue_check(fetched_links, t) == 1) && (queue_check(queued_links, t) == 1)) {

                    // insert the link into the queue
                    add_to_queue(queued_links, t);
                }
            }
            free(base);
        }
    }

    // continue search down the DOM
    GumboVector *children = &node->v.element.children;
    for (int i = 0; i < children->length; ++i) {
        search_for_links((GumboNode*)children->data[i], queued_links, current_page_url, fetched_links);
    }
}

/*
 * Checks a string for any ignorable characters
 */
int validate_url_syn(const char* url){

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
    if(strstr(url, "ftp")){
        return 0;
    }
    if(strstr(url, "https://")){
        return 0;
    }
    return 1;
}

/*
 * Checks whether all but the first components of a resolved authority match the bases
 */
int validate_url_authority(uri_t *base, char *href_value){

    uri_t *href = parse_uri(href_value);

    // split remove the first component
    char *base_split = strchr(base->auth, '.');
    char *href_split = strchr(href->auth, '.');

    // if the rest of the components are the same => success
    if((strcmp(base_split, href_split) == 0)){
        free(href);
        return 1;
    }
    // otherwise failure
    else{
        free(href);
        return -1;
    }
}

/*
 * adds a resolved href value into a queue
 */
void add_to_queue(deque_t *deque, const char *href_value){

    data_t *data = (data_t *) malloc(sizeof(*data));
    char *url = malloc(sizeof(*url) * (strlen(href_value) + 1));
    strcpy(url, href_value);

    data->url = url;

    deque_insert(deque, *data);
}

/*
 * Searches a queue for a url
 */
int queue_check(deque_t *fetched_links, const char *href_value){

    data_t *data = (data_t *) malloc(sizeof(*data));
    char *url = malloc(sizeof(url) * (strlen(href_value) + 1));
    strcpy(url, href_value);

    data->url = url;

    node_t *curr = fetched_links->head;
    while(curr != NULL){

        // if the urls are the same, return -1 failure
        if(strcmp(data->url, curr->data.url) == 0){
            free(data);
            free(url);
            return -1;
        }
        else{
            curr = curr->next;
        }
    }
    // if compared all fetched urls and no match, return 1 success
    free(data);
    free(url);
    return 1;
}

/*
 * Merges a Base uri's path with a Relative uri's path
 */
void merge(char *b_auth, char *b_path, char *r_path, char *t_path){

    // option 1 - base has no path
    if(b_auth != NULL && strcmp(b_path, "/") == 0){

        strcat(t_path, "/");
        strcat(t_path, r_path);
        return;
    }
    // option 2
    else{
        // if there is content after the right-most '/' segment => signifies there is no trailing '/'
        // if there is no trailing '/', then we remove whatever content strrchr() returns
        if(strlen(strrchr(b_path, '/')) > 1){

            char* all_but_last = (char *)malloc(sizeof(char) * strlen(b_path) + 1);
            bzero(all_but_last, (sizeof(char) * strlen(b_path) + 1));

            int all_but_last_len = strlen(b_path) - strlen(strrchr(b_path, '/')) + 1;

            strncat(all_but_last, b_path, all_but_last_len);
            all_but_last[strlen(all_but_last)] = '\0';

            strcat(t_path, all_but_last);
            strcat(t_path, r_path);

            free(all_but_last);
        }
        // otherwise append straight on to the end, no remove snip required
        else{
            strcat(t_path, b_path);
            strcat(t_path, r_path);
        }
    }
}

/*
 * Resolves href values into absolute urls, as per RFC specifications
 */
void rfc_func(char *b_auth, char *b_path, const char *r, char *t){

    // r = Reference, b = Base, t = Target

    char *r_auth = NULL;
    char *r_path = (char *)malloc(sizeof(char) * 1000);
    bzero(r_path, (sizeof(char) * 1000));

    char t_scheme[] = HTTP;

    char t_auth[100];
    bzero(t_auth, sizeof(t_auth));

    char t_path[1000];
    bzero(t_path, sizeof(t_path));

    int type = determine_relative_type(r, &r_auth, r_path);

    // if r is ABSOLUTE
    if(strstr(r, HTTP)){
        bzero(t_auth, sizeof(t_auth));
        bzero(t_path, sizeof(t_path));

        strcat(t_auth, r_auth);
        strcat(t_path, r_path);

        build_t(t, t_scheme, t_auth, t_path);

        free(r_auth);
        free(r_path);
        return;
    }
    // if r has no scheme
    else {
        // if r is IMPLIED PROTOCOL
        if (type == IMPLIED_PROTO) {
            strcat(t_auth, r_auth);
            strcat(t_path, r_path);
        }
        // if r is IMPLIED AUTHORITY + PROTOCOL
        else {
            // if the path is empty
            if (strlen(r_path) < 1) {
                strcat(t_path, b_path);
            }
            //if there is a path
            else {

                if (strncmp(r_path, "/", sizeof(char)) == 0) {
                    strcat(t_path, r_path);
                } else {
                    merge(b_auth, b_path, r_path, t_path);
                }
            }
            strcat(t_auth, b_auth);
        }
    }
    build_t(t, t_scheme, t_auth, t_path);

    if(r_auth){
        free(r_auth);
    }
    free(r_path);
}


/*
 * Determines the href values reference type
 */
int determine_relative_type(const char *r, char **r_auth, char *r_path){

    // if no scheme in r
    if(strstr(r, HTTP) == NULL){

        char tmp_r_implied_protocol[10];
        bzero(tmp_r_implied_protocol, sizeof(tmp_r_implied_protocol));

        // if authority in r
        if(strstr(r, "//") == r){

            char regex_implied_protocol[] = "//%[^/]%[^\n]";
            *r_auth = (char *)malloc(sizeof(char) * 100);
            sscanf(r, regex_implied_protocol, *r_auth, r_path);

            return IMPLIED_PROTO;
        }

        // if no scheme or authority, then its implied protocol and authority
        else {
            strcat(r_path, r);
            return IMPLIED_AUTH;
        }
    }
    // if there is a scheme
    else {

        char regex_str[] = "http://%[^/]%[^\n]";
        *r_auth = (char *)malloc(sizeof(char) * 100);
        sscanf(r, regex_str, *r_auth, r_path);
        return ABSOLUTE;
    }
}

/*
 * Takes the components of the target url and builds the target
 */
void build_t(char *t, char *scheme, char *auth, char *path){

    strcat(t, scheme);

    strcat(t, auth);

    strcat(t, path);

}





