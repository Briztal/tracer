
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


set(PCP "Config")

include_directories(${PCP}/)

add_library(project_config

        ${PCP}/_ConfigChecker.h
        ${PCP}/_ConfigChecker.cpp

        ${PCP}/_general_config.h
        ${PCP}/actions_config.h
        ${PCP}/control_config.h
        ${PCP}/control_loops_config.h
        ${PCP}/controller_gcode_config.h
        ${PCP}/controller_lcd_config.h
        ${PCP}/controller_lcd_config.h
        ${PCP}/controller_program_config.h
        ${PCP}/controller_terminal_config.h
        ${PCP}/eeprom_config.h
        ${PCP}/geometry.h
        ${PCP}/hl_config.h
        ${PCP}/scheduler_config.h
        ${PCP}/sensors_config.h
        ${PCP}/stepper_control_config.h

        )


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

add_library(EEPROM

        ${EEP}/EEPROMTree.h
        ${EEP}/EEPROMTree.cpp

        ${EEP}/EEPROMMap.h
        ${EEP}/EEPROMMap.cpp

        )


set(HP hardware_language_abstraction)

include_directories(${HP}/)

add_library(hardware

        ${HP}/hardware_language_abstraction_teensy35.h
        ${HP}/hardware_language_abstraction_teensy35.cpp


        )

#-----------------------------------------------INTERFACES--------------------------------------------------------------

set(CIP "Interaction")
set(CLP "Interaction/Languages")
set(CPP "Interaction/Protocols")
include_directories(${CCP}/)



add_library(interaction

        ${CIP}/Interaction.cpp
        ${CIP}/Interaction.h

        ${CLP}/_language_data.h

        #Terminal
        ${CLP}/Terminal/Terminal.h
        ${CLP}/Terminal/Terminal.cpp
        ${CLP}/Terminal/TerminalTree.h
        ${CLP}/Terminal/TerminalTree.cpp
        ${CLP}/Terminal/TerminalArguments.h
        ${CLP}/Terminal/TerminalArguments.cpp
        ${CLP}/Terminal/TerminalTreeGenerator.h
        ${CLP}/Terminal/TerminalTreeGenerator.cpp

        #ProgramInterface
        ${CLP}/Program/ProgramInterface.h
        ${CLP}/Program/ProgramInterface.cpp

        #GcodeInterface
        ${CLP}/GCode/GCode.h
        ${CLP}/GCode/GCode.cpp
        ${CLP}/GCode/GCodeTree.h
        ${CLP}/GCode/GCodeTree.cpp
        ${CLP}/GCode/GCodeTreeGenerator.h
        ${CLP}/GCode/GCodeTreeGenerator.cpp
        ${CLP}/GCode/GCodeArguments.h
        ${CLP}/GCode/GCodeArguments.cpp
        
        )


set(CPP "Interaction/Protocols")

include_directories(${CPP}/)


add_library(protocols

        ${CPP}/Protocol.h

        ${CPP}/ClearText.h
        ${CPP}/ClearText.cpp

        )


#-------------------------------------------------ACTIONS---------------------------------------------------------------


set(AP "Actuators")
include_directories(${AP}/)

add_library(actuators

        ${AP}/actuators.h
        ${AP}/DigitalGPIO.h
        ${AP}/DigitalGPIO.cpp
        ${AP}/PWMGpio.h
        ${AP}/PWMGpio.cpp
        ${AP}/Servo.h
        ${AP}/Servo.cpp

        )



#------------------------------------------------- LinearSolver ---------------------------------------------------------------


set(LsP "LinearSolver")
include_directories(${LsP}/)

add_library(linearSolver

        ${LsP}/CoefficientArray.h
        ${LsP}/CoefficientArray.cpp

        ${LsP}/LinearSystem.h
        ${LsP}/LinearSystem.cpp

        ${LsP}/Matrix.h
        ${LsP}/Matrix.cpp

        )



#------------------------------------------------- LinearSolver ---------------------------------------------------------------


set(DcP "DroneControl")
set(DMrP "DroneControl/MultiRotors")
set(DRoP "DroneControl/Rover")

include_directories(${LsP}/)
include_directories(${DMrP}/)
include_directories(${DRoP}/)

add_library(droneControl

        ${DcP}/DroneTest.h
        ${DcP}/DroneTest.cpp
        ${DcP}/MatrixDrone.h
        ${DcP}/MatrixDrone.cpp
        ${DcP}/MotorRegisterer.h
        ${DcP}/MotorRegisterer.cpp


        ${DMrP}/Generics/SingleHexaCopter.h
        ${DMrP}/Generics/SingleHexaCopter.cpp
        ${DMrP}/Generics/SinglePentaCopter.h
        ${DMrP}/Generics/SinglePentaCopter.cpp
        ${DMrP}/Generics/SingleQuadCopter.h
        ${DMrP}/Generics/SingleQuadCopter.cpp
        ${DMrP}/MatrixMultiRotor/MultiRotorCoordinateSysem.h
        ${DMrP}/MatrixMultiRotor/MultiRotorMotorData.h
        ${DMrP}/MatrixMultiRotor/SolidMultiRotor.h
        ${DMrP}/MatrixMultiRotor/SolidMultiRotor.cpp

        ${DRoP}/Generics/TriRover.h
        ${DRoP}/Generics/TriRover.cpp


        ${DRoP}/MatrixRover/RoverCoordinateSystem.h
        ${DRoP}/MatrixRover/RoverMotorData.h
        ${DRoP}/MatrixRover/MatrixRover.h
        ${DRoP}/MatrixRover/MatrixRover.cpp




        )

#---------------------------------------------Interaction Loops-------------------------------------------------------------


set(ClP "ControlLoops")

include_directories(${CLP}/)

add_library(asserv

        ${ClP}/ControlLoops.h

        ${ClP}/OrientationController.cpp
        ${ClP}/OrientationController.h

        ${ClP}/PID.cpp
        ${ClP}/PID.h

        )


#-------------------------------------------------SENSORS---------------------------------------------------------------


set(SP "Sensors")
include_directories(${SP}/)

add_library(sensors

        ${SP}/Sensors.h

        ${SP}/Thermistors/Thermistors.h
        ${SP}/Thermistors/Thermistors.cpp

        ${SP}/Accelerometers/MPU6050/MPU6050.h
        ${SP}/Accelerometers/MPU6050/MPU6050.cpp

        ${SP}/Endstops.h
        ${SP}/Endstops.cpp

        ${SP}/Thermistors/thermistor_data.h

        )

#---------------------------------------------DATASTRUCTURES------------------------------------------------------------

set(DP "DataStructures")
include_directories(${DP}/)

add_library(datastructures

        ${DP}/Queue.h
        ${DP}/Queue.cpp

        ${DP}/Tree.h
        ${DP}/Tree.cpp

        ${DP}/StringUtils.h
        ${DP}/StringUtils.cpp

        )


#------------------------------------------------- Stepper Control ------------------------------------------------------


set(KP "StepperControl")
include_directories(${KP}/)

add_library(kinematics_cores

        ${KP}/_kinematics_data.h

        ${KP}/StepperController.h
        ${KP}/StepperController.cpp

        ${KP}/TrajectoryTracer.h
        ${KP}/TrajectoryTracer.cpp

        ${KP}/Steppers.h
        ${KP}/Steppers.cpp

        ${KP}/SteppersData.h
        ${KP}/SteppersData.cpp

        ${KP}/IncrementComputer.h
        ${KP}/IncrementComputer.cpp

        ${KP}/MovementCoordinator.cpp
        ${KP}/MovementCoordinator.h

        ${KP}/JerkPlanner.cpp
        ${KP}/JerkPlanner.h

        ${KP}/SubMovementManager.cpp
        ${KP}/SubMovementManager.h)


#------------------------------------------------- Core 1 --------------------------------------------------------------

set(KC1P "StepperControl/KinematicsCore1")

include_directories(${KC1P}/)

add_library(core1

        #general
        ${KC1P}/mathProcess.hpp
        ${KC1P}/mathProcess.cpp
        ${KC1P}/KinematicsCore1.h
        ${KC1P}/KinematicsCore1.cpp
        ${KC1P}/K1Physics.h
        ${KC1P}/K1Physics.cpp
        ${KC1P}/SqrtFastComputer.h
        ${KC1P}/SqrtFastComputer.cpp

        )


#-------------------------------------------------Core 1---------------------------------------------------------------

set(Kc2P "StepperControl/KinematicsCore2")

include_directories(${K2P}/)

add_library(core2

        #core2
        ${Kc2P}/KinematicsCore2.cpp
        ${Kc2P}/KinematicsCore2.h
        ${Kc2P}/K2Physics.h
        ${Kc2P}/K2Physics.cpp

        )

#------------------------------------------------- Movements ---------------------------------------------------------------

set(SMP "StepperControl/Movements")

include_directories(${SMP}/)

add_library(movements


        ${SMP}/ComplexLinearMovement.cpp
        ${SMP}/ComplexLinearMovement.h

        ${SMP}/LinearMovement.cpp
        ${SMP}/LinearMovement.h

        ${SMP}/HomingMovement.h
        ${SMP}/HomingMovement.cpp

        )

#------------------------------------------------FIRMWARE---------------------------------------------------------------


add_executable(TRACER main.cpp)


