/*
  frac.h Part of nostd

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

#ifndef NOSTD_FRAC_H
#define NOSTD_FRAC_H

#include <stdint.h>

/*--------------------------------------------------- types --------------------------------------------------*/

/**
 * Fractional numbers are integers, that have a fractional part and an integer part.
 *
 *  The size of the fractional part is known by the user and not contained in the type.
 *
 *  The value of the fractional number can only be determined along with the size of its integer part;
 */

/*Fractional number of 8 bits;*/
typedef uint8_t frac8_t;

/*Fractional number of 16 bits;*/
typedef uint16_t frac16_t;

/*Fractional number of 32 bits;*/
typedef uint32_t frac32_t;

/*Fractional number of 64 bits;*/
typedef uint64_t frac64_t;


/*--------------------------------------------------- Conversions --------------------------------------------------*/

#define FRAC_CONVERT_NOCHECK(receiver, value, frac_size) (receiver) = (typeof(receiver)) ((value) * (1 << (frac_size)))

#define ITYPE_MAX(receiver) (1 << ((sizeof(receiver) << 3) - 1))

#define FRAC_CONVERT_CHECK(receiver, value, frac_size) {\
    if (((value) < 0) && (ITYPE_MAX(receiver) <= (-(value)))) {\
        (receiver) = (typeof(receiver)) (- (ITYPE_MAX(receiver) - 1));\
    } else if (((value) > 0) && (ITYPE_MAX(receiver) <= (value))) {\
        (receiver) = (typeof(receiver)) (ITYPE_MAX(receiver) - 1);\
    } else {\
        (receiver) = (typeof(receiver)) ((value) * (1 << (frac_size)));\
    }\
}



#define FRAC_SIGN_MASK(var) ((typeof(var)) (1 << ((sizeof(var) << 3) - 1)))

#define FRAC_SIGN_BIT(var) ((var) & (FRAC_SIGN_MASK(var)))

#define FRAC_ABSOLUTE(var) ((var) | ~(FRAC_SIGN_MASK(var)))

#define FRAC_ADJUST(dst, dst_frac_size, src, src_frac_size) {\
    \
    /*If dst occupies a greater (or equal) space than src : */\
    if (sizeof(src) >= sizeof(dst)) {\
        \
        /*The cast to typeof(src) to typeof(dst) is safe;*/\
        const typeof(dst) src_casted = (typeof(dst))(src);\
        \
        /*If src's fractional size is greater than dst's :*/\
        if ((src_frac_size) >= (dst_frac_size)) {\
            \
            /*The shift can be done safely without overflow; The sign bit must be removed and placed again;*/\
            (dst) = FRAC_SIGN_BIT(var) | (FRAC_ABSOLUTE(src_casted) >> ((src_frac_size) - (dst_frac_size))); \
            /*TODO CHECK ABOUT SIGNED INTEGER ARITHMETICS, IF UNSIGNED VALUES WOULDN'T BE PREFERABLE;*/\
            /*TODO TEST CASTS FROM UNSIGNED TO SIGNED AND SIGNED TO UNSIGNED TO SEE IF THERE ARE ALTERATIONS;*/\
            /**/\
            \
        }\
        \
    }\
}



/*--------------------------------------------------- Conversions --------------------------------------------------*/


#endif /*NOSTD_FRAC_H*/
