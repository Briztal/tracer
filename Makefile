
#---------------------------------------------------- Initialisation ---------------------------------------------------

PROJECT_NAME := tracer

NAME := build/$(PROJECT_NAME)

#All sources will be built in ./build.
BUILDDIR := build

#initialise compilation and linking flags;
CFLAGS := -Wall -Os -g

LDFLAGS := -Wall -Wl,--gc-sections -Os

#TODO NO GENERAL COMPIL INCLUDE;

#include paths for libraries
COMPIL_INC := -Isrc
COMPIL_INC += -Isrc/data_structures


#----------------------------------------- Init before including core makefile -----------------------------------------

#Initialise the core include list to the core folder;
CORE_INC := -Isrc/core

#Initialise the core source files set to the only debug file;
CORE_SRCS += src/core/debug.c
CORE_SRCS += src/core/comm/swuart.c
CORE_SRCS += src/core/startup.c

#Initialise the driver source files set;
DRIVER_SRCS :=

#The memory map link script will be set by one of core makefiles;
LDSCRIPT_MMAP_DIR :=


#----------------------------------------------------- Core config -----------------------------------------------------

#include the teensy35 makefile that will initialise the toolchain environment and define basic rules;
include src/core/board/teensy35/Makefile


#------------------------------------------------ Post core link config ------------------------------------------------

#Now that the core makefile has updated link files, add appropriate options to the link flags;
LDFLAGS += -Tsrc/core/unified_link_script.ld -L$(LDSCRIPT_MMAP_DIR)


#---------------------------------------------------- kernel config ----------------------------------------------------

KERNEL_PUB_SRC := src/kernel/syscall.c

KERNEL_PRIV_SRC :=
KERNEL_PRIV_INC := -Isrc/kernel


include src/kernel/Makefile


#------------------------------------------------ Core library ------------------------------------------------

#Build the objects set from sources and reroute to build dir;
CORE_OBJS := $(foreach src, $(CORE_SRCS:.c=.o), $(BUILDDIR)/$(src))

#The core library depends on all core objects provided by the core board makefile and its subs;
core : $(CORE_OBJS)


#------------------------------------------------------ Make rules -----------------------------------------------------

# The base compilation rule : takes a path that targets the build directory, truncates the source part,
#	creates a directory to contain the object file and builds it from the associated source file;
#	If the associated source file does not exist, it will fail.

$(BUILDDIR)/%.o: %.c
	@echo "[CC]\t$@"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) $(COMPIL_INC) -o "$@" -c "$<"


# The elf rule. Depends on core lib.
elf: core
	@echo "[LD]\ttracer.elf"
	@$(CC) $(LDFLAGS) -o $(NAME).elf $(CORE_OBJS) $(LINK_LIBS)




#The general build target will simply build the elf file, but not the flashable format. No objcopy;
build: elf



clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(NAME).elf" "$(NAME).hex"

