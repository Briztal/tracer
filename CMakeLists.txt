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

include_directories(src/)


#--------------------------------------------------CORE-----------------------------------------------------------------

set(CP "src/Core")

include_directories(${CP}/)

add_library(core

        ${CP}/Core.h
        ${CP}/Core.cpp
        ${CP}/EEPROMStorage.h
        ${CP}/EEPROMStorage.cpp

        )


set(HP src/hardware_language_abstraction)

include_directories(${HP}/)

add_library(hardware

        ${HP}/hardware_language_abstraction_teensy35.cpp

        ${HP}/digital_write_fast.h)


#-----------------------------------------------INTERFACES--------------------------------------------------------------

set(IP "src/Interfaces")
include_directories(${IP}/)

add_library(interface

        #TreeInterface
        ${IP}/TreeInterface/TreeInterface.h
        ${IP}/TreeInterface/TreeInterface.cpp
        ${IP}/TreeInterface/TreeInterfaceCommands.h
        ${IP}/TreeInterface/TreeInterfaceCommands.cpp
        ${IP}/TreeInterface/tree_interface_config.h

        #GcodeInterface
        ${IP}/GCodeInterface/GCodeInterface.h
        ${IP}/GCodeInterface/GCodeInterface.cpp
        ${IP}/GCodeInterface/GCodeInterfaceCommands.h
        ${IP}/GCodeInterface/GCodeInterfaceCommands.cpp
        ${IP}/GCodeInterface/gcode_interface_config.h

        )

#-------------------------------------------------ACTIONS---------------------------------------------------------------


set(AP "src/Actions")
include_directories(${AP}/)

add_library(actions

        ${AP}/ServoActions.h
        ${AP}/ServoActions.cpp
        ${AP}/BinaryActions.h
        ${AP}/BinaryActions.cpp
        ${AP}/ContinuousActions.h
        ${AP}/ContinuousActions.cpp

        src/StepperControl/Kernel2/Movements/ComplexLinearMovement.cpp src/StepperControl/Kernel2/Movements/ComplexLinearMovement.h)

#-------------------------------------------------ASSERV----------------------------------------------------------------


set(AsP "src/Core")

include_directories(${AsP}/)

add_library(asserv

        ${AsP}/Asserv.h
        ${AsP}/Asserv.cpp

        )


#-------------------------------------------------SENSORS---------------------------------------------------------------


set(SP "src/Sensors")
include_directories(${SP}/)

add_library(sensors

        ${SP}/Thermistors/Thermistors.h
        ${SP}/Thermistors/Thermistors.cpp
        ${SP}/Thermistors/thermistor_data.h

        )

#---------------------------------------------DATASTRUCTURES------------------------------------------------------------

set(DP "src/DataStructures")
include_directories(${DP}/)

add_library(datastructures

        ${DP}/Queue.h
        ${DP}/Queue.cpp

        )

#-------------------------------------------------KERNELS---------------------------------------------------------------


set(KP "src/StepperControl")
include_directories(${KP}/)

add_library(kernels

        ${KP}/MachineAbstraction.h
        ${KP}/MachineAbstraction.cpp
        ${KP}/StepperController.h
        ${KP}/StepperController.cpp

        src/StepperControl/StepperKernel.cpp src/StepperControl/StepperKernel.h)

#-------------------------------------------------KERNEL1---------------------------------------------------------------


set(K1P "src/StepperControl/Kernel1")
set(K1LP "src/StepperControl/Kernel1/LinearMovement")
set(K1NP "src/StepperControl/Kernel1/NonLinearMovements")

include_directories(${K1P}/)
include_directories(${K1LP}/)
include_directories(${K1NP}/)

add_library(kernel1

        #general
        ${K1P}/motion_data.h
        ${K1P}/mathProcess.hpp
        ${K1P}/mathProcess.cpp
        ${K1P}/MovementExecuter.h
        ${K1P}/MovementExecuter.cpp
        ${K1P}/SpeedManager.h
        ${K1P}/SpeedManager.cpp
        ${K1P}/SpeedPlanner.h
        ${K1P}/SpeedPlanner.cpp
        ${K1P}/swap.hpp
        ${K1P}/swap.cpp
        ${K1P}/TrajectoryExecuter.h
        ${K1P}/TrajectoryExecuter.cpp
        ${K1P}/SpeedManager.h
        ${K1P}/SpeedManager.cpp
        ${K1P}/SqrtFastComputer.h
        ${K1P}/SqrtFastComputer.cpp

        #Linear Movements
        ${K1LP}/HomingMovement.h
        ${K1LP}/HomingMovement.cpp
        ${K1LP}/LinearMovement.h
        ${K1LP}/LinearMovement.cpp

        )


#-------------------------------------------------KERNEL2---------------------------------------------------------------


set(K2P "src/StepperControl/Kernel2")
set(K2MP "src/StepperControl/Kernel2/Movements")

include_directories(${K2P}/)

add_library(kernel2

        #kernel2
        ${K2P}/_kernel_2_data.h
        ${K2P}/ComplexTrajectoryExecuter.h
        ${K2P}/ComplexTrajectoryExecuter.cpp
        ${K2P}/RealTimeProcessor.h
        ${K2P}/RealTimeProcessor.cpp
        ${K2P}/PreProcessor.h
        ${K2P}/PreProcessor.cpp

        #movements
        ${K2MP}/ComplexLinearMovement.h
        ${K2MP}/ComplexLinearMovement.cpp

        )
#--------------------------------------------------LINK----------------------------------------------------------------

target_link_libraries(core actions)
target_link_libraries(core interface)
target_link_libraries(actions external)
target_link_libraries(core asserv)

target_link_libraries(kernels datastructures)
target_link_libraries(kernel1 datastructures)
target_link_libraries(kernel2 datastructures)

target_link_libraries(core kernels)
target_link_libraries(kernels interface)
target_link_libraries(kernels core)

target_link_libraries(core datastructures)
target_link_libraries(kernels datastructures)

#------------------------------------------------FIRMWARE---------------------------------------------------------------


add_executable(TRACER src/main.cpp)