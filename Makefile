
#---------------------------------------------------- Initialisation ---------------------------------------------------

#All sources will be built in ./build.
BUILDDIR = build

#Project name;
PROJECT_NAME := tracer

#Elf - hex names;
NAME := $(BUILDDIR)/$(PROJECT_NAME)

#Board name; Must match a folder in src/hard/board;
BOARD := teensy35

#---------------------------------------------------- Primary config ---------------------------------------------------

#Initialise compilation and linking flags;
CFLAGS := -Wall -Os -g -std=c99 -ffreestanding
LDFLAGS := -Wall -Wl,--gc-sections -Os -std=c99 -nostdlib

#The default include path set. Comprises just src.
INC = -Isrc


build_dirs:

#Create a directory for the chip library;
	@mkdir build
	@mkdir build/hard
	@mkdir build/mod
	@mkdir build/kernel


#--------------------------------------------------- Utilities lib --------------------------------------------------

#The util sources list;
UTIL_SRCS :=

#The util makefile will add all util sources to UTIL_SRCS;
include src/util/Makefile

#Build the objects set from sources and reroute to build dir;
UTIL_OBJS := $(foreach src, $(UTIL_SRCS:.c=.o), $(BUILDDIR)/$(src))

#The core library depends on all core objects mentionned by the core makefile and its subs;
util : $(UTIL_OBJS)


#--------------------------------------------------- Hard lib ---------------------------------------------------

#The hardware module selects the memory map link script;
LDSCRIPT_MMAP_DIR :=

#The modules list references all rules that the module lib depends on;
MODULES :=

#The modules objects references all objects files that the module lib comprises;
MODULES_OBJS :=

#Include the platform makefile;
include src/hard/board/$(BOARD)/Makefile

#Now that the hardware lib has updated link files, add appropriate options to the link flags;
LDFLAGS += -Tsrc/hard/unified_link_script.ld -L$(LDSCRIPT_MMAP_DIR)


#Build the objects set from sources and reroute to build dir;
#HARD_OBJS := $(foreach src, $(HARD_SRCS:.c=.o), $(BUILDDIR)/$(src))

#The hardware lib, depends on all selected hardware sources;
#hard : $(HARD_OBJS)

#---------------------------------------------------- Kernel lib ----------------------------------------------------

#The kernel lib public sources, available to processes and modules;
KERNEL_PUB_SRCS :=

#The kernel lib internal sources, only available to kernel sources;
KERNEL_SRCS :=

#Include the kernel makefile, to build the kernel library appropriately;
include src/kernel/Makefile

#Build the objects set from sources and reroute to build dir;
KERNEL_OBJS := $(foreach src, $(KERNEL_SRCS:.c=.o), $(BUILDDIR)/$(src))
KERNEL_PUB_OBJS := $(foreach src, $(KERNEL_PUB_SRCS:.c=.o), $(BUILDDIR)/$(src))


kernel : $(KERNEL_OBJS)

#---------------------------------------------------- Custom modules ----------------------------------------------------

MODULES_SRCS :=

#Include the modules makefile, to embed all required modules;
include src/modules/Makefile


#Build the objects set from sources and reroute to build dir;
#MODULES_OBJS += $(foreach src, $(MODULES_SRCS:.c=.o), $(BUILDDIR)/$(src))

#The modules lib depends on all modules rules and all modules files;
modules : $(MODULES)


#------------------------------------------------------ Make rules -----------------------------------------------------

#The general compilation rule. Compiles a source file in src/, saves the output in the corresponding path in build/;
#Requires the compiler (CC) to be up to date, and all C flags to be appropriately set;
$(BUILDDIR)/%.o: src/%.c

#Display the name of the source we compile;
	@echo "[CC]\t$@"

#Create a dir if necessary;TODO NOPE, TAKES WAY TOO MUCH TIME !
	@mkdir -p "$(dir $@)"

#Compile, providing only src in include path, and all C flags;
	@$(CC) $(CFLAGS) $(INC) -o $@ -c $<

HARD_OBJS = $(wildcard build/hard/*.o)

MODULES_OBJS = $(wildcard build/mod/*.o)


elf: build_dirs hard modules kernel util
	@echo "[LD]\ttracer.elf"
	@$(CC) $(LDFLAGS)  $(HARD_OBJS)  $(KERNEL_OBJS) $(MODULES_OBJS) $(UTIL_OBJS) -o $(NAME).elf



clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(NAME).elf" "$(NAME).hex"

