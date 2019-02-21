/*
  uni_env.h Part of nostd

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

#ifndef NOSTD_STDENV_H
#define NOSTD_STDENV_H

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#define uni_malloc(size) malloc(size)

#define uni_free(ptr) free(ptr)

#define uni_memcpy(dst, src, size) memcpy(dst, sec, size);


#endif /*NOSTD_STDENV_H*/
