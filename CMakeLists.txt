cmake_minimum_required(VERSION 2.8.4)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)

project(TRACER C CXX)

set(BOARD mega)
set(PORT /dev/ttyACM0)

set(mega.build.mcu atmega2560)
set(mega.upload.protocol wiring)
set(mega.upload.regulation_speed 115200)

#--------------------------------------------LOW LEVEL LIBS-------------------------------------------------------------
set(CP "src/Core")

generate_arduino_library(core

        SRCS ${CP}/Core.cpp ${CP}/MachineControllerSystem.cpp ${CP}/MachineControllerCustom.cpp
        ${CP}/hardware_language_abstraction.cpp ${CP}/EEPROMStorage.cpp

        HDRS ${CP}/Core.h ${CP}/MachineControllerSystem.h ${CP}/hardware_language_abstraction.h ${CP}/EEPROMStorage.h

        BOARD ${BOARD}

        )

set(IP "src/Interfaces")

generate_arduino_library(interface

        SRCS ${IP}/CommandInterface.cpp

        HDRS ${IP}/CommandInterface.h ${IP}/interface_config.h

        BOARD ${BOARD}

        )

set(AP "src/Actions")

generate_arduino_library(actions

        SRCS ${AP}/ServoActions.cpp ${AP}/BinaryActions.cpp ${AP}/ContinuousActions.cpp

        HDRS ${AP}/ServoActions.h ${AP}/BinaryActions.h  ${AP}/ContinuousActions.h

        BOARD ${BOARD}

        )

set(AsP "src/Core")

generate_arduino_library(asserv

        SRCS ${AsP}/Asserv.cpp

        HDRS ${AsP}/Asserv.h

        BOARD ${BOARD}

        )


set(DP "src/DataStructures")

generate_arduino_library(datastructures

        SRCS ${DP}/ArgQueue.cpp  ${DP}/Queue.cpp

        HDRS  ${DP}/ArgQueue.h  ${DP}/Queue.h

        BOARD ${BOARD}

        )


set(EP "src/External")

generate_arduino_library(external

        SRCS ${EP}/digitalWriteFast.h ${EP}/Servo.cpp

        HDRS ${EP}/Servo.h ${EP}/ServoTimers.h

        BOARD ${BOARD}

        )


set(MP "src/StepperControl")
set(MNP "src/StepperControl/LinearMotionN")
set(MTP "src/StepperControl/NonLinearMotions")
generate_arduino_library(motion

        SRCS ${MP}/StepperController.cpp ${MP}/MotionExecuter.cpp ${MP}/SpeedManager.cpp ${MP}/MotionScheduler.cpp ${MNP}/LinearMotionN.cpp

        ${MNP}/HomingMotion.cpp ${MTP}/NonLinearMotionN.cpp ${MTP}/NonLinearMotionNTemplate.cpp ${MTP}/MotionBezier.cpp
        ${MTP}/MotionEllipses.cpp ${MP}/mathProcess.cpp ${MP}/swap.cpp

        HDRS  ${MP}/StepperController.h ${MP}/MotionExecuter.h ${MP}/SpeedManager.h ${MP}/MotionScheduler.h  ${MNP}/LinearMotionN.h
        ${MNP}/HomingMotion.h ${MTP}/NonLinearMotionN.h ${MTP}/NonLinearMotionNTemplate.h ${MTP}/MotionBezier.h
        ${MTP}/MotionEllipses.h ${MP}/mathProcess.hpp ${MP}/swap.hpp ${MP}/motion_data.h

        BOARD ${BOARD}

        )

#--------------------------------------------------LINK----------------------------------------------------------------

target_link_libraries(core actions)
target_link_libraries(core external)
target_link_libraries(core interface)
target_link_libraries(actions external)
target_link_libraries(core asserv)


target_link_libraries(core motion)
target_link_libraries(motion interface)
target_link_libraries(motion core)

target_link_libraries(core datastructures)
target_link_libraries(motion datastructures)

#------------------------------------------------FIRMWARE---------------------------------------------------------------

#TODO ADD YOUR LIBS
generate_arduino_firmware(${CMAKE_PROJECT_NAME}
        SKETCH TRACER.ino
        LIBS core external actions asserv interface
        BOARD ${BOARD}
        PORT ${PORT}
        )