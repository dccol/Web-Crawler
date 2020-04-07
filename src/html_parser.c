
#include "html_parser.h"

void search_for_links(GumboNode* node, deque_t *queued_links, char *current_page_url, deque_t *fetched_links) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    GumboAttribute *href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, HREF))) {
        //printf("\nLink Found: %s\n", href->value);

        // if the link has the accepted syntax continue
        if(validate_url_syn(href->value)) {

            // create the uri to add to the queue (t)
            char t[1000];
            bzero(t, sizeof(t));

            uri_t *base = parse_uri(current_page_url);

            /*if(strcmp(base->path, "") == 0){
                strcat(base->path, "/");
            }*/

            // be aware that the path must begin with the '/' for this to work,
            // hence when you change the url library ensure the path always has the '/' to begin
            //printf("href_value: %s\n", href->value);
            rfc_func(base->auth, base->path, href->value, t);
            //printf("T: %s\n", t);

            // validate authority match
            if(validate_url_authority(base, t) == 1) {

                // if the url has not already been fetched AND if the url is not currently in the queue
                if((queue_check(fetched_links, t) == 1) && (queue_check(queued_links, t) == 1)) {

                    // add a trailing '/'
                    //check_EOS(t);

                    // insert the link into the queue
                    add_to_queue(queued_links, t);


                }
            }
            free(base);
        }

    }


    // continue search
    GumboVector *children = &node->v.element.children;
    for (int i = 0; i < children->length; ++i) {
        search_for_links((GumboNode*)children->data[i], queued_links, current_page_url, fetched_links);
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
    if(strstr(url, "ftp")){
        return 0;
    }
    if(strstr(url, "https://")){
        return 0;
    }
    return 1;
}

// if all but the first component match, return 1
int validate_url_authority(uri_t *base, char *href_value){

    uri_t *href_info = parse_uri(href_value);
    //printf("The href url: %s\n", href_value);

    //printf("Current host name: %s\n", current_url_info->host);
    //printf("Href host name: %s\n", href_info->host);

    // split remove the first component
    char *base_split = strchr(base->auth, '.');
    char *href_split = strchr(href_info->auth, '.');

    //printf("The current: %s\n", current_split);
    //printf("The href: %s\n\n", href_split);

    // if the rest of the components are the same => success
    if((strcmp(base_split, href_split) == 0)){
        free(href_info);
        return 1;
    }
    // otherwise failure
    else{
        free(href_info);
        return -1;
    }
}

void add_to_queue(deque_t *deque, const char *href_value){
    // insert the link into the queue
    data_t *data = (data_t *) malloc(sizeof(*data));
    char *url = malloc(sizeof(*url) * (strlen(href_value) + 1));
    strcpy(url, href_value);

    data->url = url;
    //printf("Inserting %s\n\n", data->url);
    deque_insert(deque, *data);
}

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

// maybe only for end of authority at beginning of program
void check_EOS(char *url){

    // if there is content after the right most '/' segment => signifies there is no trailing '/'
    //printf("%s\n", strrchr(url, '/'));
    if(strlen(strrchr(url, '/')) > 1){
        // add trailing '/'
        strcat(url, "/");
    }

}


void merge(char *b_auth, char *b_path, char *r_path, char *t_path){

    // option 1 - base has no path
    if(b_auth != NULL && strcmp(b_path, "/") == 0){
        strcat(t_path, "/");
        strcat(t_path, r_path);
        //printf("The result of merge: %s\n", t_path);
        return;
    }
        // option 2 -
    else{
        // if there is content after the right-most '/' segment => signifies there is no trailing '/'
        // if there is no trailing '/', then we remove whatever content strrchr() returns
        if(strlen(strrchr(b_path, '/')) > 1){
            // remove strrchr() from b_path

            //printf("basepath: %s\n", b_path);

            char* all_but_last = (char *)malloc(sizeof(char) * strlen(b_path) + 1);
            bzero(all_but_last, (sizeof(char) * strlen(b_path) + 1));

            int all_but_last_len = strlen(b_path) - strlen(strrchr(b_path, '/')) + 1; // = 5
            //printf("allbutlast len : %d\n", all_but_last_len);
            strncat(all_but_last, b_path, all_but_last_len);
            all_but_last[strlen(all_but_last)] = '\0';
            //printf("All but last: %s\n", all_but_last);

            // strcat all but last into t, strcat r_path into t
            strcat(t_path, all_but_last);
            strcat(t_path, r_path);
            //printf("The result of merge: %s\n", t_path);
            free(all_but_last);
        }
            // otherwise just append straight on
        else{
            strcat(t_path, b_path);
            strcat(t_path, r_path);
        }
    }
}

void rfc_func(char *b_auth, char *b_path, const char *r, char *t){
    // gonna need a way to proper split up the hrefs
    char *r_auth = NULL;
    char *r_path = (char *)malloc(sizeof(char) * 1000);
    bzero(r_path, (sizeof(char) * 1000));

    char t_scheme[] = HTTP;

    char t_auth[100];
    bzero(t_auth, sizeof(t_auth));

    char t_path[1000];
    bzero(t_path, sizeof(t_path));

    int type = determine_relative_type(r, &r_auth, r_path);
    //printf("the type: %d\n", type);
    //printf("r_auth: %s\n", r_auth);
    //printf("r_path: %s\n\n", r_path);

    // if r is ABSOLUTE
    if(strstr(r, HTTP)){
        bzero(t_auth, sizeof(t_auth));
        bzero(t_path, sizeof(t_path));

        strcat(t_auth, r_auth);
        //printf("t_auth: %s\n\n", t_auth);
        //printf("t_path: %s\n\n", t_path);
        //printf("r_path: %s\n\n", r_path);
        strcat(t_path, r_path);
        //printf("t_path: %s\n\n", t_path);

        // built t
        build_t(t, t_scheme, t_auth, t_path);

        free(r_auth);
        free(r_path);
        return;
    }
    else { // if R has no scheme

        // if R is IMPLIED PROTOCOL
        if (type == IMPLIED_PROTO) { //2a
            strcat(t_auth, r_auth);
            strcat(t_path, r_path);

        }
            // if R is IMPLIED AUTHORITY + PROTOCOL
        else { //2b

            // if the path is empty
            if (strlen(r_path) < 1) {// 3a
                strcat(t_path, b_path);
            }

            else { //3b if there is a path

                if (strncmp(r_path, "/", sizeof(char)) == 0) {
                    strcat(t_path, r_path);
                } else {
                    merge(b_auth, b_path, r_path, t_path);
                    //printf("Merge: %s\n", t_path);
                }
            }
            strcat(t_auth, b_auth);
        }
    }
    build_t(t, t_scheme, t_auth, t_path);

    // potential break idk
    if(r_auth){
        free(r_auth);
    }
    free(r_path);
}


// check this lots of the urls have double // in the test env
int determine_relative_type(const char *r, char **r_auth, char *r_path){

    // if no scheme in r
    if(strstr(r, HTTP) == NULL){

        char tmp_r_implied_protocol[10];
        bzero(tmp_r_implied_protocol, sizeof(tmp_r_implied_protocol));
        // if authority in r
        if(strstr(r, "//")){
            char regex_implied_protocol[] = "//%[^/]%[^\n]";
            *r_auth = (char *)malloc(sizeof(char) * 100);
            sscanf(r, regex_implied_protocol, *r_auth, r_path);
            //printf("r_auth in func: %s\n", *r_auth);
            //printf("r_path: %s\n", r_path);
            return IMPLIED_PROTO;
        }
            // if no "//" then its implied protocol and authority
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
    //printf("r_auth: %s\n", r_auth);
    //printf("r_path: %s\n", r_path);
}

void build_t(char *t, char *scheme, char *auth, char *path){

    //printf("The current T: %s\n\n", t);
    strcat(t, scheme);
    //printf("scheme T: %s\n\n", t);
    strcat(t, auth);
    //printf("scheme auth T: %s\n\n", t);
    strcat(t, path);
    //printf("scheme auth path T: %s\n\n", t);
}





