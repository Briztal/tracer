/*
  Machine.cpp - Part of TRACER

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
 * The MachineController class is one of the two state-controllers of the project.
 *
 *  This class handles the mechanics state of the machine (movement, cooling, extrusion, position, ...).
 *      The Temperature control is handled by the named class.
 *
 *
 *  As this class can configure a lot of parameters, and trigger a lot of machine commands, it is built on
 *    the state manager model :
 *      - All its public functions (called state modifiers) take state structs in arguments.
 *      - A state struct is a structure where for all data field, there is a flag (bool) that defined if the
 *          data field's value is intentionnaly set.
 *      - When a state modifier is called, it reads all flags, and only proceeds data with their field set.
 *
 *  This model is useful, because it allows the programmer (you !!) to call a single function to modify several
 *      related parameters, for example, modify the speed of the working carriage and the speed
 *      of a particular carriage (named by its id);
 *
 *  The other solution would have been to create a single function for every single command executable, and a modifier
 *      for every single parameter, for example, we would have called a function to modify the speed of the working
 *      carriage's speed, and another to modify the speed of a carriage by its id).
 *
 *  This solution is viable too, but as functions are scheduled, and we often want to call two related actions at the
 *      same time (for example, set the current speed AND move to a position with that speed), we would have often
 *      called functions simultaneously, what would have occupied more nb_spaces in the scheduler.
 *
 */

#ifndef TRACER_COORDINATESINTERFACE_H
#define TRACER_COORDINATESINTERFACE_H


#include <cstdint>
#include <TaskScheduler/task_state_t.h>
#include <TaskScheduler/TaskScheduler.h>

class MachineController {

private:

    /*
     * State structures, and inheritance schema :
     *
     *  Below are defined state structures, that will be used to modify the machine's state.
     *
     *  They follow an inheritance schema, which is described.
     */

    //Below are defined two structs that are extended frequently in the code.

    //Basic 4 coordinates type
    struct hl_coord_t {

        float x = 0;
        float y = 0;
        float z = 0;
        float e = 0;
    };

    //A state structure derived of coordinate struct
    struct hl_coord_flags_t {

        //Add flags
        bool x_flag = false;
        bool y_flag = false;
        bool z_flag = false;
        bool e_flag = false;

    };

    //A state structure derived of coordinate struct
    struct hl_coord_state_t : hl_coord_t, hl_coord_flags_t {};


public :

    /*
     * The homing state structure. It contains a flag for all steppers
     */

    struct home_state_t : hl_coord_flags_t{

        //It only adds a flag for endstops usage;
        bool endstops_flag = false;
    };


public :

    /*
     * The carriages state structure :
     *
     *  This struct will will be used to modify the working carriage, and its speed, and also to
     *      mofify a designated carriage's speed.
     *
     *  It will contain:
     *
     *  flags and data for:
     *      - The working extruder;
     *      - The extruder speeds;
     */

    struct carriages_state_t {

        //Flag for the working extruder modification.
        bool working_carriage_flag = false;

        //Flag for the working extruder's speed modification.
        bool working_carriage_speed_flag = false;

        //Flag to enable the modification one carriage's speed.
        bool nominative_speed_mod_flag = false;


        //Data
        uint8_t working_carriage = 0;
        float working_carriage_speed = 0;

        //Data to modify one extruder's speed.
        uint8_t nominative_carriage = 0;
        float nominative_speed = 0;

    };


public:

    /*
     * The movement state structure :
     *
     *  This structure is the union of structs coord_state_t (movement coordinates) and carriages_state_t.
     *
     *  The reason for this, is that when planning a movement, we also may want to set its speed, the carriage that
     *      will move, etc...
     *
     *  The only data added, a flag determining whether the movement is relative or not.
     */

    struct movement_state_t : hl_coord_state_t, carriages_state_t {

        //The movement type flag.
        bool relative_flag = false;

    };


public:

    /*
     * The offset state structure :
     *
     *  This structure will be used to modify the offsets of the high level (4 axis) coordinate system.
     *
     *  It is a simple coordinate state structure.
     */

    struct offsets_state_t : hl_coord_state_t {
    };


public:

    /*
     * The cooling state structure :
     *
     *  This struct will be used to modify the cooling power, and to enable / disable it.
     *
     *  It will contain flags and data for:
     *      - enable state (is the cooling enabled)
     *      - the power of the cooling
     */

    struct cooling_state_t {
        //Data flags : set if the data is relevant
        bool enabled_flag = false;
        bool power_flag = false;

        //Data
        bool enabled = false;
        float power = 0;

    };


public:

    /*
     * The coordinate interface state structure :
     *
     *  This struct will be used to modify coordinate interface variables.
     *
     *  It will contain flags and data for every piece of data declared in your configuration file;
     */

    struct coord_interface_state_t {

#define COORD_INTERFACE_VARIABLE(name,...) float name = 0;bool name##_flag = false;

#include <config.h>

#undef COORD_INTERFACE_VARIABLE

    };




    /*
     * -------------------------------------------------------------------------------------------------
     * -------------------------------------------------------------------------------------------------
     * -------------------------------------------------------------------------------------------------
     *
     * State modifiers : functions below are in charge of modifying the machine's state, according to
     *  state structures they receive in arguments.
     *
     * -------------------------------------------------------------------------------------------------
     * -------------------------------------------------------------------------------------------------
     * -------------------------------------------------------------------------------------------------
     */

    //-------------------------------Homing movement-------------------------------

public:

    static task_state_t home(home_state_t state);

    //A scheduler for the cooling homing function
GENERATE_SCHEDULER(home, 0, home_state_t, state);

private:

    struct carriage_data {
        uint8_t carriage_id;
        float time;
    };


    //Set the reference carriage, in the homing preparation
    static void set_reference_carriage(uint8_t id, float x, float y, carriage_data *data);

    //Check if a carriage do not exceeds its limit, during the homing preparation.
    static void check_carriage(uint8_t id, float x, float y, carriage_data *data);


    //-------------------------------Line movement-------------------------------

public:

    static task_state_t line(movement_state_t movement_state);

    //A scheduler for the line function.
GENERATE_SCHEDULER(line, 0, movement_state_t, coords);


private:

    //The current absolute high level machine coordinates : [x, y, z, e].
    static hl_coord_t current_position;

    //The current axis coordinates.
    static float *const axis_positions;

    //Replace coordinates with flags reset with current ones.
    static void replace_coords(movement_state_t *coords);

    //Save control coords
    static void persist_coords(movement_state_t *coords);

    //movement in mode zero, all carriages accessible.
    static task_state_t move_mode_0(movement_state_t *coords);

    //Sanity check, verify that carriage do not collide, or don't go in negative positions.
    static void sanity_check(float *hl_coords);


    //-------------------------------Offsets-------------------------------

public:

    //The offset modifier
    static task_state_t set_current_position(offsets_state_t);

    //A scheduler for the offset modifier
GENERATE_SCHEDULER(set_current_position, 0, offsets_state_t, offsets);


private:

    //The current absolute high level offsets : [x, y, z, e].
    static hl_coord_t offsets;

    //-------------------------------Steppers-------------------------------

public:

    //Enable or disable all steppers
    static task_state_t enable_steppers(bool enable);

    //A scheduler for the stepper manager
GENERATE_SCHEDULER(enable_steppers, 0, bool, enable);


    //-------------------------------Carriages-------------------------------

public:

    /*
     * The carriages state modifier : this function can modify the state of the extrusion, and can be scheduled.
     *
     *  It receives a state, vith various flags enabled, and makes the appropriate modifications,
     *      calling simple set functions :
     *
     *      - set_speed;
     *      - set_working_carriage.
     */

    static task_state_t set_carriages_state(carriages_state_t data);

    //The Scheduler for the function.
GENERATE_SCHEDULER(set_carriages_state, 0, carriages_state_t, data);

    //Get the current extrusion state;
    static const carriages_state_t get_extrusion_state();

    //The current mode.
    static uint8_t mode;


private :

    //The current extrusion state;
    static carriages_state_t extrusion_state;

    //Mofify the current working extruder.
    static task_state_t set_working_extruder(uint8_t carriage);


    //-------------------------------Cooling-------------------------------

public:

    //Set and get hotbed temperatures
    static task_state_t set_cooling_state(cooling_state_t);

    //A scheduler for the cooling manager
GENERATE_SCHEDULER(set_cooling_state, 0, cooling_state_t, state);

    static const cooling_state_t get_cooling_state();


private:

    //The cooling state, that contains all current cooling parameters.
    static cooling_state_t cooling_state;


    //-------------------------------Cooling-------------------------------

public:

    //This function does not store its data. It is stored in the EEPROM.

    //Set and get hotbed temperatures
    static task_state_t set_coord_interface_state(coord_interface_state_t);

    //A scheduler for the cooling manager
GENERATE_SCHEDULER(set_coord_interface_state, 0, coord_interface_state_t, state);

    static const coord_interface_state_t get_coord_interface_state();



};


#endif //TRACER_COORDINATESINTERFACE_H
