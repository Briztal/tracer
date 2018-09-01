
#---------------------------------------------------- Initialisation ---------------------------------------------------

#--------------------------- General Init ---------------------------

#All sources will be built in ./build.
BUILDDIR = build

#Project name;
PROJECT_NAME := tracer

#Elf - hex names;
NAME := $(BUILDDIR)/$(PROJECT_NAME)


#Initialise compilation and linking flags;
CFLAGS := -Wall -Os -g
LDFLAGS := -Wall -Wl,--gc-sections -Os


#--------------------------------------------------- Utilities Module --------------------------------------------------

#The util include path;
UTIL_INC :=

#The util sources;
UTIL_SRCS :=


#Include the core makefile, to build the core library appropriately;
include src/util/Makefile

#Build the objects set from sources and reroute to build dir;
UTIL_OBJS := $(foreach src, $(UTIL_SRCS:.c=.o), $(BUILDDIR)/$(src))


#The general core object build rule : depends on the related source file, located in the src directory;
#	It changes the directory to the build dir;

$(BUILDDIR)/util/%.o: src/util/%.c
	@echo "[CC]\t$@"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) -o $@ -c $<


#The core library depends on all core objects mentionned by the core makefile and its subs;
util : $(UTIL_OBJS)


#----------------------------------------------------- Core Module -----------------------------------------------------

#The util include path;
CORE_INC :=

#The util sources;
CORE_SRCS :=

#Include the core makefile;
include src/core/Makefile

#Build the objects set from sources and reroute to build dir;
CORE_OBJS := $(foreach src, $(CORE_SRCS:.c=.o), $(BUILDDIR)/$(src))

#The general core compilation rule;
$(BUILDDIR)/core/%.o: src/core/%.c
	@echo "[CC]\t$@"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) $(CORE_INC) -o $@ -c $<


#The core library depends on sys lib, and all core objects mentionned by the core makefile and its subs;
core : sys $(CORE_OBJS)


#---------------------------------------------------- Kernel Module ----------------------------------------------------

#The kernel include path; The core lib is provided;
KERNEL_INC :=

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
	@$(CC) $(CFLAGS) $(KERNEL_INC) $(CORE_INC) -o $@ -c $<


#The kernel library depends on :
# - the core library;
# - the std drivers library;
# - all kernel source files;
kernel : core std $(KERNEL_OBJS)



#--------------------------------------------------- Hardware Module ---------------------------------------------------

#The hardware module will form two lists.

#The sys list, comprising system drivers, used by the core module;
SYS_SRCS :=

#The std list, comprising standard drivers, used by the kernel module;
STD_LIST :=

#The hardware include list.
HARD_INC :=


#The hardware module will also select the memory map link script;
LDSCRIPT_MMAP_DIR :=


#Include the hard makefile, to build the hard library appropriately;
include src/hard/Makefile


#Now that the core makefile has updated link files, add appropriate options to the link flags;
LDFLAGS += -Tsrc/core/unified_link_script.ld -L$(LDSCRIPT_MMAP_DIR)


#Build both objects set from sources and reroute to build dir;
SYS_OBJS := $(foreach src, $(SYS_SRCS:.c=.o), $(BUILDDIR)/$(src))
STD_OBJS := $(foreach src, $(STD_SRCS:.c=.o), $(BUILDDIR)/$(src))


#The general hard compilation rule;
$(BUILDDIR)/hard/%.o: src/hard/%.c
	@echo "[CC]\t$@"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) $(HARD_INC) $(CORE_INC) -o $@ -c $<


#Both libs depend on their respective object sets;
sys : $(SYS_OBJS)
std : $(STD_OBJS)


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
	@$(CC) $(LDFLAGS) -o $(NAME).elf $(SYS_OBJS) $(STD_OBJS) $(CORE_OBJS) $(KERNEL_OBJS) $(UTIL_OBJS)
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

