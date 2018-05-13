//
// Created by root on 5/11/18.
//

#ifndef TRACER_LLIST_H
#define TRACER_LLIST_H

/*
 * The linked list structure :
 *
 *  It contains pointer for previous and next;
 */

typedef struct {

    //The previous element;
    linked_element_t *prev;

    //The next element;
    linked_element_t *next;

} linked_element_t;


#define EMPTY_LINKED_ELEMENT() {.prev = 0, .next = 0}

//Concatenate two linked lists elements;
void llist_concat(linked_element_t *prev, linked_element_t *next);

//Inserts new after prev;
void llist_insert_after(linked_element_t *src_prev, linked_element_t *new);

//Inserts new before next;
void llist_insert_before(linked_element_t *src_next, linked_element_t *new);

//Executes process for all elements after the provided one:
void llist_direct_process(linked_element_t *element, void (*process)(linked_element_t *));

//Executes process for all elements in the chain starting with the first one;
void llist_direct_process_full(linked_element_t *element, void (*process)(linked_element_t *));

//Propagate spaces (trigger all streams in the opposite of data's direction order);
void llist_reverse_process(linked_element_t *element, void (*process)(linked_element_t *));

//Propagate data through the entire stream chain;
void llist_reverse_process_full(linked_element_t *element, void (*process)(linked_element_t *));



#endif //TRACER_LLIST_H
