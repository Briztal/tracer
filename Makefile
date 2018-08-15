
#---------------------------------------------------- Initialisation ---------------------------------------------------

PROJECT_NAME := tracer

NAME := build/$(PROJECT_NAME)

#All sources will be built in ./build.
BUILDDIR := build

#include paths for libraries
COMPIL_INC := -Isrc
COMPIL_INC += -Isrc/data_structures
COMPIL_INC += -Isrc/hardware
COMPIL_INC += -Isrc/kernel

#initialise compilation and linking flags;
CFLAGS := -Wall -Os -g

LDFLAGS := -Wl,--gc-sections -Os

#--------------------------------------------------- Pre core config ---------------------------------------------------

#The memory map link script will be set by one of core makefiles;
LDSCRIPT_MMAP_DIR :=

#Initialise hardware source files to contain only startup code;
CORE_SOURCES :=


#----------------------------------------------------- Core config -----------------------------------------------------

#include the teensy35 makefile that will initialise the toolchain environment and define basic rules;
include src/hardware/board/teensy35/Makefile


#------------------------------------------------ Post core link config ------------------------------------------------

#Now that the core makefile has updated link files, add appropriate options to the link flags;
LDFLAGS += -Tsrc/hardware/unified_link_script.ld -L$(LDSCRIPT_MMAP_DIR)


#------------------------------------------------ Core library ------------------------------------------------


#Build the objects set from sources and reroute to build dir;
CORE_OBJS := $(foreach src, $(CORE_SOURCES:.c=.o), $(BUILDDIR)/$(src))

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


hex: elf
	@echo "[HEX]\ttracer.hex"
	@$(SIZE) $(NAME).elf
	@$(OBJCOPY) -O ihex -R .eeprom $(NAME).elf $(NAME).hex


#The general build target will simply build the elf file, but not the flashable format. No objcopy;
build: elf



clean:
	@echo Cleaning...
	@rm -rf "$(BUILDDIR)"
	@rm -f "$(NAME).elf" "$(NAME).hex"

