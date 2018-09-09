//
// Created by root on 9/9/18.
//


#define CAT(a, b) a##b


#define MACRO_INDEX_0(a, ...) a
#define MACRO_INDEX_1(a, b, ...) b
#define MACRO_INDEX_2(a, b, c, ...) c
#define MACRO_INDEX_3(a, b, c, d, ...) d
#define MACRO_INDEX_4(a, b, c, d, e, ...) e
#define MACRO_INDEX_5(a, b, c, d, e, f, ...) f
#define MACRO_INDEX_6(a, b, c, d, e, f, g, ...) g
#define MACRO_INDEX_7(a, b, c, d, e, f, g, h, ...) h
#define MACRO_INDEX_8(a, b, c, d, e, f, g, h, i, ...) i
#define MACRO_INDEX_9(a, b, c, d, e, f, g, h, i, j, ...) j

#define MAKE_ARG(i) _MAKE_ARG(i, MAKE_ARGS)

#define _MAKE_ARG(i, ...) CAT(MACRO_INDEX_, i)(__VA_ARGS__)


#define MULTI_MACRO_0(x)
#define MULTI_MACRO_1(x) x(0)
#define MULTI_MACRO_2(x) x(0) x(1)
#define MULTI_MACRO_3(x) x(0) x(1) x(2)
#define MULTI_MACRO_4(x) x(0) x(1) x(2) x(3)
#define MULTI_MACRO_5(x) x(0) x(1) x(2) x(3) x(4)
#define MULTI_MACRO_6(x) x(0) x(1) x(2) x(3) x(4) x(5)
#define MULTI_MACRO_7(x) x(0) x(1) x(2) x(3) x(4) x(5) x(6)
#define MULTI_MACRO_8(x) x(0) x(1) x(2) x(3) x(4) x(5) x(6) x(7)
#define MULTI_MACRO_9(x) x(0) x(1) x(2) x(3) x(4) x(5) x(6) x(7) x(8)
#define MULTI_MACRO_10(x) x(0) x(1) x(2) x(3) x(4) x(5) x(6) x(7) x(8) x(9)

#define MULTI_MACRO(i, x) CAT(MULTI_MACRO_,i)(x)
