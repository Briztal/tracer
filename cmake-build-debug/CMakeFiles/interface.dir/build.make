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
include CMakeFiles/interface.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/interface.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/interface.dir/flags.make

CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o: CMakeFiles/interface.dir/flags.make
CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o: ../src/Interfaces/CommandInterface.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o -c /media/raph/Data2/Projets/Robotique/TRACER/src/Interfaces/CommandInterface.cpp

CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.i"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/raph/Data2/Projets/Robotique/TRACER/src/Interfaces/CommandInterface.cpp > CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.i

CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.s"
	/home/raph/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/raph/Data2/Projets/Robotique/TRACER/src/Interfaces/CommandInterface.cpp -o CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.s

CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o.requires:

.PHONY : CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o.requires

CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o.provides: CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o.requires
	$(MAKE) -f CMakeFiles/interface.dir/build.make CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o.provides.build
.PHONY : CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o.provides

CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o.provides.build: CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o


# Object files for target interface
interface_OBJECTS = \
"CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o"

# External object files for target interface
interface_EXTERNAL_OBJECTS =

libinterface.a: CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o
libinterface.a: CMakeFiles/interface.dir/build.make
libinterface.a: CMakeFiles/interface.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libinterface.a"
	$(CMAKE_COMMAND) -P CMakeFiles/interface.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/interface.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/interface.dir/build: libinterface.a

.PHONY : CMakeFiles/interface.dir/build

CMakeFiles/interface.dir/requires: CMakeFiles/interface.dir/src/Interfaces/CommandInterface.cpp.o.requires

.PHONY : CMakeFiles/interface.dir/requires

CMakeFiles/interface.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/interface.dir/cmake_clean.cmake
.PHONY : CMakeFiles/interface.dir/clean

CMakeFiles/interface.dir/depend:
	cd /media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/raph/Data2/Projets/Robotique/TRACER /media/raph/Data2/Projets/Robotique/TRACER /media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug /media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug /media/raph/Data2/Projets/Robotique/TRACER/cmake-build-debug/CMakeFiles/interface.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/interface.dir/depend
