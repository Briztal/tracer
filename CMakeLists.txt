cmake_minimum_required(VERSION 3.2)
project(TRACER)


include(CMakeListsPrivate.txt)

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


#--------------------------------------------LOW LEVEL LIBS-------------------------------------------------------------

include_directories(src/MovementKernels)

set(CP "src/Core")
set(HP src/hardware_language_abstraction)

add_library(core

         ${CP}/Core.cpp ${HP}/hardware_language_abstraction_teensy35.cpp
        ${CP}/EEPROMStorage.cpp

         ${CP}/Core.h  ${CP}/EEPROMStorage.h  ${HP}/hardware_language_abstraction_teensy35.cpp

        )

set(IP "src/Interfaces")

add_library(interface

        ${IP}/TreeInterface/TreeInterface.h ${IP}/TreeInterface/TreeInterface.cpp
        ${IP}/TreeInterface/TreeInterfaceCommands.h ${IP}/TreeInterface/TreeInterfaceCommands.cpp
        ${IP}/TreeInterface/tree_interface_config.h

        ${IP}/GCodeInterface/GCodeInterface.h ${IP}/GCodeInterface/GCodeInterface.cpp
        ${IP}/GCodeInterface/GCodeInterfaceCommands.h ${IP}/GCodeInterface/GCodeInterfaceCommands.cpp
        ${IP}/GCodeInterface/gcode_interface_config.h

        )

set(AP "src/Actions")

add_library(actions

         ${AP}/ServoActions.cpp ${AP}/BinaryActions.cpp ${AP}/ContinuousActions.cpp

         ${AP}/ServoActions.h ${AP}/BinaryActions.h  ${AP}/ContinuousActions.h

        )

set(AsP "src/Core")

add_library(asserv

        ${AsP}/Asserv.cpp

        ${AsP}/Asserv.h

        )


set(SP "src/Sensors")

add_library(sensors

        ${SP}/Thermistors/Thermistors.cpp

        ${SP}/Thermistors/Thermistors.h
        ${SP}/Thermistors/thermistor_data.h

        )


set(DP "src/DataStructures")

add_library(datastructures

        ${DP}/Queue.h
        ${DP}/Queue.cpp

        )

#-------------------------------------------------KERNELS---------------------------------------------------------------


set(KP "src/MovementKernels")

add_library(kernels

        ${KP}/MovementExecuter.h
        ${KP}/MovementExecuter.cpp
        ${KP}/StepperAbstraction.h
        ${KP}/StepperAbstraction.cpp
        ${KP}/StepperController.h
        ${KP}/StepperController.cpp

        )

#-------------------------------------------------KERNEL2---------------------------------------------------------------


set(K2P "src/MovementKernels/MovementKernel2")

add_library(kernel2

        ${K2P}/complex_motion_data.h
        ${K2P}/ComplexTrajectoryExecuter.h
        ${K2P}/ComplexTrajectoryExecuter.cpp
        ${K2P}/PreProcessor.h
        ${K2P}/PreProcessor.cpp
        ${K2P}/SqrtFastComputer.h
        ${K2P}/SqrtFastComputer.cpp

        )

#--------------------------------------------------LINK----------------------------------------------------------------

target_link_libraries(core actions)
target_link_libraries(core interface)
target_link_libraries(actions external)
target_link_libraries(core asserv)


target_link_libraries(core kernels)
target_link_libraries(kernels interface)
target_link_libraries(kernels core)

target_link_libraries(core datastructures)
target_link_libraries(kernels datastructures)

#------------------------------------------------FIRMWARE---------------------------------------------------------------



add_executable(TRACER src/main.cpp)