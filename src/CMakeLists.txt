
#--------------------------------------------LOW LEVEL LIBS-------------------------------------------------------------

include_directories()


#--------------------------------------------------CORE-----------------------------------------------------------------

set(CP "Core")

include_directories(${CP}/)

add_library(core

        config.h
        parameters_indices.h
        hardware_language_abstraction.h
        interface.h
        sanity_check.h

        ${CP}/Core.h
        ${CP}/Core.cpp
        ${CP}/EEPROMStorage.h
        ${CP}/EEPROMStorage.cpp

        )


set(HP hardware_language_abstraction)

include_directories(${HP}/)

add_library(hardware

        ${HP}/hardware_language_abstraction_teensy35.h
        ${HP}/hardware_language_abstraction_teensy35.cpp


        )

#-----------------------------------------------INTERFACES--------------------------------------------------------------

set(IP "Interfaces")
include_directories(${IP}/)

add_library(interface

        #LCDInterface
        ${IP}/LCDInterface/LCDInterface.h
        ${IP}/LCDInterface/LCDInterface.cpp
        ${IP}/LCDInterface/LCDNode.h
        ${IP}/LCDInterface/LCDNode.cpp
        ${IP}/LCDInterface/lcd_interface_config.h


        #TerminalInterface
        ${IP}/TerminalInterface/TerminalInterface.h
        ${IP}/TerminalInterface/TerminalInterface.cpp
        ${IP}/TerminalInterface/TerminalInterfaceCommands.h
        ${IP}/TerminalInterface/TerminalInterfaceCommands.cpp
        ${IP}/TerminalInterface/terminal_interface_config.h

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

        Interfaces/TerminalInterface/TerminalNode.cpp Interfaces/TerminalInterface/TerminalNode.h)

#-------------------------------------------------ACTIONS---------------------------------------------------------------


set(AP "Actions")
include_directories(${AP}/)

add_library(actions

        ${AP}/ServoActions.h
        ${AP}/ServoActions.cpp
        ${AP}/BinaryActions.h
        ${AP}/BinaryActions.cpp
        ${AP}/ContinuousActions.h
        ${AP}/ContinuousActions.cpp

        )

#-------------------------------------------------ASSERV----------------------------------------------------------------


set(AsP "Core")

include_directories(${AsP}/)

add_library(asserv

        ${AsP}/Asserv.h
        ${AsP}/Asserv.cpp

        )


#-------------------------------------------------SENSORS---------------------------------------------------------------


set(SP "Sensors")
include_directories(${SP}/)

add_library(sensors

        ${SP}/Thermistors/Thermistors.h
        ${SP}/Thermistors/Thermistors.cpp
        ${SP}/Thermistors/thermistor_data.h

        )

#---------------------------------------------DATASTRUCTURES------------------------------------------------------------

set(DP "DataStructures")
include_directories(${DP}/)

add_library(datastructures

        ${DP}/Queue.h
        ${DP}/Queue.cpp

        DataStructures/Node.cpp DataStructures/Node.h)

#-------------------------------------------------KERNELS---------------------------------------------------------------


set(KP "StepperControl")
include_directories(${KP}/)

add_library(kernels

        ${KP}/_kinematics_data.h

        ${KP}/MachineInterface.h
        ${KP}/MachineInterface.cpp
        ${KP}/TrajectoryTracer.h
        ${KP}/TrajectoryTracer.cpp
        ${KP}/StepperController.h
        ${KP}/StepperController.cpp
        ${KP}/IncrementComputer.h
        ${KP}/IncrementComputer.cpp
        ${KP}/JerkPlanner.cpp
        ${KP}/JerkPlanner.h

        StepperControl/SubMovementManager.cpp StepperControl/SubMovementManager.h)


#-------------------------------------------------KERNEL1---------------------------------------------------------------

set(K1P "StepperControl/KinematicsCore1")
set(K1LP "StepperControl/KinematicsCore1/Movements")
set(K1NP "StepperControl/KinematicsCore1/NonLinearMovements")

include_directories(${K1P}/)
include_directories(${K1LP}/)
include_directories(${K1NP}/)

add_library(kernel1

        #general
        ${K1P}/mathProcess.hpp
        ${K1P}/mathProcess.cpp
        ${K1P}/KinematicsCore1.h
        ${K1P}/KinematicsCore1.cpp
        ${K1P}/K1Physics.h
        ${K1P}/K1Physics.cpp
        ${K1P}/SqrtFastComputer.h
        ${K1P}/SqrtFastComputer.cpp

        #Linear Movements
        ${K1LP}/HomingMovement.h
        ${K1LP}/HomingMovement.cpp
        ${K1LP}/LinearMovement.h
        ${K1LP}/LinearMovement.cpp

        )


#-------------------------------------------------KERNEL2---------------------------------------------------------------

set(K2P "StepperControl/KinematicsCore2")
set(K2MP "StepperControl/KinematicsCore2/Movements")

include_directories(${K2P}/)

add_library(kernel2

        #kernel2
        ${K2P}/KinematicsCore2.cpp
        ${K2P}/KinematicsCore2.h
        ${K2P}/K2Physics.h
        ${K2P}/K2Physics.cpp


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


add_executable(TRACER main.cpp)