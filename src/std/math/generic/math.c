//
// Created by root on 10/17/18.
//

#include "math/vf32.h"

//--------------------------------------------------- Basic operations -------------------------------------------------


#define VECT_APPLY(v0, v1, r, op) while(size--) {*((r)++) = *((v0)++) op *((v1)++);}

//r[] = v0[] + v1[];
void vf32_add(const f32_t *v0, const f32_t *v1, f32_t *r, size_t size) {
	VECT_APPLY(v0, v1, r, +);
}


//r[] = v0[] - v1[];
void vf32_sub(const f32_t *v0, const f32_t *v1, f32_t *r, size_t size) {
	VECT_APPLY(v0, v1, r, -);
}


//r[] = v0[] * v1[];
void vf32_mul(const f32_t *v0, const f32_t *v1, f32_t *r, size_t size) {
	VECT_APPLY(v0, v1, r, *);
}


//v2[] = v0[] / v1[];
void vf32_div(const f32_t *v0, const f32_t *v1, f32_t *r, size_t size) {
	VECT_APPLY(v0, v1, r, /);
}

#undef VECT_APPLY

//-------------------------------------------------- Chained operations ------------------------------------------------

#define VECT_MAC(v0, v1, v2, r, op0, op1) while(size--) {*((r)++) = op0 (*((v0)++) op1 *((v1)++) * *((v2)++));}

//r[] = v0[] + v1[] * v2[];
void f32_mac(const f32_t *v0, const f32_t *v1, const f32_t *v2, f32_t *r, size_t size) {
	VECT_MAC(v0, v1, v2, r, +, +);
}

//r[] = v0[] - v1[] * v2[];
void f32_msub(const f32_t *v0, const f32_t *v1, const f32_t *v2, f32_t *r, size_t size) {
	VECT_MAC(v0, v1, v2, r, +, -);
}

//r[] = - v0[] + v1[] * v2[]);
void f32_nmac(const f32_t *v0, const f32_t *v1, const f32_t *v2, f32_t *r, size_t size) {
	VECT_MAC(v0, v1, v2, r, -, +);
}

//r[] = -v0[] - v1[] * v2[]);
void f32_nmsub(const f32_t *v0, const f32_t *v1, const f32_t *v2, f32_t *r, size_t size) {
	VECT_MAC(v0, v1, v2, r, -, -);
}


//--------------------------------------------------- Functions -------------------------------------------------

//r[] = sqrt(v0[]);
void f32_sqrt(const f32_t *v0, f32_t *r, size_t size);


//--------------------------------------------------- Type conversions -------------------------------------------------

//f[] = (u32_t) u[];
void to_u32(const f32_t *f, u32_t *u, size_t size) {
	
	while(size--) {
		
		*((u)++) = (uint32_t) *(f++);
		
	}
}

//u[] = (f32_t) f[];
void to_f32(const u32_t *u, f32_t *f, size_t size) {
	
	while(size--) {
		
		*((f)++) = (uint32_t) *(u++);
		
	}
	
}

