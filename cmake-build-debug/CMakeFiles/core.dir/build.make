# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/clion-2017.1.1/bin/cmake/bin/cmake

# The command to remove a file.
RM = /opt/clion-2017.1.1/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /media/raph/Data2/Projets/Robotique/TRACER

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/core.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/core.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/core.dir/flags.make

CMakeFiles/core.dir/src/Core/Core.cpp.o: CMakeFiles/core.dir/flags.make
CMakeFiles/core.dir/src/Core/Core.cpp.o: ../src/Core/Core.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/core.dir/src/Core/Core.cpp.o"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/core.dir/src/Core/Core.cpp.o -c /media/raph/Data2/Projets/Robotique/TRACER/src/Core/Core.cpp

CMakeFiles/core.dir/src/Core/Core.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/core.dir/src/Core/Core.cpp.i"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/raph/Data2/Projets/Robotique/TRACER/src/Core/Core.cpp > CMakeFiles/core.dir/src/Core/Core.cpp.i

CMakeFiles/core.dir/src/Core/Core.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/core.dir/src/Core/Core.cpp.s"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/raph/Data2/Projets/Robotique/TRACER/src/Core/Core.cpp -o CMakeFiles/core.dir/src/Core/Core.cpp.s

CMakeFiles/core.dir/src/Core/Core.cpp.o.requires:

.PHONY : CMakeFiles/core.dir/src/Core/Core.cpp.o.requires

CMakeFiles/core.dir/src/Core/Core.cpp.o.provides: CMakeFiles/core.dir/src/Core/Core.cpp.o.requires
	$(MAKE) -f CMakeFiles/core.dir/build.make CMakeFiles/core.dir/src/Core/Core.cpp.o.provides.build
.PHONY : CMakeFiles/core.dir/src/Core/Core.cpp.o.provides

CMakeFiles/core.dir/src/Core/Core.cpp.o.provides.build: CMakeFiles/core.dir/src/Core/Core.cpp.o


CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o: CMakeFiles/core.dir/flags.make
CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o: ../src/Core/MachineControllerSystem.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o -c /media/raph/Data2/Projets/Robotique/TRACER/src/Core/MachineControllerSystem.cpp

CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.i"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/raph/Data2/Projets/Robotique/TRACER/src/Core/MachineControllerSystem.cpp > CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.i

CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.s"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/raph/Data2/Projets/Robotique/TRACER/src/Core/MachineControllerSystem.cpp -o CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.s

CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o.requires:

.PHONY : CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o.requires

CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o.provides: CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o.requires
	$(MAKE) -f CMakeFiles/core.dir/build.make CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o.provides.build
.PHONY : CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o.provides

CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o.provides.build: CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o


CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o: CMakeFiles/core.dir/flags.make
CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o: ../src/Core/MachineControllerCustom.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o -c /media/raph/Data2/Projets/Robotique/TRACER/src/Core/MachineControllerCustom.cpp

CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.i"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/raph/Data2/Projets/Robotique/TRACER/src/Core/MachineControllerCustom.cpp > CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.i

CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.s"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/raph/Data2/Projets/Robotique/TRACER/src/Core/MachineControllerCustom.cpp -o CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.s

CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o.requires:

.PHONY : CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o.requires

CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o.provides: CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o.requires
	$(MAKE) -f CMakeFiles/core.dir/build.make CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o.provides.build
.PHONY : CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o.provides

CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o.provides.build: CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o


CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o: CMakeFiles/core.dir/flags.make
CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o: ../src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o -c /media/raph/Data2/Projets/Robotique/TRACER/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp

CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.i"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/raph/Data2/Projets/Robotique/TRACER/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp > CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.i

CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.s"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/raph/Data2/Projets/Robotique/TRACER/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp -o CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.s

CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.requires:

.PHONY : CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.requires

CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.provides: CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.requires
	$(MAKE) -f CMakeFiles/core.dir/build.make CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.provides.build
.PHONY : CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.provides

CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.provides.build: CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o


CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o: CMakeFiles/core.dir/flags.make
CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o: ../src/Core/EEPROMStorage.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o -c /media/raph/Data2/Projets/Robotique/TRACER/src/Core/EEPROMStorage.cpp

CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.i"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/raph/Data2/Projets/Robotique/TRACER/src/Core/EEPROMStorage.cpp > CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.i

CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.s"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/raph/Data2/Projets/Robotique/TRACER/src/Core/EEPROMStorage.cpp -o CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.s

CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o.requires:

.PHONY : CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o.requires

CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o.provides: CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o.requires
	$(MAKE) -f CMakeFiles/core.dir/build.make CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o.provides.build
.PHONY : CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o.provides

CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o.provides.build: CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o


# Object files for target core
core_OBJECTS = \
"CMakeFiles/core.dir/src/Core/Core.cpp.o" \
"CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o" \
"CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o" \
"CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o" \
"CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o"

# External object files for target core
core_EXTERNAL_OBJECTS =

libcore.a: CMakeFiles/core.dir/src/Core/Core.cpp.o
libcore.a: CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o
libcore.a: CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o
libcore.a: CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o
libcore.a: CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o
libcore.a: CMakeFiles/core.dir/build.make
libcore.a: CMakeFiles/core.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX static library libcore.a"
	$(CMAKE_COMMAND) -P CMakeFiles/core.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/core.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/core.dir/build: libcore.a

.PHONY : CMakeFiles/core.dir/build

CMakeFiles/core.dir/requires: CMakeFiles/core.dir/src/Core/Core.cpp.o.requires
CMakeFiles/core.dir/requires: CMakeFiles/core.dir/src/Core/MachineControllerSystem.cpp.o.requires
CMakeFiles/core.dir/requires: CMakeFiles/core.dir/src/Core/MachineControllerCustom.cpp.o.requires
CMakeFiles/core.dir/requires: CMakeFiles/core.dir/src/hardware_language_abstraction/hardware_language_abstraction_teensy35.cpp.o.requires
CMakeFiles/core.dir/requires: CMakeFiles/core.dir/src/Core/EEPROMStorage.cpp.o.requires

.PHONY : CMakeFiles/core.dir/requires

CMakeFiles/core.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/core.dir/cmake_clean.cmake
.PHONY : CMakeFiles/core.dir/clean

CMakeFiles/core.dir/depend:
	cd /media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/raph/Data2/Projets/Robotique/TRACER /media/raph/Data2/Projets/Robotique/TRACER /media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug /media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug /media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug/CMakeFiles/core.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/core.dir/depend

