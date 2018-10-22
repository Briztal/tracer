
SS = src/std
SB = build/stdl


#------------------------------------------- General stdlib compilation rule -------------------------------------------

stdlib :

	@$(_CC) -o $(SB)/list.o -c $(SS)/list.c
	@$(_CC) -o $(SB)/mem.o -c $(SS)/mem.c
	@$(_CC) -o $(SB)/stdout.o -c $(SS)/stdout.c
	@$(_CC) -o $(SB)/string.o -c $(SS)/string.c

STDL_RULES += stdlib



#------------------------------------------- Math compilation rule -------------------------------------------


#The math library can be either the stdlib math generic implementation, or can be overridden by the hardware make unit,
#in order to provide more efficient code.
#Implementation source file is in that case passed in the STD_MATH_OVERRIDDEN macro;

ifndef STD_MATH_DIR

STD_MATH_DIR = src/std/math/generic

endif

#Add the math folder to the general include path;
CFLAGS += -I$(STD_MATH_DIR)

std_math:
	@$(_CC) -o $(SB)/math.o -c $(STD_MATH_DIR)/math.c


STDL_RULES += std_math
