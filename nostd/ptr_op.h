/*
  ptr.h Part of nostd

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

#ifndef NOSTD_PTR_OP_H
#define NOSTD_PTR_OP_H

#define ptr_sum_byte_offset(p, offset) ((void *) (((uint8_t *) (p)) + ((size_t) (offset))))

#define ptr_get_byte_offset(low, high) ((size_t) (((uint8_t *) (high)) - ((uint8_t *) (low))))


#define ptr_sum_array_offset(type, p, offset) ((type *) (((type *) (p)) + ((size_t) (offset))))

#define ptr_get_array_offset(type, low, high) ((size_t) (((type *) (high)) - ((type *) (low))))

#endif /*NOSTD_PTR_OP_H*/
