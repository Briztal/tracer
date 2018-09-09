/*
  incr_call.h Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

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

//Concatenates two parameters;
#define CAT2(a, b) a##b

//INCR_CALL_N(x) : generates a call for x for all integers below N
#define INCR_CALL_0(x)
#define INCR_CALL_1(x) x(0)
#define INCR_CALL_2(x) x(0) x(1)
#define INCR_CALL_3(x) x(0) x(1) x(2)
#define INCR_CALL_4(x) x(0) x(1) x(2) x(3)
#define INCR_CALL_5(x) x(0) x(1) x(2) x(3) x(4)
#define INCR_CALL_6(x) x(0) x(1) x(2) x(3) x(4) x(5)
#define INCR_CALL_7(x) x(0) x(1) x(2) x(3) x(4) x(5) x(6)
#define INCR_CALL_8(x) x(0) x(1) x(2) x(3) x(4) x(5) x(6) x(7)
#define INCR_CALL_9(x) x(0) x(1) x(2) x(3) x(4) x(5) x(6) x(7) x(8)
#define INCR_CALL_10(x) x(0) x(1) x(2) x(3) x(4) x(5) x(6) x(7) x(8) x(9)

//INCR_CALL(n, x) : generates a call for x for all integers below n
#define INCR_CALL(i, x) CAT2(INCR_CALL_,i)(x)
