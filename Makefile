
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


#----------------------------------------------------- Core Module -----------------------------------------------------

#The core include path;
CORE_INC :=

#The core lib sources, with system drivers and nonstandard drivers;
CORE_SRCS :=

#The standard drivers lib sources;
DRIVER_SRCS :=

#The link script include directories;;
LDSCRIPT_MMAP_DIR :=


#Include the core makefile, to build the core library appropriately;
include src/core/Makefile


#Now that the core makefile has updated link files, add appropriate options to the link flags;
LDFLAGS += -Tsrc/core/unified_link_script.ld -L$(LDSCRIPT_MMAP_DIR)

#Build the objects set from sources and reroute to build dir;
CORE_OBJS := $(foreach src, $(CORE_SRCS:.c=.o), $(BUILDDIR)/$(src))


#The general core object build rule : depends on the related source file, located in the src directory;
#	It changes the directory to the build dir;

$(BUILDDIR)/core/%.o: src/core/%.c
	@echo "[CC]\t$@"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) $(CORE_INC) -o $@ -c $<


#The core library depends on all core objects mentionned by the core makefile and its subs;
core : $(CORE_OBJS)


#---------------------------------------------------- Kernel Module ----------------------------------------------------

#The kernel include path; The core lib is provided;
KERNEL_INC := $(CORE_INC)

#The kernel lib public sources, available to processes and modules;
KERNEL_PUB_SRCS :=

#The kernel lib internal sources, only available to kernel sources;
KERNEL_SRCS :=


#Include the kernel makefile, to build the kernel library appropriately;
include src/kernel/Makefile


#Build the objects set from sources and reroute to build dir;
KERNEL_OBJS := $(foreach src, $(KERNEL_SRCS:.c=.o), $(BUILDDIR)/$(src))
KERNEL_PUB_OBJS := $(foreach src, $(KERNEL_PUB_SRCS:.c=.o), $(BUILDDIR)/$(src))


#The general kernel object build rule : depends on the related source file, located in the src directory;
#	It changes the directory to the build dir;

$(BUILDDIR)/kernel/%.o: src/kernel/%.c
	@echo "[CC]\t$@"
	@mkdir -p "$(dir $@)"
	@$(CC) $(CFLAGS) $(KERNEL_INC) -o $@ -c $<


#The kernel library depends on all kernel objects mentionned by the kernel makefile and its subs;
kernel : $(KERNEL_OBJS)


#------------------------------------------------------ Make rules -----------------------------------------------------

# The base compilation rule : takes a path that targets the build directory, truncates the source part,
#	creates a directory to contain the object file and builds it from the associated source file;
#	If the associated source file does not exist, it will fail.

#(BUILDDIR)/%.o: %.c
#	@echo "[CC]\t$@"
#	@mkdir -p "$(dir $@)"
#	@$(CC) $(CFLAGS) $(COMPIL_INC) -o "$@" -c "$<"


# The elf rule. Depends on core lib.
elf: core kernel
	@echo "[LD]\ttracer.elf"
	@$(CC) $(LDFLAGS) -o $(NAME).elf $(CORE_OBJS) $(KERNEL_OBJS)
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

