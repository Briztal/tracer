#
# This file contains general build options, that may affect the behaviour of several build units;
#

# The architecture executables should be built for;
#	The variable must contain the name of a board makefile (probabily located in arch/board/);

BOPS_ARCHITECTURE := arch/board/teensy35.mk


# The type of executable that should be generated :
#
#   - 0 : standalone : the executable is the only piece of code present.
#       -> kernel executable resides in ROM;
#       -> linker script concerns ROM and RAM;
#       -> the executable must contain architecture startup code;
#
#   -  1 : (ROM-RAM) app : the kernel resides is executed by another program;
#       -> kernel executable is made to be executed from ROM or RAM;
#       -> linker script concerns ROM or RAM;
#       -> the executable is not supposed to contain architecture startup code;

BOPS_EXECUTABLE_TYPE := 0


# The build execution mode :
#
#   - 0 : static : no code can be dynamically modified, and executed;
#		this mode is relevant when the RAM cannot be used to contain executable code, or when
#		efficiency is primary;
#		In this mode, running applications and dynamic modules is not possible, static modules are
#		pre-linked to the kernel;
#
#
#   -  1 : dynamic : code can be modified and executed dynamically;
#		this mode is relevant when code can be executed in RAM, and when execution efficiency is not a
#		major objective;
#		This mode supports dynamic modules and applications, memory protection for these executables;
#		In this mode, static modules are embedded in the executable as elf files, that the kernel loads
#		at run-time;
#

BOPS_EXECUTION_MODE := 0


