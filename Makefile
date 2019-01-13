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
#	Five make units are included. In the order :
#
# 		- hard : contains standard components hardware dependent code. Its aim is to provide the most complete standard
#			if for the whole projects. Non standard components (applications and projects mainly) are free to
#			include hardware specific code.
#			It configures the kernel, adds kernel rules to generate the kernel's hardware dependent parts, adds
#			device driver modules, and generate hardware code for the standard lib (ex for math).
#
#		- kernel : builds the kernel's hardware independent part; Only has effect on the kernel component;
#
#		- dmz : contains utilities that are used by the kernel and by the stdlib. All source files in this make unit
#			are compiled twice, once for the kernel, and once for the stdlib;
#
#		- std : contains the standard lib's hardware independent part; Only has an effect on stdlib component;
#
#		- project : generates application code, by creating and adding rules for the app and module component;
#
#
# 	Linking barriers :
#
#		As the executable is entirely static, all parts (kernel, modules, stdlib and apps), are linked together to form the
#		final executable. This raise a security problem; Indeed, modules and kernel run on a high privilleged level, and
#		stdlib and applications run on a lower privilege level. Their compilation and linking should be separated;
#
#		Moreover, modules and (resp) apps use public symbols of kernel and (resp) stdlib, but the opposite should never
#		happen. By no means the kernel or the stdlib should use a function of a module or an application.
#
#		There need to be a mechanism to restrict the linking process to only compatible parts of the code, but standard
#		toolchains do not provide such mechanism.
#
#		To achieve this, we will mangle (prefix) defined symbols of each object that would form the executable, and propagate
#		changes only to object that are authorised to be linked with it.
#
#		For example, we will mangle all public defined symbols of the kernel, and propagate this mangling to modules only.
#		That way, if the stdlib, or an application used a kernel symbol, during the final linking, it will query a symbol
#		that will have been mangled in the kernel object file but not in its object file. An undefined reference will exist.
#
#		This mechanism is only a safeguard, and doesn't fully prevent forbidden linking, as pre-using a mangled name
#		is sufficient to overpass it.
#
#		With such mechanisms, kernel and stdlib are stand_alone, and modules (resp applications) can only use their
#		symbols, and those of the kernel (resp stdlib).
#		The only exception to this rule concerns two functions of the standard library, run_exec and run_exit, that are
#		used by the kernel at a process's entry and exit points;
#
#
#	In opposition to kernel and stdlib that are unique, modules and applications are many, and their amound is not
#		defined. To register properly a module, or an application, create a rule that should build your objects,
#		and a variable that should contain your object files names;  Both rule and variable must have the same name;
#		The name itself is not important, as soon as it doesn't conflict with another rule / variable;
#
#		Finally, to include your module in the make process, add the rule / variable name in the appropriate rules
#		list. This list can be MODS_RULES or APPS_RULES;
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

#The linker, used to merge relocatable objects. Only used to produce relocatable code;
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

KRNL_MGL_PREFIX = kernel
STDL_MGL_PREFIX = stdlib

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

#Each module must be packed and mangled. For each module, the packing rule must be called;
MODS_PACKERS = $(addsuffix _mod_pack,$(MODS_RULES))

#Packed and mangles modules objects are stored in the MODC dir.
MODS_O = $(wildcard $(MODC_D)/*.o)

#Each application must be packed and mangled. For each app, the packing rule must be called;
APPS_PACKERS = $(addsuffix _app_pack,$(APPS_RULES))

#Packed and mangles app objects are stored in the APPC dir.
APPS_O = $(wildcard $(APPC_D)/*.o)


#--------------------------------------------------- Build arbo init ---------------------------------------------------

#Create a directory for each libs
dtree:

#The build dir, root of the build tree;
	@mkdir -p build

#The kernel dir, where kernel objects are stored;
	@mkdir -p $(KRNL_D)

#The modules objects dir, where pre-packing module objects are stored;
	@mkdir -p $(MODS_D)

#The packed modules objects dir, where packed and manged module objects are stored;
	@mkdir -p $(MODC_D)

#The stdlib dir, where stdlib objects are stored;
	@mkdir -p $(STDL_D)

#The apps objects dir, where pre-packing app objects are stored;
	@mkdir -p $(APPS_D)

#The packed apps objects dir, where packed and manged app objects are stored;
	@mkdir -p $(APPC_D)


#---------------------------------------------------- Link barriers ----------------------------------------------------

# As the executable is entirely static, all parts (kernel, modules, stdlib and apps), are linked together to form the
# 	final executable. This raise a security problem; Indeed, modules and kernel run on a high privilleged level, and
#	stdlib and applications run on a lower privilege level. Their compilation and linking should be separated;
#
#	Moreover, modules and (resp) apps use public symbols of kernel and (resp) stdlib, but the opposite should never
#	happen. By no means the kernel or the stdlib should use a function of a module or an application.
#
#	There need to be a mechanism to restrict the linking process to only compatible parts of the code, but standard
#	toolchains do not provide such mechanism.
#
#	To achieve this, we will mangle (prefix) defined symbols of each object that would form the executable, and propagate
#	changes only to object that are authorised to be linked with it.
#
#	For example, we will mangle all public defined symbols of the kernel, and propagate this mangling to modules only.
#	That way, if the stdlib, or an application used a kernel symbol, during the final linking, it will query a symbol
#	that will have been mangled in the kernel object file but not in its object file. An undefined reference will exist.
#
#	This mechanism is only a safeguard, and doesn't fully prevent forbidden linking, as pre-using a mangled name
#	is sufficient to overpass it.


#ELF translate : prefix,input,output
elf_translate = ./elf_mangle -p $(1) -i $(2) -o $(3)

#ELF mangle : translation_file,object_file
elf_mangle = $(OBJCOPY) --redefine-syms=$(1) $(2)


#The kernel must be build, packed, and mangled. Its transliation table must be used for all modules;
krnl_pack : $(KRNL_RULES)

#Pack the kernel in one single object file;
	@$(LD) -r -o build/krnl.o $(KRNL_O)

#Generate the kernel translation table;
	@$(call elf_translate,$(KRNL_MGL_PREFIX),build/krnl.o,build/krnl_trsl)

#Add custom rules to allow interaction of kernel and stdlib;
	@echo "run_exec $(STDL_MGL_PREFIX)_run_exec" >> build/krnl_trsl
	@echo "run_exit $(STDL_MGL_PREFIX)_run_exit" >> build/krnl_trsl

#Mangle the kernel symbols in the kernel file;
	@$(call elf_mangle,build/krnl_trsl,build/krnl.o)



#Each module must be packed, mangled independently, using its own translation table and the kernel translation table;
%_mod_pack : %

#Pack the module in a single object file;
	@$(LD) -r -o $(MODC_D)/$<.o $(addprefix $(MODS_D)/,$($<))

#Generate the module's translation table;
	@$(call elf_translate,$<,$(MODC_D)/$<.o,$(MODC_D)/$<_trsl)

#Mangle module's symbols in the module file;
	@$(call elf_mangle,$(MODC_D)/$<_trsl,$(MODC_D)/$<.o)

#Mangle the kernel's symbols in the module file;
	@$(call elf_mangle,build/krnl_trsl,$(MODC_D)/$<.o)



#The standard lib is different. We must archive all objetcs, generate the translation table, and mangle std symbols;
stdl_pack : $(STDL_RULES)

#Archive the stdandard library;
	@$(AR) cr build/stdlib.a $(STDL_O)

#Generte the stdlib translation table;
	@$(call elf_translate,$(STDL_MGL_PREFIX),build/stdlib.a,build/stdl_trsl)

#Mangle stdlib's symbols in the stdlib archive;
	@$(call elf_mangle,build/stdl_trsl,build/stdlib.a)



#Each app must be packed, mangled independently, using its own translation table and the stdlib translation table;
%_app_pack : %

#Pack the app in a single object file;
	@$(LD) -r -o $(APPC_D)/$<.o $(addprefix $(APPS_D)/,$($<))

#Generate the app's translation table;
	@$(call elf_translate,$<,$(APPC_D)/$<.o,$(APPC_D)/$<_trsl)

#Mangle app's symbols in the app file;
	@$(call elf_mangle,$(APPC_D)/$<_trsl,$(APPC_D)/$<.o)

#Mangle the stdlib's symbols in the app file;
	@$(call elf_mangle,build/stdl_trsl,$(APPC_D)/$<.o)



#----------------------------------------------- Linking rule ----------------------------------------------

#To build the elf file, we need the directory tree, and packed versions of kernel, modules, stdlib and applications;
elf: dtree krnl_pack $(MODS_PACKERS) stdl_pack $(APPS_PACKERS)

#Link all objects to form the executable;
	@$(CC) $(LDFLAGS) build/krnl.o $(MODS_O) $(APPS_O) build/stdlib.a -o $(NAME).elf

#Display sections informations;
	@$(SIZE) -A -t -x $(NAME).elf


#----------------------------------------------------- Clean rule ------------------------------------------------------

#All build files are stored in the build dir; Removing it is sufficient;
clean:
	@rm -rf "build"

