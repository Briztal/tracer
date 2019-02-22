/*
  bitwise.h Part of nostd

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

#ifndef NOSTD_BITWISE_H
#define NOSTD_BITWISE_H

/*
 * IS_SET : asserts if bits set in @mask are set in @var;
 */

#define IS_SET(var, mask) ((bool) ((var) & (mask)))


/**
 * IS_CLEAR : asserts if bits set in @mask are clear in @var, ie if bits set in @mask are set in ~@var;
 */

#define IS_CLEAR(var, mask) IS_SET(~(var), mask)


/**
 * ALIGNMENT_SIZE : determines the size associated with the provided magnitude, ie 2 ^ magnitude;
 *
 * A shift is used instead of an exponent;
 */

#define ALIGNMENT_SIZE(magnitude) (1 << (magnitude))


/**
 * ALIGNMENT_MASK : given an alignment magnitude, determines the associated alignment mask;
 *
 *  The mask is a word composed of @magnitude less significant bits set, and all remaining bits clear;
 *
 *      Ex : magnitude = 5, mask = b00011111;
 */

#define ALIGNMENT_MASK(magnitude) (ALIGNMENT_SIZE(magnitude) - 1)


/**
 * ALIGN_TO_INFERIOR : from an value and an alignment magnitude, determines the first number inferior or equal to
 *  @value that is properly aligned, ie multiple of 2 ^ @magnitude;
 *
 *  The trick is to determine the alignment mask, and to clear all required less significant bits;
 */

#define ALIGN_TO_INFERIOR(value, magnitude)  (((size_t) (value)) & (~(ALIGNMENT_MASK(magnitude))))


/**
 * ALIGN_TO_SUPERIOR : from an value and an alignment magnitude, determines the first number superior or equal to
 *  @value that is properly aligned, ie multiple of 2 ^ @magnitude;
 *
 *  The trick is to add the alignment mask and to determine the inferior aligned value of the result;
 */

#define ALIGN_TO_SUPERIOR(value, magnitude) ALIGN_TO_INFERIOR(((value) + ALIGNMENT_MASK(magnitude)), magnitude)





#endif /*NOSTD_BITWISE_H*/
