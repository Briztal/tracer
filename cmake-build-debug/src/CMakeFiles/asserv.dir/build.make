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
CMAKE_SOURCE_DIR = /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug

# Include any dependencies generated for this target.
include src/CMakeFiles/asserv.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/asserv.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/asserv.dir/flags.make

src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o: src/CMakeFiles/asserv.dir/flags.make
src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o: ../src/ControlLoops/PID.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o"
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && /root/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o -c /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/src/ControlLoops/PID.cpp

src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/asserv.dir/ControlLoops/PID.cpp.i"
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && /root/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/src/ControlLoops/PID.cpp > CMakeFiles/asserv.dir/ControlLoops/PID.cpp.i

src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/asserv.dir/ControlLoops/PID.cpp.s"
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && /root/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/src/ControlLoops/PID.cpp -o CMakeFiles/asserv.dir/ControlLoops/PID.cpp.s

src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o.requires:

.PHONY : src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o.requires

src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o.provides: src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/asserv.dir/build.make src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o.provides.build
.PHONY : src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o.provides

src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o.provides.build: src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o


src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o: src/CMakeFiles/asserv.dir/flags.make
src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o: ../src/ControlLoops/ControlLoops.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o"
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && /root/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o -c /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/src/ControlLoops/ControlLoops.cpp

src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.i"
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && /root/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/src/ControlLoops/ControlLoops.cpp > CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.i

src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.s"
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && /root/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/src/ControlLoops/ControlLoops.cpp -o CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.s

src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o.requires:

.PHONY : src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o.requires

src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o.provides: src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/asserv.dir/build.make src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o.provides.build
.PHONY : src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o.provides

src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o.provides.build: src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o


# Object files for target asserv
asserv_OBJECTS = \
"CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o" \
"CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o"

# External object files for target asserv
asserv_EXTERNAL_OBJECTS =

src/libasserv.a: src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o
src/libasserv.a: src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o
src/libasserv.a: src/CMakeFiles/asserv.dir/build.make
src/libasserv.a: src/CMakeFiles/asserv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libasserv.a"
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && $(CMAKE_COMMAND) -P CMakeFiles/asserv.dir/cmake_clean_target.cmake
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/asserv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/asserv.dir/build: src/libasserv.a

.PHONY : src/CMakeFiles/asserv.dir/build

src/CMakeFiles/asserv.dir/requires: src/CMakeFiles/asserv.dir/ControlLoops/PID.cpp.o.requires
src/CMakeFiles/asserv.dir/requires: src/CMakeFiles/asserv.dir/ControlLoops/ControlLoops.cpp.o.requires

.PHONY : src/CMakeFiles/asserv.dir/requires

src/CMakeFiles/asserv.dir/clean:
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src && $(CMAKE_COMMAND) -P CMakeFiles/asserv.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/asserv.dir/clean

src/CMakeFiles/asserv.dir/depend:
	cd /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/src /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src /media/raph/suus/Projets/Robotique/TRACERBOT/TRACER/cmake-build-debug/src/CMakeFiles/asserv.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/asserv.dir/depend

