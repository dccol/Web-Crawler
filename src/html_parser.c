

#include "html_parser.h"

/*struct data {
    const char *url;
};*/

void search_for_links(GumboNode* node, deque_t *deque) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    GumboAttribute *href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, HREF))) {
        //printf("Link Found: %s\n", href->value);

        // if it breaks look into creating a copy
        // need to find a way to insert data which is not in any way connected to the Gumbo output
        // insert the link into the queue
        data_t *data = (data_t*)malloc(sizeof(data));
        char *url = malloc(sizeof(url) * strlen(href->value) + 1);
        strcpy(url, href->value);

        data->url = url;
        //printf("Inserting %s\n", data->url);
        deque_insert(deque, *data);
    }

    GumboVector *children = &node->v.element.children;
    for (int i = 0; i < children->length; ++i) {
        search_for_links((GumboNode*)children->data[i], deque);
    }
}
