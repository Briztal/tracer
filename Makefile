
#---------------------------------------------------- Initialisation ---------------------------------------------------

#All sources will be built in ./build.
BUILDDIR = build

#Project name;
PROJECT_NAME := tracer

#Elf - hex names;
NAME := $(BUILDDIR)/$(PROJECT_NAME)


#Initialise compilation and linking flags;
CFLAGS := -Wall -Os -g
LDFLAGS := -Wall -Wl,--gc-sections -Os

KERNEL_DEP :=


#--------------------------------------------------- Utilities Module --------------------------------------------------

#The util include path; Available to all sources;
UTIL_INC :=

#Include the core makefile, to build the core library appropriately;
include src/util/Makefile


#--------------------------------------------------- Core Module ---------------------------------------------------

#The hardware module selects the memory map link script;
LDSCRIPT_MMAP_DIR :=

#Include the hard makefile, to build the hard library appropriately;
include src/hard/Makefile

#Now that the core makefile has updated link files, add appropriate options to the link flags;
LDFLAGS += -Tsrc/hard/unified_link_script.ld -L$(LDSCRIPT_MMAP_DIR)


#---------------------------------------------------- Kernel Module ----------------------------------------------------


#The kernel include path; The core lib is provided;
KERNEL_INC := -Isrc/

#The kernel lib public sources, available to processes and modules;
KERNEL_PUB_SRCS :=

#The kernel lib internal sources, only available to kernel sources;
KERNEL_SRCS :=


#Include the kernel makefile, to build the kernel library appropriately;
include src/kernel/Makefile


#Build the objects set from sources and reroute to build dir;
KERNEL_OBJS := $(foreach src, $(KERNEL_SRCS:.c=.o), $(BUILDDIR)/$(src))
KERNEL_PUB_OBJS := $(foreach src, $(KERNEL_PUB_SRCS:.c=.o), $(BUILDDIR)/$(src))


#The kernel compilation rule;
$(BUILDDIR)/kernel/%.o: src/kernel/%.c
	@echo "[CC]\t$@"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) $(KERNEL_INC) -o $@ -c $<


kernel : $(KERNEL_OBJS) $(KERNEL_DEP)



#------------------------------------------------------ Make rules -----------------------------------------------------

# The base compilation rule : takes a path that targets the build directory, truncates the source part,
#	creates a directory to contain the object file and builds it from the associated source file;
#	If the associated source file does not exist, it will fail.

#(BUILDDIR)/%.o: %.c
#	@echo "[CC]\t$@"
#	@mkdir -p "$(dir $@)"
#	@$(CC) $(CFLAGS) $(COMPIL_INC) -o "$@" -c "$<"


# The elf rule. Depends on core lib.
elf: core kernel util
	@echo "[LD]\ttracer.elf"
	@$(CC) $(LDFLAGS)  $(HARD_OBJS)  $(KERNEL_OBJS) -Wl,--whole-archive $(MODULES) -Wl,--no-whole-archive $(UTIL_OBJS) -o $(NAME).elf
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!
	#TODO THIS IS NOT LINKED PROPERLY !!



clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(NAME).elf" "$(NAME).hex"

