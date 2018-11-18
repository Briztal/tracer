cmake_minimum_required(VERSION 3.2)
project(tracer)
set(ENV{PATH} "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin")
set(PLATFORMIO_CMD "/usr/local/bin/platformio")

SET(CMAKE_C_COMPILER "/root/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-gcc")
SET(CMAKE_CXX_COMPILER "/root/.platformio/packages/toolchain-gccarmnoneeabi/bin/arm-none-eabi-g++")
SET(CMAKE_CXX_FLAGS_DISTRIBUTION "-fno-exceptions -felide-constructors -fno-rtti -std=gnu++14 -Os -Wall -ffunction-sections -fdata-sections -mthumb -mcpu=cortex-m4 -nostdlib -fsingle-precision-constant -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DF_CPU=120000000L -DLAYOUT_US_ENGLISH -DPLATFORMIO=30503 -D__MK64FX512__ -DTEENSY35 -DUSB_SERIAL -DARDUINO=10805 -DTEENSYDUINO=141")
SET(CMAKE_C_FLAGS_DISTRIBUTION "-Os -Wall -ffunction-sections -fdata-sections -mthumb -mcpu=cortex-m4 -nostdlib -fsingle-precision-constant -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DF_CPU=120000000L -DLAYOUT_US_ENGLISH -DPLATFORMIO=30503 -D__MK64FX512__ -DTEENSY35 -DUSB_SERIAL -DARDUINO=10805 -DTEENSYDUINO=141")
set(CMAKE_CXX_STANDARD 11)

add_definitions(-D'F_CPU=120000000L')
add_definitions(-D'LAYOUT_US_ENGLISH')
add_definitions(-D'PLATFORMIO=30503')
add_definitions(-D'__MK64FX512__')
add_definitions(-D'TEENSY35')
add_definitions(-D'USB_SERIAL')
add_definitions(-D'ARDUINO=10805')
add_definitions(-D'TEENSYDUINO=141')

include_directories("$ENV{HOME}/Projects/tracer/src")
include_directories("$ENV{HOME}/Projects/tracer/src/hard/chip/kinetis/periph")
include_directories("$ENV{HOME}/Projects/tracer/src/kernel/mem")
include_directories("$ENV{HOME}/Projects/tracer/src/kernel")
include_directories("$ENV{HOME}/Projects/tracer/src/dmz")

FILE(GLOB_RECURSE SRC_LIST "/root/Projects/tracer/src/*.*")

add_custom_target(
    PLATFORMIO_BUILD ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_UPLOAD ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target upload
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_CLEAN ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target clean
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_TEST ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_PROGRAM ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target program
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_UPLOADFS ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion run --target uploadfs
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_UPDATE_ALL ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion update
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
    PLATFORMIO_REBUILD_PROJECT_INDEX ALL
    COMMAND ${PLATFORMIO_CMD} -f -c clion init --ide clion
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)


add_executable(tracer ${SRC_LIST} )