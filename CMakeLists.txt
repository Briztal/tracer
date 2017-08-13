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


set(MP "src/StepperControl")
set(MNP "src/StepperControl/LinearMovement")
set(MTP "src/StepperControl/NonLinearMovements")
set(CP "src/StepperControl/ComplexMovements")


add_library(motion

        ${CP}/ComplexMovement.cpp
        ${CP}/ComplexMovement.h
        ${CP}/LinearComplexMovement.cpp
        ${CP}/LinearComplexMovement.h

        ${MP}/StepperAbstraction.cpp ${MP}/StepperController.cpp ${MP}/MovementExecuter.cpp ${MP}/TrajectoryExecuter.cpp ${MP}/SpeedManager.cpp ${MP}/SpeedPlanner.cpp ${MNP}/LinearMovement.cpp

        ${MNP}/HomingMovement.cpp ${MTP}/NonLinearMovement.cpp ${MTP}/NonLinearMovementTemplate.cpp ${MTP}/MotionBezier.cpp
        ${MTP}/MotionEllipses.cpp ${MP}/mathProcess.cpp ${MP}/swap.cpp

        ${MP}/StepperAbstraction.h ${MP}/StepperController.h ${MP}/MovementExecuter.h ${MP}/TrajectoryExecuter.h ${MP}/SpeedManager.h ${MP}/SpeedPlanner.h  ${MNP}/LinearMovement.h
        ${MNP}/HomingMovement.h ${MTP}/NonLinearMovement.h ${MTP}/NonLinearMovementTemplate.h ${MTP}/MotionBezier.h
        ${MTP}/MotionEllipses.h ${MP}/mathProcess.hpp ${MP}/swap.hpp ${MP}/motion_data.h

        src/StepperControl/ComplexMovements/ComplexTrajectoryExecuter.cpp src/StepperControl/ComplexMovements/ComplexTrajectoryExecuter.h)

#--------------------------------------------------LINK----------------------------------------------------------------

target_link_libraries(core actions)
target_link_libraries(core interface)
target_link_libraries(actions external)
target_link_libraries(core asserv)


target_link_libraries(core motion)
target_link_libraries(motion interface)
target_link_libraries(motion core)

target_link_libraries(core datastructures)
target_link_libraries(motion datastructures)

#------------------------------------------------FIRMWARE---------------------------------------------------------------

add_executable(TRACER src/main.cpp)

