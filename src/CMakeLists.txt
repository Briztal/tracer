
include_directories(".")
#------------------------------------------------PROJECT----------------------------------------------------------------

set(PP "Project")

include_directories(${PC}/)

add_library(project

        ${PP}/MachineController.h
        ${PP}/MachineController.cpp

        ${PP}/TemperatureController.cpp
        ${PP}/TemperatureController.h)


#--------------------------------------------PROJECT CONFIG-------------------------------------------------------------


set(PCP "Project/Config")

include_directories(${PCP}/)

add_library(project_config

        ${PCP}/config_file.h
        ${PCP}/lcd_interface_config.h
        ${PCP}/terminal_interface_config.h
        ${PCP}/program_interface_config.h
        ${PCP}/gcode_interface_config.h
        ${PCP}/geometry.cpp
        ${PCP}/eeprom_config.h)


#--------------------------------------PROJECT INTERFACES COMMANDS------------------------------------------------------


set(PIP "Project/InterfaceCommands")

include_directories(${PIP}/)

add_library(project_interfaces

        ${PIP}/TerminalCommands.h
        ${PIP}/TerminalCommands.cpp
        ${PIP}/ProgramInterfaceCommands.h
        ${PIP}/ProgramInterfaceCommands.cpp
        ${PIP}/GCodeCommands.h
        ${PIP}/GCodeCommands.cpp

        )


#-------------------------------------------------------- TRACER -------------------------------------------------------

include_directories(.)

add_library(tracer_config

        Kernel.h
        Kernel.cpp

        config.h
        parameters_indices.h
        hardware_language_abstraction.h
        sanity_check.h

        )

#-----------------------------------------------SCHEDULER---------------------------------------------------------------

set(TP "TaskScheduler")

include_directories(${TP}/)

add_library(scheduler

        ${TP}/TaskScheduler.h
        ${TP}/TaskScheduler.cpp
        ${TP}/scheduler_generation.h
        ${TP}/_task_scheduler_data.h
        ${TP}/task_state_t.h

        )


set(EEP "EEPROM")

include_directories(${EEP}/)

add_library(core

        ${EEP}/EEPROMStorage.h
        ${EEP}/EEPROMStorage.cpp
        ${EEP}/EEPROMNode.h
        ${EEP}/EEPROMNode.cpp
        ${EEP}/EEPROMInterface.h
        ${EEP}/EEPROMInterface.cpp
        ${EEP}/_eeprom_storage_data.h

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

        ${IP}/_interface_data.h
        ${IP}/Interfaces.cpp
        ${IP}/Interfaces.h

        #TerminalInterface
        ${IP}/TerminalInterface/TerminalInterface.h
        ${IP}/TerminalInterface/TerminalInterface.cpp
        ${IP}/TerminalInterface/TerminalTree.h
        ${IP}/TerminalInterface/TerminalTree.cpp
        ${IP}/TerminalInterface/TerminalArguments.h
        ${IP}/TerminalInterface/TerminalArguments.cpp
        ${IP}/TerminalInterface/TerminalTreeGenerator.h
        ${IP}/TerminalInterface/TerminalTreeGenerator.cpp

        #ProgramInterface
        ${IP}/ProgramInterface/ProgramInterface.h
        ${IP}/ProgramInterface/ProgramInterface.cpp

        #GcodeInterface
        ${IP}/GCodeInterface/GCodeInterface.h
        ${IP}/GCodeInterface/GCodeInterface.cpp
        ${IP}/GCodeInterface/GCodeTree.h
        ${IP}/GCodeInterface/GCodeTree.cpp
        ${IP}/GCodeInterface/GCodeTreeGenerator.h
        ${IP}/GCodeInterface/GCodeTreeGenerator.cpp
        ${IP}/GCodeInterface/GCodeArguments.h
        ${IP}/GCodeInterface/GCodeArguments.cpp
        )


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

#---------------------------------------------Control Loops-------------------------------------------------------------


set(CLP "ControlLoops")

include_directories(${CLP}/)

add_library(asserv

        ${CLP}/PID.h
        ${CLP}/PID.cpp

        ${CLP}/ControlLoops.cpp
        ${CLP}/ControlLoops.h

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

        ${DP}/ArgumentsContainer.h
        ${DP}/ArgumentsContainer.cpp

        ${DP}/StringUtils.h
        ${DP}/StringUtils.cpp

        )


#-------------------------------------------------Kinematics Cores------------------------------------------------------


set(KP "StepperControl")
include_directories(${KP}/)

add_library(kinematics_cores

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

        ${KP}/SubMovementManager.cpp
        ${KP}/SubMovementManager.h)


#-------------------------------------------------Core 1---------------------------------------------------------------

set(K1P "StepperControl/KinematicsCore1")
set(K1LP "StepperControl/KinematicsCore1/Movements")
set(K1NP "StepperControl/KinematicsCore1/NonLinearMovements")

include_directories(${K1P}/)
include_directories(${K1LP}/)
include_directories(${K1NP}/)

add_library(core1

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


#-------------------------------------------------Core 1---------------------------------------------------------------

set(K2P "StepperControl/KinematicsCore2")
set(K2MP "StepperControl/KinematicsCore2/Movements")

include_directories(${K2P}/)

add_library(core2

        #core2
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

target_link_libraries(kinematics_cores datastructures)
target_link_libraries(core1 datastructures)
target_link_libraries(core2 datastructures)

target_link_libraries(core kinematics_cores)
target_link_libraries(kinematics_cores interface)
target_link_libraries(kinematics_cores core)

target_link_libraries(core datastructures)
target_link_libraries(kinematics_cores datastructures)

#------------------------------------------------FIRMWARE---------------------------------------------------------------


add_executable(TRACER main.cpp)


