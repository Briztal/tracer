//
// Created by root on 8/6/18.
//

#ifndef TRACER_LIST_H
#define TRACER_LIST_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>
#include <core/core.h>

//A list head contains two pointers to list heads;
struct list_head {

	//Pointers to previous and next elements;
	void *prev, *next;

};

//---------------------------------------------------- Simple lists ----------------------------------------------------

/*
 * Simple lists are structs whose first field (offset 0) is a list head. They can be casted into a list head;
 */

//Link the couple of list heads;
#define __list_link(p, n) {(p)->next = (n), (n)->prev = (p);}

//Link the head to itself;
#define list_init(h) {(h)->next = (h)->prev = h;}



//Validity check for list add; Library internal;
void __list_add_valid(struct list_head *new, struct list_head *prev, struct list_head *next);


//Add the new element between previous and next. Library internal, doesn't make check;
static inline void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next) {

__list_link(prev, new);
__list_link(new, next);

}


/**
 * list_empty : asserts if the list's tail is empty;
 * @head : the list's head;
 */

static inline bool list_empty(struct list_head *head) {

	//Compare if head's successor is head.
	return head->next == head;

}


/**
 * list_add : from h0::h00::t0 and h1::t1, forms h0::h1::t1::h00::t0;
 *
 * links h0 and h1, and t1
 */
static inline void list_add(struct list_head *h0, struct list_head *h1) {

	//Cache ends of src and dst;
	struct list_head *t0 = h0->next;
	struct list_head *t1 = h1->prev;

	//Link the end of dst to the head of src;
	__list_link(h0, h1);

	//Link the end of src to the head of dst;
	__list_link(t1, t0);

}



/**
 * list_add : from h0::t0 and h1::t1, forms h0::t0::h1::t1;
 *
 * links h0 and h1, and t1
 */

static inline void list_concat(struct list_head *h0, struct list_head *h1) {

	//Cache ends of src and dst;
	struct list_head *t0 = h0->prev;
	struct list_head *t1 = h1->prev;

	//Link the end of dst to the head of src;
	__list_link(t1, h0);

	//Link the end of src to the head of dst;
	__list_link(t0, h1);

}


/**
 * list_remove : Remove the list head from the rest of the list; head will be linked to itself;
 */
static inline void list_remove(struct list_head *l) {

	//First, cache the list neighbors;
	struct list_head *prev = l->prev, *next = l->next;

	//Link prev and next;
	__list_link(prev, next);

	//Link l with itself;
	__list_link(l, l);

}


//--------------------------------------------------- Extended lists ---------------------------------------------------


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

#define __elist_prev(l, off) (((struct list_head *)((void *)(l) + (off)))->prev)
#define __elist_next(l, off) (((struct list_head *)((void *)(l) + (off)))->next)

//Link the couple of list heads;
#define __elist_link(p, n, off) {\
	__elist_prev(n, off) = (p);\
	__elist_next(p, off) = (n);\
	}



/**
 * list_add : concatenates the end of s0 and the head of l1
 */
static inline void __elist_add(void *src, void *dst, size_t head_offset) {

	//Cache ends of src and dst;
	void *src_e = __elist_prev(src, head_offset);
	void *dst_e = __elist_prev(dst, head_offset);

	//Link the end of dst to the head of src;
	__elist_link(dst_e, src, head_offset);

	//Link the end of src to the head of dst;
	__elist_link(src_e, dst, head_offset);

}

#define elist_add(struct_name, field_name, src, dst)\
	__elist_add((src), (dst), offsetof(struct_name, field_name));


/**
 * list_remove : Remove the list head from the rest of the list; head will be linked to itself;
 */

static inline void __elist_remove(void *l, size_t head_offset) {

	//First, cache the list neighbors;
	void *prev = __elist_prev(l, head_offset), *next = __elist_next(l, head_offset);

	//Link prev and next;
	__elist_link(prev, next, head_offset);

	//Link l with itself;
	__elist_link(l, l, head_offset);


}

#define elist_remove(struct_name, field_name, instance_name)\
	__elist_remove((instance_name), offsetof(struct_name, field_name));

#endif //TRACER_LIST_H
