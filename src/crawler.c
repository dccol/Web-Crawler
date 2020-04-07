#include <stdio.h>


#include "connection.h"

uri_t *parse_uri(char *url);

// TO DO
// Parse URL into into domain name and path X
// Use this struct to build the initial request X
// Parse the HTML response for <a> tags
// If path is different to current page -> request the new page

// appends a trailing '/' to the given url

int main(int argc, char *argv[]) {

    char *url = (char*)malloc(sizeof(char) * strlen(argv[1]) + 1);
    strcpy(url, argv[1]);

    check_EOS(url);
    //printf("The updated url :%s\n", url);

    deque_t *links = new_deque();
    deque_t *fetched_links = new_deque();

    set_up_connection(url, links, fetched_links);

    // take each link and (recurse the process)
    while(links->head != NULL){

        // get the next link from the queue
        data_t data = deque_pop(links);
        //printf("Next link: %80s\n", data.url);

        set_up_connection(data.url, links, fetched_links);
    }

    free_deque(links);
    free_deque(fetched_links);

    printf("All links crawled\n");
    return 0;
}






