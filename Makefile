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
#			interface for the whole projects. Non standard components (applications and projects mainly) are free to
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


#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES
#TODO ADD TO COMPONENTS RULES


#---------------------------------------------------- Naming ---------------------------------------------------

#Project name;
PROJECT_NAME := tracer

#Elf - hex names;
NAME := build/$(PROJECT_NAME)


#---------------------------------------------------- Initialisation ---------------------------------------------------

#Initialise compilation and linking flags;
CFLAGS := -Wall -Os -g -std=c99 -ffreestanding
LDFLAGS := -Wall -Wl,--gc-sections -Os -std=c99 -nostdlib

#Include path only contains src and std dir;
INC = -Isrc
INC = -Isrc/std


#The hardware module selects the memory map link script;
_LDSCRIPT_MMAP_DIR :=


#Rules lists reference all rules that a component depends on;
KRNL_RULES :=
MODS_RULES :=
APPS_RULES :=
STDL_RULES :=


#Components objects must be generated in their build directories;
KRNL_D := build/krnl
MODS_D := build/mods
APPS_D := build/apps
STDL_D := build/stdl


#------------------------------------------------ Primary configuration ------------------------------------------------

#Include the kernel configuration file;
include src/config/kernel.mk


#------------------------------------------------------ Make units -----------------------------------------------------

#Include the board makefile, the entry point of hardware make tree;
include src/config/board.mk

#Include the kernel makefile
include src/kernel/Makefile

#include the project makefile
include src/config/project.mk

#include the standard lib makefile
include src/std/Makefile


#----------------------------------------------------- Finalisation ----------------------------------------------------

#Now that the hardware lib has updated link files, add appropriate options to the link flags;
LDFLAGS += -Tsrc/hard/unified_link_script.ld -L$(_LDSCRIPT_MMAP_DIRmake)


#-------------------------------------------- Objects list (late expanding) --------------------------------------------

#Components objects are generated in their build directories;
KRNL_O = $(wildcard $(KRNL_D)/*.o) 
MODS_O = $(wildcard $(MODS_D)/*.o)
APPS_O = $(wildcard $(APPS_D)/*.o)
STDL_O = $(wildcard $(STDL_D)/*.o)


#--------------------------------------------------- Build arbo init ---------------------------------------------------

#Create a directory for each libs
build_dirs:
	@mkdir build
	@mkdir $(KRNL_D)
	@mkdir $(MODS_D)
	@mkdir $(APPS_D)
	@mkdir $(STDL_D)


#-------------------------------------------------- Components rules ---------------------------------------------------

#components all depend on the content of their rules list;
krnl : $(KRNL_RULES)
mods : $(MODS_RULES)
apps : $(APPS_RULES)
stdl : $(STDL_RULES)
#TODO STDL BECOMES AN ARCHIVE, CODE WILL BE LIGHTER;


#----------------------------------------------- Linking rule ----------------------------------------------

elf: build_dirs krnl mods stdl apps
	@echo "[LD]\ttracer.elf"

#TODO STDL BECOMES AN ARCHIVE, CODE WILL BE LIGHTER;

	@$(CC) $(LDFLAGS)  $(KRNL_O) $(MOD_O) $(APP_O)  $(STDL_O)-o $(NAME).elf
	@$(SIZE) -A -t -x $(NAME).elf


#----------------------------------------------------- Clean rule ------------------------------------------------------

clean:
	@echo Cleaning...
	@rm -rf "build"

