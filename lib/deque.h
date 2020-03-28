
/* Deque Module
 * Written by Daniel Coleman 2019*/

#ifndef DEQUE_H
#define DEQUE_H

#include "url_parser.h"

typedef struct data data_t;

typedef struct deque deque_t;

typedef struct node node_t;

deque_t *new_deque();

void free_deque(deque_t *deque);

void deque_push(deque_t *deque, data_t data);

void deque_insert(deque_t *deque, data_t data);

data_t deque_pop(deque_t *deque);

data_t deque_remove(deque_t *deque);

int deque_size(deque_t *deque);

#endif