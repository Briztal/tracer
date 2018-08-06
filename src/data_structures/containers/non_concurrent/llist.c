/*
  llist.c - Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "llist.h"

#include <kernel/kernel.h>



/*
 * ------------------------------ Linked element ------------------------------
 */

/*
 * lelmt_concat : concatenates the couple of elements; If their predecessors / successors are allocated, error;
 */

void lelmt_concat(llist_t *const prev, llist_t *const next) {

    //First, if the first's output or the second's input is already allocated :
    if (prev->next || next->prev) {

        //Error. Association must not overwrite previous associations;
        kernel_error("SUUS");//TODO

    }

    //The next of the first stream must be the second;
    prev->next = next;

    //The previous of the second string must be the first;
    next->prev = prev;

}


/*
 * lelmt_get_first : searches for the first element of the list;
 */

llist_t *lelmt_get_first(llist_t *element) {

    //Cache the previous element;
    llist_t *prev;

    //Search the linked list for the first element;
    while ((prev = element->prev))
        element = prev;

    //Return the first element;
    return element;

}


/*
 * lelmt_get_last : searches for the last element in the list;
 */

llist_t *lelmt_get_last(llist_t *element) {

    //Cache the next element;
    llist_t *next;

    //Search the linked list for the first stream in order of data propagation;
    while ((next = element->next))
        element = next;

    //Return the last element;
    return next;

}


/*
 * lelmt_insert_after : inserts the whole list where new belongs between prev and its eventual successor
 */

void lelmt_insert_after(llist_t *const src_prev, llist_t *const new_element) {

    llist_t *new_first = lelmt_get_first(new_element);
    llist_t *new_last = lelmt_get_last(new_element);

    //Cache the current successor of src_prev;
    llist_t *src_next = src_prev->next;

    //Concatenate src_prev and new_first;
    lelmt_concat(src_prev, new_first);

    //If the src element already had a src_next element :
    if (src_next) {

        //Concatenate src_next and new_last;
        lelmt_concat(src_next, new_last);

    }

}

/*
 * lelmt_insert_before : inserts the whole list where new belongs between next and its eventual predecessor;
 */

void lelmt_insert_before(llist_t *const src_next, llist_t *const new_element) {

    llist_t *new_first = lelmt_get_first(new_element);
    llist_t *new_last = lelmt_get_last(new_element);

    //Cache the predecessor of src_next;
    llist_t *src_prev = src_next->prev;

    //Concatenate src_next and new_last;
    lelmt_concat(src_next, new_last);

    //If the src element already had a src_next element :
    if (src_prev) {

        //Concatenate src_prev and new_first;
        lelmt_concat(src_prev, new_first);

    }

}

/*
 * ------------------------------ Linked list ------------------------------
 */

/*
 * llist_insert_first : inserts the new element at the initial of the list;
 */

void llist_insert_first(linked_list_t *const list, llist_t *const element) {

    //If the element is null :
    if (!list || !element) {
        kernel_error("SUUS");//TODO
        return;
    }

    //If the maximum size is reached :
    if (list->nb_elements == list->max_nb_elements) {
        kernel_error("SUUS");//TODO
        return;
    }

    //Reset the element;
    element->next = element->prev = 0;

    //If the list is currently empty (increase the number of elements by the way);
    if (list->nb_elements == 0) {

        //Set the element as the first and the last;
        list->first = list->last = element;

    } else {

        //The first element exists. Cache it;
        llist_t *next = list->first;

        //Link both first and new element;
        next->prev = element;
        element->next = next;

        //Update the list's first element;
        list->first = element;

    }

    //Increase the number of elements;
    list->nb_elements++;

}

/*
 * llist_insert_last : inserts the new element at the end of the list;
 */

void llist_insert_last(linked_list_t *const list, llist_t *const element) {

    //If the element is null :
    if (!list || !element) {
        kernel_error("SUUS");//TODO
        return;
    }

    //If the maximum size is reached :
    if (list->nb_elements == list->max_nb_elements) {
        kernel_error("SUUS");//TODO
        return;
    }

    //Reset the element;
    element->next = element->prev = 0;

    //If the list is currently empty (increase the number of elements by the way);
    if (list->nb_elements == 0) {

        //Set the element as the first and the last;
        list->first = list->last = element;

    } else {

        //The last element exists, cache it;
        llist_t *prev = list->last;

        //Link both previous and new element;
        prev->next = element;
        element->prev = prev;

        //Update the list's last element;
        list->last = element;

    }

    //Increase the number of elements;
    list->nb_elements++;

}

//Insert the new element after an element of the list;
void llist_insert_after(linked_list_t *const list, llist_t *const src, llist_t *const new_element) {

    //If src is null, the list is supposedly empty;
    if (!src) {

        //If the list is not empty (update the number of elements by the way):
        if (list->nb_elements++) {

            //Error : null provided but list not empty;
            kernel_error("SUUS");//TODO

        }

        //Set the new_element as the first and the last;
        list->first = list->last = new_element;

        //Nothing else to do;
        return;

    }

    //src is not null; The case where src doesn't belong to the list is not caught;

    //If the successor is null, src is the last new_element of the list;
    if (!src->next) {

        //Update the list's last new_element;
        list->last = new_element;

    }

    //Increase the number of elements;
    list->nb_elements++;

    //insert the new_element after src;
    lelmt_insert_after(src, new_element);

}


/*
 * llist_insert_before : inserts the new element before an element of the list;
 */

void llist_insert_before(linked_list_t *const list, llist_t *const src, llist_t *const new_element) {

    if (!list || !new_element ) {

        kernel_error("llist.c : llist_insert_before : null list or new_element provided;");

        return;
    }

    //If src is null, the list is supposedly empty;
    if (!src) {

        //If the list is not empty (update the number of elements by the way):
        if (list->nb_elements++) {

            //Error : null provided but list not empty;
            kernel_error("SUUS");//TODO

        }

        //Set the new_element as the first and the last;
        list->first = list->last = new_element;

        //

        //Nothing else to do;
        return;

    }

    //src is not null; The case where src doesn't belong to the list is not caught;

    //If the successor is null, src is the first new_element of the list;
    if (!src->prev) {

        //Update the list's first new_element;
        list->first = new_element;

    }

    //Increase the number of elements;
    list->nb_elements++;

    //Insert the new_element before src;
    lelmt_insert_before(src, new_element);

}



/*
 * llist_remove_first : removes the first element of the list;
 */

llist_t *llist_remove_first(linked_list_t *const list) {

    //If the lst is null :
    if (!list) {
        kernel_error("SUUS");//TODO
        return 0;
    }

    //If the list is empty :
    if (!list->nb_elements) {

        //error;
        kernel_error("SUUS");//TODO

        return 0;

    } else {

        //Cache the first element;
        llist_t *elmt = list->first;

        //Cache the next element;
        llist_t *next = elmt->next;

        //Reset the first's neighbors;
        elmt->next = elmt->prev = 0;

        //If the next element exists :
        if (next) {

            //Reset its predecessor;
            next->prev = 0;

        } else {

            //If list comprises only one element :
            list->last = 0;

        }

        //Update the first element of the list;
        list->first = next;

        //Update the number of elements;
        list->nb_elements--;

        //Return the previous first element;
        return elmt;

    }

}

//Remove the last element of the list;
llist_t *llist_remove_last(linked_list_t *const list) {

    //If the lst is null :
    if (!list) {
        kernel_error("SUUS");//TODO
        return 0;
    }

    //If the list is empty :
    if (!list->nb_elements) {

        //error;
        kernel_error("SUUS");//TODO

        return 0;

    } else {

        //Cache the last element;
        llist_t *elmt = list->last;

        //Cache the previous element;
        llist_t *prev = elmt->prev;

        //Reset the last's neighbors;
        elmt->next = elmt->prev = 0;

        //If the previous element exists :
        if (prev) {

            //Reset its successor;
            prev->next = 0;

        } else {

            //If the list only comprised one element :
            list->first = 0;
        }

        //Update the last element of the list;
        list->last = prev;

        //Update the number of elements;
        list->nb_elements--;

        //Return the previous last element;
        return elmt;

    }

}


/*
 * llist_remove_element : un-links an element of the list;
 */

void llist_remove_element(linked_list_t *const list, llist_t *const element) {

    //If the element is null, error;
    if (!list || !element) {
        kernel_error("SUUS");//TODO
        return;
    }

    //Cache the element's successor;
    llist_t *next = element->next;

    //If the successor is null, the element is the end of the list;
    if (!next) {

        //Remove the last element of the list;
        llist_remove_last(list);

    } else {

        //Cache the element's predecessor;
        llist_t *prev = element->prev;


        //If the predecessor is null, the element is the list's first;
        if (!prev) {

            //Remove the first element of the list;
            llist_remove_first(list);

        } else {

            //Link the predecessor and the successor;
            prev->next = next;
            next->prev = prev;

            //Unlink the element;
            element->prev = element->next = 0;

        }

    }

}



/*
 * ------------------------------ Linked list ------------------------------
 */

/*
 * lring_insert_after : insert an element after the current element of the ring;
 */

void lring_insert_after(linked_ring_t *const ring, llist_t *const element) {

    //If the maximum size is reached :
    if (ring->nb_elements == ring->max_nb_elements)
        kernel_error("SUUS");//TODO

    //If the ring has no elements (increase it by the way);
    if (ring->nb_elements++ == 0) {

        //Set the current element as the new one;
        ring->current = element;

        //Concatenate the new element with himself;
        lelmt_concat(element, element);

    } else {

        //Cache the current second element;
        llist_t *first = ring->current, *second = first->next;

        //Link the element between the first and the second;
        first->next = second->prev = element;
        element->prev = first;
        element->next = second;

    }

}


void lring_insert_before(linked_ring_t *const ring, llist_t *const element) {

    //If the maximum size is reached :
    if (ring->nb_elements == ring->max_nb_elements)
        kernel_error("SUUS");//TODO

    //If the ring has no elements (increase it by the way);
    if (ring->nb_elements++ == 0) {

        //Set the current element as the new one;
        ring->current = element;

        //Concatenate the new element with himself;
        lelmt_concat(element, element);

    } else {

        //Cache first and second element;
        llist_t *second = ring->current, *first = second->prev;

        //Link the element between the first and the second;
        first->next = second->prev = element;
        element->prev = first;
        element->next = second;

    }

}


//Focus on the next element;
void lring_incr(linked_ring_t *const ring) {

    //If the ring is empty, nothing to do;
    if (!ring->nb_elements) {
        return;
    }

    //Set the current element as the current's next one;
    ring->current = ring->current->next;

}

//Focus on the previous element;
void lring_decr(linked_ring_t *const ring) {

    //If the ring is empty, nothing to do;
    if (!ring->nb_elements) {
        return;
    }

    //Set the current element as the current's next one;
    ring->current = ring->current->prev;

}


/*
 * llist_direct_process : This function executes process for all elements after the provided one:
 */

void llist_direct_process(const llist_t *element, void (*const process)(const llist_t *)) {

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

void llist_direct_process_full(llist_t *const element, void (*const process)(const llist_t *)) {

    //Search for the first element;
    llist_t *first = lelmt_get_first(element);

    //Process from the first element;
    llist_direct_process(first, process);

}


/*
 * llist_reverse_process : This function executes process for all elements before the provided one:
 */

void llist_reverse_process(llist_t *element, void (*const process)(const llist_t *)) {

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

void llist_reverse_process_full(llist_t *const element, void (*const process)(const llist_t *)) {

    //Get the last element;
    llist_t *last = lelmt_get_last(element);

    //Propagate data from this stream;
    llist_reverse_process(last, process);

}

