/*
  TerminalInterfaceCommands.cpp - Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/


#ifdef ENABLE_TERMINAL_INTERFACE

#include "TerminalPipe.h"

#include <Config/control_config.h>
#include <Kernel/Interaction/Delimiter/LineDelimiter.h>
#include <Kernel/Interaction/Language/Languages/TerminalLanguage.h>
#include <Kernel/kernel.h>
#include <Project/MachineController.h>
#include <Project/TemperatureController.h>
#include <Sensors/Thermistors/Thermistors.h>
#include <Sensors/Accelerometers/MPU6050/MPU6050.h>
#include <Math/Matrix.h>
#include <Filters/KalmanFilter/KalmanFilters/IMUKalmanFIlter.h>
#include <ControlSystem/DataConversion/VectorConversion/Vector3DToAngles.h>
#include <ControlSystem/DataConversion/OrientationConversion/Orientation2DToRotation.h>
#include <ControlSystem/DataConversion/VectorConversion/AnglesToVector3D.h>

#include "Kernel/Storage/EEPROMMap.h"

#include "hardware_language_abstraction.h"

namespace  TerminalPipe {
    
    //-----------------------------------------------Custom functions---------------------------------------------------

    task_state_t eeprom();
    
    task_state_t home();
    task_state_t line();
    
    task_state_t enable_steppers();
    task_state_t set_position();
    task_state_t set_extrusion();
    task_state_t set_cooling();
    task_state_t set_hotend();
    task_state_t set_hotbed();

    task_state_t get_temps();
    task_state_t get_regulations();

    task_state_t test_steppers();
    task_state_t test_temp();
    task_state_t test_mpu();
    task_state_t test_kalman();
    task_state_t test_rotation();
    task_state_t test_angles();
    task_state_t test_flood();
    task_state_t test_action();

    //The language we will use;
    TerminalLanguage * language;

};


/*
 * This function creates a interaction pipe and memory it in the kernel;
 */

void TerminalPipe::kernel_registration() {

    //Instantiate our language;
    language = new TerminalLanguage(15, 8, 10);

    //Instantiate our pipe;
    CommunicationPipe *pipe = new CommunicationPipe(Serial, new LineDelimiter(100), language);

    //Register the pipe to the kernel;
    Kernel::register_communication_pipe(pipe);

}


/*
 * This function creates the language's tree;
 */

void TerminalPipe::initialise_data() {


    language->clear();

    language->addCommand("eeprom", eeprom);

    language->addCommand("move home", home);
    language->addCommand("move line", line);

    language->addCommand("set power", enable_steppers);
    language->addCommand("set position", set_position);
    language->addCommand("set suus", set_extrusion);
    language->addCommand("set hotend", set_hotend);
    language->addCommand("set hotbed", set_hotbed);

    language->addCommand("get temp", get_temps);
    language->addCommand("get regul", get_regulations);

    language->addCommand("test machine_control", test_steppers);
    language->addCommand("test temp", test_temp);
    language->addCommand("test mpu", test_mpu);
    language->addCommand("test kalman", test_kalman);
    language->addCommand("test rotation", test_kalman);
    language->addCommand("test angles", test_angles);
    language->addCommand("test flood", test_flood);
    language->addCommand("test action", test_action);

    language->sort();
}


//TODO COMMENT AND COMMENT GCODEINTERFACECOMMANDS
#define PARSE_ARGUMENTS(arguments) {if (!language->parse_arguments(arguments)) return invalid_arguments;}

#define REQUIRE_ALL_ARGUMENTS(arg_string) {if (!language->verify_all_identifiers_presence(arg_string)) return invalid_arguments;}

#define REQUIRE_ONE_ARGUMENTS(arg_string) {if (!language->verify_one_identifiers_presence(arg_string)) return invalid_arguments;}

#define CHECK_ARGUMENT(identifier) (language->verify_identifier_presence(identifier))

#define GET_ARG(identifier) language->get_argument(identifier)

#define GET_ARG_VALUE(identifier) language->get_argument_value(identifier)



task_state_t TerminalPipe::test_flood() {

    //TODO TaskScheduler::flood_enabled = true;

    std_out("EXIT");

    return complete;

}


task_state_t TerminalPipe::test_action() {

    //DroneTest *copter = new DroneTest();

    //copter->solve();

    return complete;

}

//--------------------------------------------------------Storage--------------------------------------------------------

task_state_t TerminalPipe::eeprom() {


    //verify that function and p data are provided.
    REQUIRE_ONE_ARGUMENTS("n");


    //get the cache_path;
    const char *name = GET_ARG('n');

    //Cache the cache_path, in case of multiple uses;
    uint8_t size = cstring::length(name) + (uint8_t) 1;

    //Declare the name cache;
    char cache_path[size];

    //Cache the name;
    strncpy(cache_path, name, size);

    //If the value must be written
    if (CHECK_ARGUMENT('w')) {

        //Extract the value to write_data
        float new_value = GET_ARG_VALUE('w');

        //Log message
        std_out(string("Writing ") + cache_path + " to " + new_value);

        //write the variable
        EEPROMMap::write_data_by_string(cache_path, new_value);

        //Cache the name;
        strncpy(cache_path, name, size);

    }


    //If the value must be printed
    if (CHECK_ARGUMENT('p')) {

        float read_value;

        //Read and display the value.
        if (EEPROMMap::read_data_by_string(cache_path, &read_value)) {

            //Log message
            std_out("Value for " + string(name) + " : " + read_value);

        }

        //Cache the name;
        strncpy(cache_path, name, size);

    }


    //If the profile must be printed
    if (CHECK_ARGUMENT('d')) {

        //Print the current profile.
        EEPROMMap::print_tree(cache_path);

        //Cache the name;
        strncpy(cache_path, name, size);

    }


    //Reset the tree corresponding to the cache_path;
    if (CHECK_ARGUMENT('r')) {
        //If the default profile must be reset

        std_out("Reseting the Storage default profile.");

        //Cache the name;
        strncpy(cache_path, name, size);

        //Reset
        EEPROMMap::reset(cache_path);

    }

    //Save the Storage if required;
    if (CHECK_ARGUMENT('s')) {

        //Save;
        EEPROMMap::save_eeprom_data();

        std_out("Data saved in the Storage;");
    }

    //Succeed
    return complete;

}

//-------------------------------------------------------Movement-------------------------------------------------------

/*
 * IMPORTANT :
 *
 *      All tasks below are tasks of type 0, and must be executed at the order they were received.
 *
 *      We will implement security mechanisms described in the Scheduler help file.
 *
 *
 *      The macro below will be used to verify that [tasks_nb] tasks of type 0 can be executed, and return "reprogram"
 *          if not.
 */

#define FAIL_IF_CANT_SCHEDULE(nb)\
    {\
        /*If the task is not executable :*/\
        if (!TaskScheduler::verify_schedulability(nb)) {\
            /*Lock the sequence 0*/\
            /*TDOO LOCK_SEQUENCE(0)\*/\
            /*Schedule a re-comput*/\
            return reprogram;\
        }\
    }\



/*
 * home : this function moves all axis to zero. It takes the following parameters :
 *
 *  -s : homes the machine using endstops.
 */

task_state_t TerminalPipe::home() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //Declare the structure;
    MachineController::home_state_t sequences_initialised;

    //If the home must use endstops
    if (CHECK_ARGUMENT('e')) {

        std_out("ENDSTOPS NOT SUPPORTED FOR INSTANCE");

    } else {
        //If the movement must just be a line to zero

        //Reset the flag;
        sequences_initialised.endstops_flag = false;

        //Schedule a reset of the carriages
        MachineController::home_scheduled_0(sequences_initialised);

    }

    return complete;

}


/*
 * line : this function draws a line to the provided position.
 *  It takes the following data  :
 *
 *      -{x, y, z, e} : coordinates of the current carriage. Almost one is required.
 *      - r : (optionnal) all provided coordinates are relative.
 */

task_state_t TerminalPipe::line() {

    std_out("SUUS");

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    std_out("SUUS2");

    //verify that almost one movement coordinate is provided.
    REQUIRE_ONE_ARGUMENTS("xyze");

    MachineController::movement_state_t sequences_initialised = MachineController::movement_state_t();

    /*
     * To save repetitive lines of code, a macro that, if a coord is provided, sets its flag and its value.
     */
#define CHECK_COORDINATE(coord, coord_char)\
    if (CHECK_ARGUMENT(coord_char)) {\
        sequences_initialised.coord##_flag = true;\
        sequences_initialised.coord = GET_ARG_VALUE(coord_char);\
    }\

    //If a coordinate was provided, enable this coordinate, and get it.
    CHECK_COORDINATE(x, 'x');
    CHECK_COORDINATE(y, 'y');
    CHECK_COORDINATE(z, 'z');
    CHECK_COORDINATE(e, 'e');

    //For safety, un-define the previously created macro.
#undef CHECK_COORDINATE

    //If the movement is absolute, prepare_movement to the destination
    if (CHECK_ARGUMENT('r')) {

        //Mark the movement to be relative.
        sequences_initialised.relative_flag = true;

    }

    //Schedule a line to the specified coordinates
    return MachineController::line_scheduled_0(sequences_initialised);

}


//--------------------------------------------------------Extrusion-----------------------------------------------------

/*
 * enable_steppers : enable or disable machine_control.
 *
 *  It takes only one argument_t, -e followed by 0 (disable) or [not zero] enabled
 */

task_state_t TerminalPipe::enable_steppers() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //Fail if the enabling argument_t is omitted
    REQUIRE_ALL_ARGUMENTS("e")

    //Extract the enable boolean
    bool enable = (bool) GET_ARG_VALUE('e');

    //Schedule an enable / disable of machine_control.
    return MachineController::enable_steppers_scheduled_0(enable);

}


/*
 * set_position : updates the current position.
 *
 *  It takes almost one of the coordinates x, y, z and e.
 */

task_state_t TerminalPipe::set_position() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //verify that almost one movement coordinate is provided.
    REQUIRE_ONE_ARGUMENTS("xyze");

    MachineController::offsets_state_t sequences_initialised = MachineController::offsets_state_t();

    /*
     * To save repetitive lines of code, a macro that, if a coord is provided, sets its flag and its value.
     */
#define CHECK_COORDINATE(coord, coord_char)\
    if (CHECK_ARGUMENT(coord_char)) {\
        sequences_initialised.coord##_flag = true;\
        sequences_initialised.coord = GET_ARG_VALUE(coord_char);\
    }\

    //If a coordinate was provided, enable this coordinate, and get it.
    CHECK_COORDINATE(x, 'x');
    CHECK_COORDINATE(y, 'y');
    CHECK_COORDINATE(z, 'z');
    CHECK_COORDINATE(e, 'e');

    //For safety, un-define the previously created macro.
#undef CHECK_COORDINATE


    //Schedule a line to the specified coordinates
    return MachineController::set_current_position_scheduled_0(sequences_initialised);

}


/*
 * set_extrusion : this function modifies extrusion parameters.
 *
 *  It takes the folloging data :
 *      - c : changes the working carriage;
 *      - s : changes the speed for the carriages designed by c (or for the working carriage if c is not provided).
 *
 *  Almost one must be provided.
 */
task_state_t TerminalPipe::set_extrusion() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //At least w (working carriage) and s (speed) must be provided
    REQUIRE_ONE_ARGUMENTS("cs");

    MachineController::carriages_state_t new_state = MachineController::carriages_state_t();

    //If the working carriage must be changed
    if (CHECK_ARGUMENT('c')) {

        //Set the flag;
        new_state.working_carriage_flag = true;

        //Set the new working extruder;
        new_state.working_carriage = (uint8_t) GET_ARG_VALUE('w');

    }

    //If the speed must be changed;
    if (CHECK_ARGUMENT('s')) {

        //If we must set the speed for a particular carriage:
        if (CHECK_ARGUMENT('c')) {

            //Enable the speed modif for a carriage;
            new_state.nominative_speed_mod_flag = true;

            //Set the carriage;
            new_state.nominative_carriage = (uint8_t) GET_ARG_VALUE('c');

            //Set the speed;
            new_state.nominative_speed = GET_ARG_VALUE('s');


        } else {
            //If we must set the speed for the current carriage :

            //Set the flag;
            new_state.working_carriage_speed_flag = true;

            //Set the new working extruder;
            new_state.working_carriage_speed = GET_ARG_VALUE('s');
        }

    }

    //Modify the extrusion sequences_initialised.

    return MachineController::set_carriages_state_scheduled_0(new_state);;

}


/*
 * set_cooling : this function modifies the cooling sequences_initialised.
 *
 *  It takes the following data :
 *      -e : 0 means disable the cooling, other values will enable it.
 *      -p : modifies the cooling power (truncated between 0 and 100).
 */

task_state_t TerminalPipe::set_cooling() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //Fail if none of power of enable are provided
    REQUIRE_ONE_ARGUMENTS("ep");

    MachineController::cooling_state_t new_state = MachineController::cooling_state_t();

    //Set the enable sequences_initialised if required
    if (CHECK_ARGUMENT('e')) {

        //Set the flag
        new_state.enabled_flag = true;

        //get the enable boolean
        new_state.enabled = (bool) GET_ARG_VALUE('e');

    }

    //Set the power if required
    if (CHECK_ARGUMENT('p')) {

        //Set the flag
        new_state.enabled_flag = true;

        //Set the new power
        new_state.power = GET_ARG_VALUE('p');

    }

    return MachineController::set_cooling_state_scheduled_0(new_state);;

}


/*
 * set_hotend : this function sets the sequences_initialised of a particular hotend.
 *
 *  It takes the following data :
 *      -h : the hotend to modify, mandatory.
 *      -e : 0 means disable the power on the hotend, other values will enable it.
 *      -t : sets the targetVector temperature of the hotend.
 *
 *      -e or -t must be provided.
 */

task_state_t TerminalPipe::set_hotend() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //Fail if a hotend was not specified
    REQUIRE_ALL_ARGUMENTS("h")

    //Fail if neither temperature (t) or enable_state (e) are provided.
    REQUIRE_ONE_ARGUMENTS("te");
    TemperatureController::hotend_state_t sequences_initialised = TemperatureController::hotend_state_t();

    sequences_initialised.hotend_flag = true;
    sequences_initialised.hotend = (uint8_t) GET_ARG_VALUE('h');

    //If the temperature must be adjusted
    if (CHECK_ARGUMENT('t')) {

        //Set the temperature flag
        sequences_initialised.temperature_flag = true;

        //Get the temperature
        sequences_initialised.temperature = GET_ARG_VALUE('t');

    }

    //If the activity must be changed
    if (CHECK_ARGUMENT('e')) {

        //Set the enable flag
        sequences_initialised.enabled_flag = true;

        //Get the temperature
        sequences_initialised.enabled = (bool) GET_ARG_VALUE('e');

    }

    //Schedule an enable/disable of the hotbed regulation.
    return TemperatureController::set_hotends_state_scheduled_0(sequences_initialised);

}


/*
 * set_hotbed : this function sets the hotbed's sequences_initialised
 *
 *  It takes the following data :
 *      -e : 0 means disable the power on the hotbed, other values will enable it.
 *      -t : sets the targetVector temperature of the hotbed.
 *
 *      -e or -t must be provided.
 */

task_state_t TerminalPipe::set_hotbed() {

    //This command must schedule one type-0 task.
    FAIL_IF_CANT_SCHEDULE(1);

    //Fail if neither temperature (t) or enable_state (e) are provided.
    REQUIRE_ONE_ARGUMENTS("te");
    TemperatureController::hotbed_state_t sequences_initialised = TemperatureController::hotbed_state_t();


    //If the temperature must be adjusted
    if (CHECK_ARGUMENT('t')) {

        //Set the temperature flag
        sequences_initialised.temperature_flag = true;

        //Get the temperature
        sequences_initialised.temperature = GET_ARG_VALUE('t');

    }

    //If the activity must be changed
    if (CHECK_ARGUMENT('e')) {

        //Set the enable flag
        sequences_initialised.enabled_flag = true;

        //Get the temperature
        sequences_initialised.enabled = (bool) GET_ARG_VALUE('e');

    }

    //Schedule an enable/disable of the hotbed regulation.
    return TemperatureController::set_hotbed_state_scheduled_0(sequences_initialised);

}




//---------------------------------------------------------Gets---------------------------------------------------------

task_state_t TerminalPipe::get_regulations() {

    return complete;

}


task_state_t TerminalPipe::get_temps() {

    return complete;

}


//---------------------------------------------------------Tests--------------------------------------------------------


task_state_t TerminalPipe::test_steppers() {

    std_out("EXIT");

    return complete;

}


task_state_t TerminalPipe::test_temp() {


    std_out("t0 : " + string(Thermistors::get_temperature_hotend_0(845), 5));
    std_out("t0 : " + string(Thermistors::get_temperature_hotend_0(846), 5));
    std_out("t0 : " + string(Thermistors::get_temperature_hotend_0(847), 5));
    std_out("t0 : " + string(Thermistors::get_temperature_hotend_0(846), 5));
    std_out("t0 : " + string(Thermistors::get_temperature_hotend_0(845), 5));


    return complete;
}


task_state_t TerminalPipe::test_mpu() {

    MPU6050 *mpu = new MPU6050();

    mpu->initialise_hardware();

    mpu->initialise_data();

    /*
    Matrix *process_noise = new Matrix(3, 3);
    Matrix *measure_noise = new Matrix(3, 3);

    Matrix::setIdentityMatrix(process_noise);
    process_noise->divideBy(100);

    Matrix::setIdentityMatrix(measure_noise);
    measure_noise->divideBy(100);

    std_out(process_noise->toString());

    std_out(measure_noise->toString());


    float initial[3] {0, 0, 0};

    Matrix *initialP = new Matrix(3, 3);
    Matrix::setIdentityMatrix(initialP);

    initialP->divideBy(10);

     */

    uint32_t period_ms = 10;

    Matrix process_noise(3, 3, false);
    Matrix measure_noise(2, 2, false);
    process_noise.setToIdentity();
    measure_noise.setToIdentity();

    process_noise.divideBy(2);
    measure_noise.divideBy(2);

    process_noise.setCoefficient(0, 0, 0.5);
    process_noise.setCoefficient(1, 1, 0.01);
    process_noise.setCoefficient(2, 2, 0.5);

    //Gyro speed is not reliable;
    measure_noise.setCoefficient(0, 0, 0.01);
    //Acc measure is reliable
    measure_noise.setCoefficient(1,1, 0.1);

    IMUKalmanFIlter filter((float) 0.01, process_noise, measure_noise);

    Vector3D accelero = Vector3D(0, 0, -1);
    Triplet gyro(0, 0, 0);

    uint32_t time = millis() + period_ms;

    //TODOsleep(1000);


    while (true) {

        mpu->compute_data();

        mpu->get_accelerometer_data(accelero);

        mpu->get_gyrometer_data(gyro);

        //TODO sleep(10);

        //std_out("ax : " + string(accelero[0]) + " ay " + string(accelero[1]) + " az " + string(accelero[2]));

        //std_out("gx : " + string(gyro[0]) + " gy " + string(gyro[1]) + " gz " + string(gyro[2]));



        filter.update(accelero, gyro);

        filter.getGravity(accelero);

        filter.getGyroSpeeds(gyro);


        std_out("\n");
        std_out(st("Acc ") + accelero.toString());
        std_out("Gyr " + gyro.toString());


        while (millis() < time);

        time += period_ms;

    }

    return complete;

}


task_state_t TerminalPipe::test_kalman() {


    Matrix process_noise(3, 3, false);
    Matrix measure_noise(2, 2, false);

    process_noise.setToIdentity();
    process_noise.divideBy(10000);

    measure_noise.setToIdentity();
    measure_noise.divideBy(1000);

    std_out(process_noise.toString());

    std_out(measure_noise.toString());


    float initial[3]{0.5, 0, 0};

    Matrix initialP(3, 3, false);
    initialP.setToIdentity();
    initialP.divideBy(10);

    AngleOrder1KalmanFilter filter;

    filter.initialise(0.01, process_noise, measure_noise);

    filter.setInitialState(initial, initialP);

    uint32_t period = 10;

    uint32_t time = millis() + period;

    MPU6050 *mpu = new MPU6050();

    mpu->initialise_hardware();

    mpu->initialise_data();

    Vector3D accelero;
    Triplet gyro;

    while (true) {


        mpu->compute_data();

        mpu->get_accelerometer_data(accelero);

        std_out("accelero : " + accelero.toString());


        mpu->get_gyrometer_data(gyro);

        Angles3D angles;

        //TODO PROBLEM IN CONVERSION
        Vector3DToAngles::convert(accelero, angles);
        std_out("Angles : " + angles.toString());

        filter.compute(angles.get(0), gyro.get(0));

        float angle, angular_speed, biais;
        filter.getState(angle, angular_speed, biais);

        //std_out("sequences_initialised : "+string(angle)+" "+string(angular_speed)+" "+string(biais)+"\n");

        std_out("waiting");
        while (millis() < time);

        time += period;

    }

}


task_state_t TerminalPipe::test_rotation() {

    Vector3D v0(-1, 0, 0);

    Orientation2DToRotation converter = Orientation2DToRotation();

    converter.setTaget(v0);

    v0.set(0, 1);

    rotation_data_t data = rotation_data_t();

    converter.compute(v0, data);

    std_out(string("Angle : ") + data.rotation_angle);

    std_out(" x : " + data.rotation_vector.toString());

    return complete;

}


task_state_t TerminalPipe::test_angles() {

    Vector3D v0(0, -23, 1);

    Angles3D angles;

    Vector3DToAngles::convert(v0, angles);

    AnglesToVector3D::convert(angles, v0);

    std_out("Vector " + v0.toString());

    return complete;

}


#endif

