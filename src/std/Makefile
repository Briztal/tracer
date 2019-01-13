
STD_F = src/std
STD_B = build/stdl

std_base :
	@$(_CC) -o $(STD_B)/syscall.o 	-c $(STD_F)/syscall.c
	@$(_CC) -o $(STD_B)/run.o 	-c $(STD_F)/run.c


STDL_RULES += std_base

#------------------------------------------- Math compilation rule -------------------------------------------


#The math library can be either the stdlib math generic implementation, or can be overridden by the hardware make unit,
#in order to provide more efficient code.
#Implementation source file is in that case passed in the STD_MATH_OVERRIDDEN macro;

ifndef STD_MATH_DIR

STD_MATH_DIR = src/std/_generic

endif

#Add the math folder to the general include path;
CFLAGS += -I$(STD_MATH_DIR)

std_math:
	@$(_CC) -o $(SB)/math.o -c $(STD_MATH_DIR)/math.c


STDL_RULES += std_math
