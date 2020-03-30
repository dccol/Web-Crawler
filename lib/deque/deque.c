
/* Deque Module
 * Written by Daniel Coleman 2019*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "deque.h"


/*struct data {
	const char *url;
};

struct node {
	data_t data;
	node_t *next;
    node_t *prev;
};

struct deque {
    int size;
    node_t *head;
	node_t *foot;
};*/


deque_t *new_deque() {
    deque_t *deque;
    deque = (deque_t*)malloc(sizeof(*deque));
    assert(deque!=NULL);
    deque->head = deque->foot = NULL;
    deque->size = 0;
    return deque;
}

void free_deque(deque_t *deque) {
    node_t *curr, *prev;
    assert(deque!=NULL);
    curr = deque->head;
    while (curr) {
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    free(deque);
    return;
}

void deque_push(deque_t *deque, data_t data) {
    node_t *new;
    new = (node_t*)malloc(sizeof(*new));
    assert(deque!=NULL && new!=NULL);
    new->data = data;
    new->next = NULL;
    new->prev = NULL;
    if (deque->foot==NULL) {
        deque->foot = new;
        deque->head = new;
    }
    else {
        deque->foot->next = new;
        new->prev = deque->foot;
        deque->foot = new;
    }
    deque->size++;
    return;
}

void deque_insert(deque_t *deque, data_t data) {
    node_t *new;
    new = (node_t*)malloc(sizeof(*new));
    assert(deque!=NULL && new!=NULL);
    new->data = data;
    new->next = deque->head;
    new->prev = NULL;
    if (deque->foot==NULL) {
      deque->head = deque->foot = new;
    }
    deque->head->prev = new;
    deque->head = new;
    deque->size++;
    return;
}

data_t deque_pop(deque_t *deque) {
    assert(deque!=NULL && deque->foot!=NULL);
    data_t new;
    new.url = deque->foot->data.url;
    deque->foot = deque->foot->prev;
    if(deque->foot == NULL){
        deque->head = NULL;
    }
    else{
        deque->foot->next = NULL;
    }
    deque->size--;
    return new;
    fprintf(stderr, "Error: deque_pop() not implemented\n");
    exit(EXIT_FAILURE);
}

data_t deque_remove(deque_t *deque) {
    assert(deque!=NULL && deque->head != NULL);
    data_t new;
    new.url = deque->head->data.url;
    deque->head = deque->head->next;
    if(deque->head == NULL){
        deque->foot = NULL;
    }
    else{
        deque->head->prev = NULL;
    }
    deque->size--;
    return new;
}

int deque_size(deque_t *deque) {
    return deque->size;
}