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

         ${CP}/Core.cpp ${CP}/MachineControllerSystem.cpp ${CP}/MachineControllerCustom.cpp ${HP}/hardware_language_abstraction_teensy3.cpp
        ${CP}/EEPROMStorage.cpp

         ${CP}/Core.h ${CP}/MachineControllerSystem.h ${CP}/EEPROMStorage.h  ${HP}/hardware_language_abstraction_teensy3.cpp

       

        )

set(IP "src/Interfaces")

add_library(interface

         ${IP}/CommandInterface.cpp

         ${IP}/CommandInterface.h ${IP}/interface_config.h

       

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


set(DP "src/DataStructures")

add_library(datastructures

         ${DP}/ArgQueue.cpp  ${DP}/Queue.cpp

          ${DP}/ArgQueue.h  ${DP}/Queue.h

       

        )




set(MP "src/StepperControl")
set(MNP "src/StepperControl/LinearMotionN")
set(MTP "src/StepperControl/NonLinearMotions")
add_library(motion

         ${MP}/StepperController.cpp ${MP}/MotionExecuter.cpp ${MP}/SpeedManager.cpp ${MP}/MotionScheduler.cpp ${MNP}/LinearMotionN.cpp

        ${MNP}/HomingMotion.cpp ${MTP}/NonLinearMotionN.cpp ${MTP}/NonLinearMotionNTemplate.cpp ${MTP}/MotionBezier.cpp
        ${MTP}/MotionEllipses.cpp ${MP}/mathProcess.cpp ${MP}/swap.cpp

          ${MP}/StepperController.h ${MP}/MotionExecuter.h ${MP}/SpeedManager.h ${MP}/MotionScheduler.h  ${MNP}/LinearMotionN.h
        ${MNP}/HomingMotion.h ${MTP}/NonLinearMotionN.h ${MTP}/NonLinearMotionNTemplate.h ${MTP}/MotionBezier.h
        ${MTP}/MotionEllipses.h ${MP}/mathProcess.hpp ${MP}/swap.hpp ${MP}/motion_data.h

       

        )

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

