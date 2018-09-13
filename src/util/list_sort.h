//
// Created by root on 9/12/18.
//

#include "type.h"


/*
 * Prerequisites : 
 * 
 * 	#define MERGE_NAME list_merge
 * 
 * 	#define SORT_T struct tlist
 * 
 * 	#define SORT_CMP_T uint32_t
 * 
 * 	#define VAL(x) ((x)->val)
 */



/**
 * merge : takes two sorted non-null lists A and B, and returns the merged sorted list, L;
 *
 * @param A
 * @param B
 * @return
 */

static SORT_T *MERGE_NAME(SORT_T *A, SORT_T *B) {

	//Define two flags, that become set when a list has been fully inserted;
	bool A_terminated = false, B_terminated = false;

	//Cache A and B's first elements;
	SORT_T *const Afirst = A, *const Bfirst = B;

	//Cache A and B's last elements;
	SORT_T *const Alast = Afirst->head.prev, *const Blast = Bfirst->head.prev;

	//L is the last element of the merged list; new elements must be concatenated to it;
	SORT_T *L0;
	SORT_T *L = 0;

	//Cache the segment of A that we will insert;
	SORT_T *Lmin;
	SORT_T *Lmax;

	
	//If A_0 < B_0, A must be inserted before B;
	if (VAL(A) < VAL(B)) {
		kernel_log("l0 A %d", VAL(A));
		L0 = A;
		goto insert_A;
	} else {
		kernel_log("l0 B %d", VAL(B));
		L0 = B;
		goto insert_B;
	}


	//L_INSERT : concatenates [Lmin:Lmax] with L;
	#define L_INSERT if (L) {L->head.next = Lmin, Lmin->head.prev = L;}; L = Lmax;


	//--------------------------------------------------- A insertion --------------------------------------------------

	/*
	 * A must be inserted in L until we encounter an element whose value is greater that B_0's;
	 */
	insert_A :
	
	Lmin = A;

	//If B is terminated :
	if (B_terminated) {

		//A must be inserted until the end;
		Lmax = Alast;

	} else {

		//If B is not terminated, we must insert a segment of A;
		//We must determine the greatest element of A with inferior to B's current;

		//Cache B's value;
		SORT_CMP_T bv = VAL(B);

		//Traverse A;
		while (1) {

			//Cache A's successor;
			SORT_T *const An = A->head.next;

			//If A is terminated :
			if (An == Afirst) {

				//Mark A terminated;
				A_terminated = true;

				//Stop traversing A;
				break;

			}

			//If A's successor is greater than B's limit :
			if (VAL(An) >= bv) {

				//We can traversing A;
				break;

			}

			//If A's successor is still lower than B, keep traversing;
			A = An;

		}

		//A now points on the greatest element inferior to B;
		Lmax = A;
	}


	L_INSERT;

	//Now that A has been inserted, B can be inserted;
	//If B has been fully inserted, we can complete;
	if (B_terminated)
		goto end;

	//If not, insert a segment of B;
	goto insert_B;


	//--------------------------------------------------- B insertion --------------------------------------------------

	/*
	 * B must be inserted in L until we encounter an element whose value is greater that A_0's;
	 */
	insert_B :

	Lmin = B;

	//If A is terminated :
	if (A_terminated) {

		//B must be inserted until the end;
		Lmax = Blast;

	} else {

		//If A is not terminated, we must insert a segment of B;
		//We must determine the greatest element of B with inferior to A's current;

		//Cache A's value;
		SORT_CMP_T bv = VAL(A);

		//Traverse B;
		while (1) {

			//Cache B's successor;
			SORT_T *const Bn = B->head.next;

			//If B is terminated :
			if (Bn == Bfirst) {

				//Mark B terminated;
				B_terminated = true;

				//Stop traversing B;
				break;

			}

			//If B's successor is greater than A's limit :
			if (VAL(Bn) >= bv) {

				//We can traversing B;
				break;

			}

			//If B's successor is still lower than A, keep traversing;
			B = Bn;

		}

		//B now points on the greatest element inferior to A;
		Lmax = B;
	}

	L_INSERT;

	//Now that B has been inserted, A can be inserted;
	//If A has been fully inserted, we can complete;
	if (A_terminated)
		goto end;

	//If not, insert a segment of A;
	goto insert_A;


	end :

	kernel_log("LL0 %d", VAL(L0));

	//Close L;
	L->head.next = L0, L0->head.prev = L;

	kernel_log("LL0 %d", VAL(L0));

	//Return the first element of L;
	return L0;
	
}