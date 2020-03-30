#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/gumbo/gumbo.h"
#include "../lib/deque/deque.h"

#define HREF "href"

void search_for_links(GumboNode* node, deque_t *deque);