cmake_minimum_required(VERSION 3.2)
project(tracer)


set(PLATFORMIO_CMD "/usr/local/bin/platformio")

set(CMAKE_C_STANDARD 90)

include_directories("arch")
include_directories("include")
include_directories("include/nostd")
include_directories("arch/chip/kinetis/periph")
#include_directories("kernel/mem")
#include_directories("kernel")

FILE(GLOB_RECURSE SRCS kernel/*.* include/nostd/*.* arch/*.* std/*.* include/*.*)

add_executable(tracer ${SRCS} )