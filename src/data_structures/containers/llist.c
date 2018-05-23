//
// Created by root on 5/11/18.
//

#include "llist.h"


/*
 * llist_concat : concatenates the couple of elements; If their predecessors / successors are allocated, error;
 */

void llist_concat(linked_element_t *prev, linked_element_t *next) {

    //First, if the first's output or the second's input is already allocated :
    if (prev->next || next->prev) {

        //Error. Association must not overwrite previous associations;
        return;//TODO EXCEPTION;

    }

    //The next of the first stream must be the second;
    prev->next = next;

    //The previous of the second string must be the first;
    next->prev = prev;

}


/*
 * llist_get_last : searches for the last element in the list;
 */

linked_element_t *llist_get_last(linked_element_t *element) {

    //Cache the next element;
    linked_element_t *next;

    //Search the linked list for the first stream in order of data propagation;
    while ((next = element->next))
        element = next;

    //Return the last element;
    return next;

}


/*
 * llist_get_first : searches for the first element of the list;
 */

linked_element_t *llist_get_first(linked_element_t *element) {

    //Cache the previous element;
    linked_element_t *prev;

    //Search the linked list for the first element;
    while ((prev = element->prev))
        element = prev;

    //Return the first element;
    return element;

}


/*
 * llist_insert_after : inserts the whole list where new belongs between prev and its eventual successor
 */

void llist_insert_after(linked_element_t *src_prev, linked_element_t *new) {

    linked_element_t *new_first = llist_get_first(new);
    linked_element_t *new_last = llist_get_last(new);

    //Cache the current successor of src_prev;
    linked_element_t *src_next = src_prev->next;

    //Concatenate src_prev and new_first;
    llist_concat(src_prev, new_first);

    //If the src element already had a src_next element :
    if (src_next) {

        //Concatenate src_next and new_last;
        llist_concat(src_next, new_last);

    }

}


/*
 * llist_insert_before : inserts the whole list where new belongs between next and its eventual predecessor;
 */

void llist_insert_before(linked_element_t *src_next, linked_element_t *new) {

    linked_element_t *new_first = llist_get_first(new);
    linked_element_t *new_last = llist_get_last(new);

    //Cache the predecessor of src_next;
    linked_element_t *src_prev = src_next->prev;

    //Concatenate src_next and new_last;
    llist_concat(src_next, new_last);

    //If the src element already had a src_next element :
    if (src_prev) {

        //Concatenate src_prev and new_first;
        llist_concat(src_prev, new_first);

    }

}


/*
 * llist_direct_process : This function executes process for all elements after the provided one:
 */

void llist_direct_process(linked_element_t *element, void (*process)(linked_element_t *)) {

    //If process is a null pointer, nothing to do;
    if (!process)
        return;

    do {

        //Process the stream;
        (*process)(element);

        //Update the stream;
        element = element->next;

        //While there is stream to process in the linked list;
    } while (element);

}


/*
 * llist_direct_process_full : Searches for the first element, and executes process for all elements after;
 */

void llist_direct_process_full(linked_element_t *element, void (*process)(linked_element_t *)) {

    //Search for the first element;
    linked_element_t *first = llist_get_first(element);

    //Process from the first element;
    llist_direct_process(first, process);

}


/*
 * llist_reverse_process : This function executes process for all elements before the provided one:
 */

void llist_reverse_process(linked_element_t *element, void (*process)(linked_element_t *)) {

    //If process is a null pointer, nothing to do;
    if (!process)
        return;

    do {

        //Process the element;
        (*process)(element);

        //Update the stream;
        element = element->prev;

        //While there is stream to process in the linked list;
    } while (element);

}


/*
 * Searches for the last element, and executes process for all elements before;
 */

void llist_reverse_process_full(linked_element_t *element, void (*process)(linked_element_t *)) {

    //Get the last element;
    linked_element_t *last = llist_get_last(element);

    //Propagate data from this stream;
    llist_reverse_process(last, process);

}

