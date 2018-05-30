//
// Created by root on 5/11/18.
//

#ifndef TRACER_LLIST_H
#define TRACER_LLIST_H


#include <stddef.h>

#include <stdbool.h>

/*
 * ------------------------------ Linked elements ------------------------------
 */

/*
 * The linked element structure :
 *
 *  It contains pointer for previous and next;
 */

typedef struct linked_element_t {

    //The previous element;
    struct linked_element_t *prev;

    //The next element;
    struct linked_element_t *next;

} linked_element_t;

//Initializer;
#define EMPTY_LINKED_ELEMENT() {.prev = 0, .next = 0}

//Concatenate two linked lists elements;
void lelmt_concat(linked_element_t *prev, linked_element_t *next);

//Inserts new after prev;
void lelmt_insert_after(linked_element_t *src_prev, linked_element_t *new_element);

//Inserts new before next;
void lelmt_insert_before(linked_element_t *src_next, linked_element_t *new_element);


/*
 * ------------------------------ Linked lists ------------------------------
 */

/*
 * The linked list is composed of the two extremal linked elements, and of the element counter;
 */

typedef struct {

    //The list's first element;
    linked_element_t *first;

    //The list's last element;
    linked_element_t *last;

    //The number of elements;
    size_t nb_elements;

    //The maximum number of elements;
    size_t max_nb_elements;

} linked_list_t ;

//Initializer
#define EMPTY_LINKED_LIST(max_nb) (linked_list_t) {.first = 0, .last = 0, .nb_elements = 0, .max_nb_elements = (max_nb)}


//Verify that an element can be inserted;
inline bool llist_insertion_available(const linked_list_t *const list) {
    return list->nb_elements < list->max_nb_elements;
}

//Insert the new element at the beginning of the list;
void llist_insert_begin(linked_list_t *list, linked_element_t *element);

//Insert the new element at the end of the list;
void llist_insert_end(linked_list_t *list, linked_element_t *element);

//Insert the new element after an element of the list;
void llist_insert_after(linked_list_t *list, linked_element_t *src, linked_element_t *element);

//Insert the new element before an element of the list;
void llist_insert_before(linked_list_t *list, linked_element_t *src, linked_element_t *element);

//Remove the first element of the list;
linked_element_t *llist_remove_begin(linked_list_t *list);

//Remove an element of the list;
void llist_remove_element(linked_list_t *list, linked_element_t *element);

//Remove the last element of the list;
linked_element_t *llist_remove_end(linked_list_t *list);


/*
 * ------------------------------ Linked ring ------------------------------
 */

/*
 * The linked ring is composed of the current element, and of the element counter;
 */

typedef struct {

    //The list's first element;
    linked_element_t *current;

    //The number of elements;
    size_t nb_elements;

    //The maximum number of elements;
    size_t max_nb_elements;

} linked_ring_t ;

//Initialize;
#define EMPTY_LINKED_RING(max_nb) {.current = 0, .nb_elements = 0, .max_nb_elements = (max_nb)}

//Verify that an element can be inserted;
inline bool lring_insertion_available(linked_ring_t *ring) {
    return ring->nb_elements < ring->max_nb_elements;
}

//Insert an element before the current element of the ring (last position);
void lring_insert_after(linked_ring_t *ring, linked_element_t *element);

//Insert an element after the current element of the ring (next position);
void lring_insert_before(linked_ring_t *ring, linked_element_t *element);

//Focus on the next element;
void lring_incr(linked_ring_t *ring);

//Focus on the previous element;
void lring_decr(linked_ring_t *ring);

/*

//Executes process for all elements after the provided one:
void llist_direct_process(linked_element_t *element, void (*process)(linked_element_t *));

//Executes process for all elements in the chain starting with the first one;
void llist_direct_process_full(linked_element_t *element, void (*process)(linked_element_t *));

//Propagate spaces (trigger all streams in the opposite of data's direction order);
void llist_reverse_process(linked_element_t *element, void (*process)(linked_element_t *));

//Propagate data through the entire stream chain;
void llist_reverse_process_full(linked_element_t *element, void (*process)(linked_element_t *));


 */

#endif //TRACER_LLIST_H
