/*
  list.h Part of nostd

  Copyright (c) 2018 Raphaël Outhier

  nostd is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  nostd is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with nostd.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef NOSTD_LIST_H
#define NOSTD_LIST_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

/*---------------------------------------------------- Simple lists ----------------------------------------------------*/

/*A list head contains two pointers to list heads;*/
struct list_head {
	
	/*Pointers to previous and next elements;*/
	void *prev, *next;
	
};


/*
 * Simple lists are structs whose first field (offset 0) is a list head. They can be casted into a list head;
 */

/*Link the couple of list heads;*/
#define __list_link(p, n) {(p)->next = (n), (n)->prev = (p);}

/*Link the head to itself;*/
#define list_init(h) {(h)->next = (h)->prev = h;}


/*TODO DOCUMENT PROPERLY WHAT FUNCTION DO;*/
/*TODO DOCUMENT PROPERLY WHAT FUNCTION DO;*/
/*TODO DOCUMENT PROPERLY WHAT FUNCTION DO;*/
/*TODO DOCUMENT PROPERLY WHAT FUNCTION DO;*/
/*TODO DOCUMENT PROPERLY WHAT FUNCTION DO;*/
/*TODO DOCUMENT PROPERLY WHAT FUNCTION DO;*/
/*TODO DOCUMENT PROPERLY WHAT FUNCTION DO;*/
/*TODO DOCUMENT PROPERLY WHAT FUNCTION DO;*/
/*TODO DOCUMENT PROPERLY WHAT FUNCTION DO;*/
/*TODO DOCUMENT PROPERLY WHAT FUNCTION DO;*/
/*TODO DOCUMENT PROPERLY WHAT FUNCTION DO;*/
/*TODO DOCUMENT PROPERLY WHAT FUNCTION DO;*/

/**
 * list_add : from h0::t0 and h1::t1, forms h0::t0::h1::t1;
 *
 * links h0 and h1, and t1
 */

static __always_inline void list_concat(struct list_head *h0, struct list_head *h1) {
	
	/*Cache ends of src and dst;*/
	struct list_head *t0 = h0->prev;
	struct list_head *t1 = h1->prev;
	
	/*Link the end of dst to the head of src;*/
	__list_link(t1, h0);
	
	/*Link the end of src to the head of dst;*/
	__list_link(t0, h1);
	
}


/**
 * list_remove : Remove the list head from the rest of the list; head will be linked to itself;
 */
static __always_inline void list_remove(struct list_head *head) {
	
	/*First, cache the list neighbors;*/
	struct list_head *prev = head->prev, *next = head->next;
	
	/*Link prev and next;*/
	__list_link(prev, next);
	
	/*Link l with itself;*/
	__list_link(head, head);
	
}


/**
 * list_concat_ref : concatenates @h0 and the list head referenced by h1.
 * 	If h1 is null, h1 will now reference h0;
 *
 * @param h0 : the list head to concat on the left;
 * @param h1 : the reference to the list head to concat on the right;
 */

void list_concat_ref(struct list_head *h0, struct list_head **h1);


/**
 * list_remove_ref : when we must remove an element from a list with a particular element referenced, these functions
 * 	can be used;
 *
 * 	It updates @ref to 0 when @head is the only element of the list, or to @head's successor or predecessor;
 */

void list_remove_ref_next(struct list_head *head, struct list_head **ref);

void list_remove_ref_prev(struct list_head *head, struct list_head **ref);


/*--------------------------------------------------- Extended lists ---------------------------------------------------*/


/*
 * Extended lists are structs whose first field (offset 0) is not a list head. They can't be casted into a list head;
 *
 * One of their fields is an list_head (extended list head);
 *
 * To operate properly, operation functions also take the offset of this field in the types;
 */



/**
 * __elist_prev/__elist_next : gets the previous/next element of an extended list;
 *
 * @param l : the extended list to get the prev/next element;
 * @param off : the list head's offset in l;
 *
 */

#define __elist_prev(l, off) (((struct list_head *)((void *)((uint8_t *)(l) + (off))))->prev)
#define __elist_next(l, off) (((struct list_head *)((void *)((uint8_t *)(l) + (off))))->next)

/*Link the couple of list heads;*/
#define __elist_link(p, n, off) {\
	__elist_prev(n, off) = (p);\
	__elist_next(p, off) = (n);\
	}



/**
 * list_add : concatenates the end of s0 and the head of l1
 */
static __always_inline void __elist_concat(void *h0, void *h1, size_t head_offset) {
	
	/*Cache ends of src and dst;*/
	void *t0 = __elist_prev(h0, head_offset);
	void *t1 = __elist_prev(h1, head_offset);
	
	/*Link the end of dst to the head of src;*/
	__elist_link(t1, h0, head_offset);
	
	/*Link the end of src to the head of dst;*/
	__elist_link(t0, h1, head_offset);
	
}

#define elist_concat(struct_name, field_name, h0, h1)\
	__elist_concat((h0), (h1), offsetof(struct_name, field_name));


/**
 * list_remove : Remove the list head from the rest of the list; head will be linked to itself;
 */

static __always_inline void __elist_remove(void *l, size_t head_offset) {
	
	/*First, cache the list neighbors;*/
	void *prev = __elist_prev(l, head_offset), *next = __elist_next(l, head_offset);
	
	/*Link prev and next;*/
	__elist_link(prev, next, head_offset);
	
	/*Link l with itself;*/
	__elist_link(l, l, head_offset);
	
	
}

#define elist_remove(struct_name, field_name, instance_name)\
	__elist_remove((instance_name), offsetof(struct_name, field_name));


/**
 * list_concat_ref : concatenates @h0 and the list head referenced by h1.
 * 	If h1 is null, h1 will now reference h0;
 *
 * @param h0 : the list head to concat on the left;
 * @param h1 : the reference to the list head to concat on the right;
 */

void __elist_concat_ref(void *h0, void **h1, size_t head_offset);

#define elist_concat_ref(struct_name, field_name, elmt, ref)\
	__elist_concat((elmt), (ref), offsetof(struct_name, field_name));

/**
 * list_remove_ref : when we must remove an element from a list with a particular element referenced, these functions
 * 	can be used;
 *
 * 	It updates @ref to 0 when @head is the only element of the list, or to @head's successor or predecessor;
 */

void __elist_remove_ref_next(void *elmt, void **ref, size_t head_offset);

void __elist_remove_ref_prev(void *elmt, void **ref, size_t head_offset);


#define elist_remove_ref_next(struct_name, field_name, elmt, ref)\
	__elist_remove_ref_next((void *)(elmt), (void **) (ref), offsetof(struct_name, field_name));


#define elist_remove_ref_prev(struct_name, field_name, elmt, ref)\
	__elist_remove_ref_prev((elmt), (ref), offsetof(struct_name, field_name));

#endif /*NOSTD_LIST_H*/
