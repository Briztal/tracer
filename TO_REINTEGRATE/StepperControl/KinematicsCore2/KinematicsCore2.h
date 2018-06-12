/*
  KinematicsCore2.h - Part of TRACER

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

/*
 * The KinematicsCore2 class. This class's methods are called by TrajectoryExecuter, is this version of the kernel have been
 *      chosen in your configuration file.
 *
 *
 *  This version is the most advanced, and offers the following features :
 *      - tracing of trajectories expressed in the high level coordinates. Any kind of trajectory is supported;
 *      - exact acceleration regulation in beginning and ending points, for each steppers;
 *      - real-time acceleration / deceleration regulation during movements;
 *      - exact jerk regulation for each steppers;
 *      - regulation_speed regulation by cartesian groups of the high level coordinates;
 *      - constant update of tools power, based on the current regulation_speed and their power density;
 *
 *
 *  The kernel class is in charge of :
 *      - the sub_movements pre-processing;
 *      - the sub_movements queue management
 *      - the regulation_speed and jerk regulation;
 *      - the position log;
 *
 */

#include <config.h>
#include <Config/stepper_control_config.h>

#if defined(ENABLE_STEPPER_CONTROL) && (CORE_VERSION == 2)


#ifndef TRACER_KERNEL2_H
#define TRACER_KERNEL2_H

#include <StepperControl/_kinematics_data.h>


class KinematicsCore2 {

    //------------------------------------------- Initialisation --------------------------------------------

public:

    static void initialise_data();


    //------------------------------------------------ start ------------------------------------------------

public :

    static void start_tracing_procedure();


    //-------------------------------------------Current high level position--------------------------------------------

private :

    //The current position in the high level system.
    static float *const current_hl_position;


    //---------------------------------------------Current movement update----------------------------------------------

public:

    static void initialise_kinetics_data(k2_movement_data *movement_data);

    static void load_pre_process_kinetics_data(k2_movement_data *movement_data);



    //----------------------------------------------jerk_environment update---------------------------------------------

public :

    static void compute_jerk_offsets(float speed, k2_movement_data *previous_movement);

    //Function to update the real time jerk data
    static void load_real_time_jerk_data(const k2_movement_data *movement_data);


    //In KinematicsCore2, there is no more information to load in real time than jerk.
    static inline void load_real_time_kinetics_data(const k2_movement_data *movement_data) {
        load_real_time_jerk_data(movement_data);
    }

    //--------------------------------------------sub_movements preparation---------------------------------------------

public:

    //The function to determine a new sub_movement
    static void initialise_sub_movement(k2_sub_movement_data *sub_movement_data);

    //The function to update the current variables according to the sub_movement data computed in the function above
    static void accept_sub_movement(k2_sub_movement_data *sub_movement);

    //The first sub_movement preparation, called at the beginning of the movement routine.
    static float compute_us_time_for_first_sub_movement(k2_sub_movement_data *sub_movement_data);

    //The sub_movement preparation function, called on interrupts.
    static float compute_us_time_for_sub_movement(k2_sub_movement_data *sub_movement_data);

    //The evloution factor update function;
    static inline void update_evolution_coefficient(float) {};
private:

    //The regulation_speed group for the current movement
    static uint8_t movement_speed_group;

    //the regulation regulation_speed for the next movement
    static float next_regulation_speed;



    //----------------------------------------------------positon log---------------------------------------------------

public:

    //The function to send the current position through the interface.
    static void send_position();



};

#endif

#endif //TRACER_KERNEL2_H
