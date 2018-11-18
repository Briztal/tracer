#
#	This makefile contains the base for building tracer;
#
#
#	Tracer's component are the following :
#		- kernel : the standard operationnal base, in charge of managing shared ressources;
#			These resources can be hardware (processor, coprocessors, peripheral, memory, etc...) or software
#			(files, shared mem, interfaces, etc...);
#		- modules : the variable operationnal base, executing at kernel privilege level, and taking advantage of
#			the kernel's capabilities. Modules can be hardware dependant code, (peripheral drivers), code that must
#			use abstracted hardware capabilities (servo controller).
#		- apps : processes, executing at low privilege, without access to kernel capabilities.
#		- stdlib : a set of standard functions, that can be used by all components;
#
#
#	Each component is built using :
#		- A build folder, unique for each component, where its elf objects must be generated;
#		- A set of dependency rules, that make units can populate. Those rules will compile code and store resulting
#			objects in the component's build folder;
#
#
#	Components are built by successively including different make units. Each one  :
#		- defines make variables, to configurate other make units;
#		- implements compilation rules, each one being destinated to one component.
#			Resulting objects should be generated in its build dir;
#		- add each rule to the appropriate component's list;
#
#
#	Four make units are included. In the order :
#
# 		- hard : contains standard components hardware dependent code. Its aim is to provide the most complete standard
#			if for the whole projects. Non standard components (applications and projects mainly) are free to
#			include hardware specific code.
#			It configures the kernel, adds kernel rules to generate the kernel's hardware dependent parts, adds
#			device driver modules, and generate hardware code for the standard lib (ex for math).
#
#		- kernel : builds the kernel's hardware independent part; Only has effect on the kernel component;
#
#		- std : contains the standard lib's hardware independent part; Only has an effect on stdlib component;
#
#		- project : generates application code, by creating and adding rules for the app and module component;
#


#---------------------------------------------------- Naming ---------------------------------------------------

#Project name;
PROJECT_NAME := tracer

#Elf - hex names;
NAME := build/$(PROJECT_NAME)

#------------------------------------------------------ Toolchain ------------------------------------------------------

#Several programs are available during the executable generation. The hardware make unit must define them;

#The preprocessor, compiler, assembler, and linker;
CC =

#The archiver;
AR :=

#The loader;
OBJCOPY :=

#The analyser;
SIZE :=

#---------------------------------------------------- Initialisation ---------------------------------------------------

#Initialise compilation and linking flags;
CFLAGS := -Wall -Os -g -std=c99 -ffreestanding
LDFLAGS := -Wall -Wl,--gc-sections -Os -std=c99 -nostdlib


#FLags proper to the kernel;
KFLAGS :=


INC := -Isrc
INC += -Isrc/dmz
INC += -Isrc/std

#Include path only contains src and std dir;
#INC += -Isrc/std

KINC := -Isrc
KINC += -Isrc/dmz
KINC += -Isrc/kernel
KINC += -Isrc/kernel/mem


#The hardware module selects the memory map link script;
LDSCRIPT_MMAP_DIR :=


#Rules lists reference all rules that a component depends on;
KRNL_RULES :=
MODS_RULES :=
APPS_RULES :=
STDL_RULES :=


#Components objects must be generated in their build directories;
KRNL_D := build/krnl
MODS_D := build/mods
MODC_D := build/modc
APPS_D := build/apps
APPC_D := build/appc
STDL_D := build/stdl


#------------------------------------------------ Primary configuration ------------------------------------------------

#Include the kernel configuration file;
include src/config/kernel.mk


#------------------------------------------------------ Make units -----------------------------------------------------

#Include the board makefile, the entry point of hardware make tree;
include src/config/board.mk

#Include the kernel makefile
include src/kernel/Makefile

#Include the dmz makefile
include src/dmz/Makefile

#include the project makefile
include src/config/project.mk

#include the standard lib makefile
include src/std/Makefile

#----------------------------------------------------- Finalisation ----------------------------------------------------

#Kernel sources are compiled from multiple make units. This abreviation eases their work;
KCC = $(CC) $(CFLAGS) $(KFLAGS) $(KINC)

#Abreviation for compiling with standard C flags and include paths;
_CC = $(CC) $(CFLAGS) $(INC)

#Abreviation proper to each make unit;
__CC =

#Now that the hardware lib has updated link files, add appropriate options to the link flags;
LDFLAGS += -Tsrc/hard/unified_link_script.ld -L$(LDSCRIPT_MMAP_DIR)


#-------------------------------------------- Objects list (late expanding) --------------------------------------------

#Components objects are generated in their build directories;
KRNL_O = $(wildcard $(KRNL_D)/*.o)
STDL_O = $(wildcard $(STDL_D)/*.o)

#Each module must be packed and mangled
MODS_PACKERS = $(addsuffix _mod_pack,$(MODS_RULES))
MODS_O = $(wildcard $(MODC_D)/*.o)


#Each application must be packed and mangled
APPS_PACKERS = $(addsuffix _app_pack,$(APPS_RULES))
APPS_O = $(wildcard $(APPC_D)/*.o)


#--------------------------------------------------- Build arbo init ---------------------------------------------------

#Create a directory for each libs
dtree:
	@mkdir build
	@mkdir $(KRNL_D)
	@mkdir $(MODS_D)
	@mkdir $(MODC_D)
	@mkdir $(STDL_D)
	@mkdir $(APPS_D)
	@mkdir $(APPC_D)


#-------------------------------------------------- Components rules ---------------------------------------------------

#ELF translate : prefix,input,output
elf_translate = ./elf_mangle -p $(1) -i $(2) -o $(3)

#ELF mangle : translation_file,object_file
elf_mangle = $(OBJCOPY) --redefine-syms=$(1) $(2)


#components all depend on the content of their rules list;
krnl_pack : $(KRNL_RULES)
	@$(LD) -r -o build/krnl.o $(KRNL_O)
	@$(call elf_translate,kernel,build/krnl.o,build/krnl_trsl)
	@$(call elf_mangle,build/krnl_trsl,build/krnl.o)


%_mod_pack : %
	@$(LD) -r -o $(MODC_D)/$<.o $(addprefix $(MODS_D)/,$($<_f))

	@$(call elf_translate,$<,$(MODC_D)/$<.o,$(MODC_D)/$<_trsl)
	@$(call elf_mangle,$(MODC_D)/$<_trsl,$(MODC_D)/$<.o)
	@$(call elf_mangle,build/krnl_trsl,$(MODC_D)/$<.o)



#Each module comprises multiple files that must be packed in one single file;
mods_pack : $(MODS_PACKERS)


#The standard lib is different. We must archive all objetcs;
stdl_pack : $(STDL_RULES)

	@$(AR) cr build/stdlib.a $(STDL_O)
	@$(call elf_translate,stdl,build/stdlib.a,build/stdl_trsl)
	@$(call elf_mangle,build/stdl_trsl,build/stdlib.a)



%_app_pack : %
	@$(LD) -r -o $(APPC_D)/$<.o $(addprefix $(APPS_D)/,$($<_f))

	@$(call elf_translate,$<,$(APPC_D)/$<.o,$(APPC_D)/$<_trsl)
	@$(call elf_mangle,$(APPC_D)/$<_trsl,$(APPC_D)/$<.o)
	@$(call elf_mangle,build/stdl_trsl,$(APPC_D)/$<.o)



apps_pack : $(APPS_PACKERS)




#----------------------------------------------- Linking rule ----------------------------------------------

elf: dtree krnl_pack mods_pack stdl_pack apps_pack
	@echo "[LD]\ttracer.elf"

	$(CC) $(LDFLAGS) build/krnl.o $(MODS_O) $(APPS_O) build/stdlib.a -o $(NAME).elf

	@$(SIZE) -A -t -x $(NAME).elf


#----------------------------------------------------- Clean rule ------------------------------------------------------

clean:
	@echo Cleaning...
	@rm -rf "build"

