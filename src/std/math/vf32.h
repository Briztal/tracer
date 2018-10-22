//
// Created by root on 10/17/18.
//

#ifndef TRACER_FLOAT_H
#define TRACER_FLOAT_H

#include "type.h"

#include <math_types.h>


/*
 * Hardware vector operations are made on blocks of a certain length, and sizes of arrays used in vector operations
 * must be multiple of this length;
 *
 * The length is provided by the hardware library, implementing functions;
 */

#define vf32_size(target_size) (((target_size) + (VF32_BASE_LENGTH - 1)) % VF32_BASE_LENGTH)


//--------------------------------------------------- Basic operations -------------------------------------------------

//r[] = v0[] + v1[];
void vf32_add(const f32_t *v0, const f32_t *v1, f32_t *r, size_t size);

//r[] = v0[] - v1[];
void vf32_sub(const f32_t *v0, const f32_t *v1, f32_t *r, size_t size);

//r[] = v0[] * v1[];
void vf32_mul(const f32_t *v0, const f32_t *v1, f32_t *r, size_t size);

//v2[] = v0[] / v1[];
void vf32_div(const f32_t *v0, const f32_t *v1, f32_t *r, size_t size);


//-------------------------------------------------- Chained operations ------------------------------------------------

//r[] = v0[] + v1[] * v1[];
void f32_mac(const f32_t *v0, const f32_t *v1, const f32_t *v2, f32_t *r, size_t size);

//r[] = v0[] - v1[] * v1[];
void f32_msub(const f32_t *v0, const f32_t *v1, const f32_t *v2, f32_t *r, size_t size);

//r[] = -(v0[] + v1[] * v1[]);
void f32_nmac(const f32_t *v0, const f32_t *v1, const f32_t *v2, f32_t *r, size_t size);

//r[] = -(v0[] - v1[] * v1[]);
void f32_nmsub(const f32_t *v0, const f32_t *v1, const f32_t *v2, f32_t *r, size_t size);


//--------------------------------------------------- Functions -------------------------------------------------

//r[] = sqrt(v0[]);
void f32_sqrt(const f32_t *v0, f32_t *r, size_t size);


//--------------------------------------------------- Type conversions -------------------------------------------------

//f[] = (u32_t) u[];
void to_u32(const f32_t *f, u32_t *u, size_t size);

//u[] = (f32_t) f[];
void to_f32(const u32_t *u, f32_t *f, size_t size);



#endif //TRACER_FLOAT_H
