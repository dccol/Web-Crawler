
/**
 * COMP30023 Computer Systems Project 1: Crawler
 *
 * Daniel Coleman, 994887
 *
 * Date: 9/04/2020
 */

#include <stdio.h>

#include "connection.h"


/* Take a url from command line and send a HTTP request to the server
 * Crawl this response for additional links
 * Store these links and repeat the process until all links hae been visited
*/
int main(int argc, char *argv[]) {

    char *url = (char*)malloc(sizeof(char) * (strlen(argv[1]) + 1));
    strcpy(url, argv[1]);

    deque_t *links = new_deque();
    deque_t *fetched_links = new_deque();

    set_up_connection(url, links, fetched_links);

    free(url);

    // take each link and (recurse the process)
    while(links->head != NULL){

        data_t data = deque_remove(links);

        set_up_connection(data.url, links, fetched_links);
    }

    free_deque(links);
    free_deque(fetched_links);

    return 0;
}






