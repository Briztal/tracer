/*
  EEPROMInterface.cpp - Part of TRACER

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

#include "EEPROMInterface.h"
#include "EEPROMStorage.h"

#include <DataStructures/StringUtils.h>
#include <Control/Control.h>

#include <Config/actions_config.h>
#include <Config/control_loops_config.h>


#define ES EEPROMStorage

/*
 * Structure sub_canals :
 *      0 : PIDs
 *          indice :
 *              0 : kp
 *              1 : ki
 *              2 : kd
 *      1 : Loops
 *          indice :
 *              period_ms
 *      2 : Continuous Actuators
 *          indice
 *              maximum
 *      3 : Servo Actuators
 *          indice
 *              minimum
 *              maximum
 *      4 : steppers
 *          indice
 *              0 : size
 *              1 : steps
 *              2 : max_speeds
 *              3 : acceleration
 *      5 : speed_groups
 *          indice
 *              max_speed
 *      6 : custom_parameters;
 *          k1_position_indice : parameter k1_position_indice;
 */



#define PID_CAT (uint8_t) 0
#define LOOP_CAT (uint8_t) 1
#define CONTINUOUS_CAT (uint8_t) 2
#define SERVO_CAT (uint8_t) 3
#define STEPPER_CAT (uint8_t) 4
#define CARTESIAN_GROUP_CAT (uint8_t) 5
#define CUSTOM_CAT (uint8_t) 6


//###############################################PRIMITIVE_WRITE_METHODS################################################



void EEPROMInterface::print_stored_data() {

    //Stepper motors
#ifdef ENABLE_STEPPER_CONTROL
    std_out("Stepper Motors : \n");
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        stepper_data_t *data = ES::steppers_data + stepper;
        std_out("Stepper " + str(stepper));
        std_out("\tsteps : \t" + str(data->steps));
        std_out("\tending speed : \t" + str(data->maximum_speed));
        std_out("\tacceleration : \t" + str(data->acceleration));
        std_out("\tjerk : \t" + str(data->jerk)+"\n");
    }

    std_out("\n---------------------\n");

    std_out("Cartesian Groups : \n");
    for (uint8_t speed_group = 0; speed_group < NB_CARTESIAN_GROUPS; speed_group++) {
        std_out(
                "Group " + str(speed_group) + ", ending regulation speed : " + str(ES::group_maximum_speeds[speed_group]));
    }

#endif

    std_out("\n---------------------\n");

#ifdef ENABLE_CONTROL_LOOPS

    //PIDs
    std_out("PIDS : \n");
    for (int pid = 0; pid < NB_PIDS; pid++) {
        pid_data_t *data = ES::pids_data + pid;
        std_out("\tPID " + str(pid));
        std_out("\tkp : \t" + str(data->kp));
        std_out("\tki : \t" + str(data->ki));
        std_out("\tkd : \t" + str(data->kd)+"\n");
    }

    std_out("\n---------------------\n");

    //Control Loops
    std_out("Control Loops : \n");
    for (int loop = 0; loop < NB_LOOPS; loop++) {
        std_out("\tLoop " + str(loop) + ", period (ms) : \t" + str(ES::loop_periods[loop]));
    }

#endif

    std_out("\n---------------------\n");

    //Continuous Actuators
    std_out("Continuous Actuators : \n");
    for (int continuous = 0; continuous < NB_CONTINUOUS; continuous++) {
        continuous_data_t *data = ES::continuous_data + continuous;
        std_out("\tContinuous " + str(continuous) + " : maximum : \t" + str(data->max));
    }

    std_out("\n---------------------\n");

    //Servo Actuators
    std_out("Servo Actuators : \n");
    for (int servo = 0; servo < NB_SERVOS; servo++) {
        servo_data_t *data = ES::servos_data + servo;
        std_out("Loop " + str(servo));
        std_out("\tminimum : \t" + str(data->min));
        std_out("\tmaximum : \t" + str(data->max) + "\n");
    }

    std_out("\n---------------------\n");

    //Custom Data
    std_out("Custom Data : ");

    //Custom data
#define EEPROM_VARIABLE(name, default_value) std_out("\t"+str(#name)+" : "+str(ES::custom_data.name));

#include <Config/eeprom_config.h>

#undef EEPROM_VARIABLE

    std_out(" ");

}


/*
 * get_root_children_nb : this function calculates the number of nb_children of the root EEPROMTree.
 *
 *  It makes nothing but verifying that modules are enabled, and adding integers to the total amount if so.
 *
 */

uint8_t get_root_children_nb() {

    //Continuous actions;
    //Servo actions;
    //Custom data actions;
    uint8_t nb = 3;

#ifdef ENABLE_CONTROL_LOOPS
    //PIDs
    //Loops
    nb += 2;
#endif


#ifdef ENABLE_STEPPER_CONTROL
    //Stepper_data
    //Cartesian groups
    nb += 2;
#endif

    return nb;

}

void display(EEPROMTree *node, uint8_t t) {

    Serial.println("child : "+String(t)+" "+String(*(node->name)));

    if (node->nb_children == 0) {
        Serial.println("dick");
        return;

    }
    EEPROMTree** children = node->children;

    t++;
    //Check every sub_node
    for (uint8_t i = 0; i < node->nb_children; i++) {

        EEPROMTree * n = (*(children + i));

        display(n, t);

    }

}



EEPROMTree *EEPROMInterface::build_tree() {

    //First, create the root tree
    EEPROMTree *root = new EEPROMTree(new String("eeprom"), 0, nullptr, get_root_children_nb());

    EEPROMTree *tree;
    EEPROMTree *tree2;
    uint8_t root_id = 0;

    //-------------------Actuators-------------------

    //Continuous actions

    //Create the tree, and add it to root
    tree = new EEPROMTree(new String("continuous"), root_id, nullptr, NB_CONTINUOUS);
    root->children[root_id] = tree;

    //Add the sub nodes
#define CONTINUOUS(i, name, ...) tree->children[i] = new EEPROMTree(new String(#name), i, &((ES::continuous_data+i)->max), 0);

#include <Config/actions_config.h>

#undef CONTINUOUS

    //Increment the root id
    root_id++;

    //------------------

    //Servo actions

    //Create the tree, and add it to root
    tree = new EEPROMTree(new String("servo"), root_id, nullptr, NB_SERVOS);
    root->children[root_id] = tree;

    //Add the sub nodes
#define SERVO(i, name, ...) \
    tree->children[i] = tree2 = new EEPROMTree(new String(#name), i, nullptr, 2);\
    tree2->children[0] = new EEPROMTree(new String("min"), 0, &((ES::servos_data+i)->min), 0);\
    tree2->children[1] = new EEPROMTree(new String("max"), 1, &((ES::servos_data+i)->max), 0);

#include <Config/actions_config.h>

#undef SERVO

    //Increment the root id
    root_id++;


    //-------------------Asserv-------------------

#ifdef ENABLE_CONTROL_LOOPS

    //PIDs
    //Create the tree, and add it to root
    tree = new EEPROMTree(new String("pid"), root_id, nullptr, NB_PIDS);
    root->children[root_id] = tree;

    //Add the sub nodes
#define PID(i, name, ...) \
    tree->children[i] = tree2 = new EEPROMTree(new String(#name), i, nullptr, 3);\
    tree2->children[0] = new EEPROMTree(new String("kp"), 0, &((ES::pids_data+i)->kp), 0);\
    tree2->children[1] = new EEPROMTree(new String("ki"), 1, &((ES::pids_data+i)->ki), 0);\
    tree2->children[2] = new EEPROMTree(new String("kd"), 2, &((ES::pids_data+i)->kd), 0);

#include <Config/control_loops_config.h>

#undef PID

    //Increment the root id
    root_id++;


    //------------------

    //Loops
    //Create the tree, and add it to root
    tree = new EEPROMTree(new String("loop"), root_id, nullptr, NB_LOOPS);
    root->children[root_id] = tree;

    //Add the sub nodes
#define LOOP_FUNCTION(i, name, ...)\
    tree->children[i] = new EEPROMTree(new String(#name), i, (ES::loop_periods+i), 0);

#include <Config/control_loops_config.h>

#undef LOOP_FUNCTION

    //Increment the root id
    root_id++;

#endif


    //-------------------Stepper control-------------------

#ifdef ENABLE_STEPPER_CONTROL

    //Stepper data
    //Create the tree, and add it to root
    tree = new EEPROMTree(new String("stepper"), root_id, nullptr, NB_STEPPERS);
    root->children[root_id] = tree;

    //Add the sub nodes
#define STEPPER(i, ...) \
    tree->children[i] = tree2 = new EEPROMTree(new String(#i), i ,nullptr, 5);\
    tree2->children[1] = new EEPROMTree(new String("steps"),    1, &((ES::steppers_data+i)->steps), 0);\
    tree2->children[2] = new EEPROMTree(new String("speed"),    2, &((ES::steppers_data+i)->maximum_speed), 0);\
    tree2->children[3] = new EEPROMTree(new String("acc"),      3, &((ES::steppers_data+i)->acceleration), 0);\
    tree2->children[4] = new EEPROMTree(new String("jerk"),     4, &((ES::steppers_data+i)->jerk), 0);

#include <Config/stepper_control_config.h>

#undef STEPPER

    //Increment the root id
    root_id++;


    //Cartesian groups
    //Create the tree, and add it to root
    tree = new EEPROMTree(new String("groups"), root_id, nullptr, NB_CARTESIAN_GROUPS);
    root->children[root_id] = tree;

    //Add the sub nodes
#define CARTESIAN_GROUP(i, name, ...) tree->children[i] = new EEPROMTree(new String(#name), i, (ES::group_maximum_speeds+i), 0);

#include <Config/stepper_control_config.h>

#undef CARTESIAN_GROUP

    //Increment the root id
    root_id++;

#endif

    //-------------------Custom parameters-------------------


    tree = new EEPROMTree(new String("custom"), root_id, nullptr, sizeof(custom_data_t) / sizeof(float));
    root->children[root_id] = tree;

    uint8_t custom_index = 0;

#define EEPROM_VARIABLE(name, default_val) tree->children[custom_index] = new EEPROMTree(new String(#name), custom_index, (&ES::custom_data.name), 0);

#include <Config/eeprom_config.h>

#undef EEPROM_VARIABLE


    return root;
}

bool EEPROMInterface::search_tree_by_string(char *data_in, float **data) {

    //Initialise the current current_tree to the root;
    EEPROMTree *current_tree = eeprom_tree;
    EEPROMTree *current_sub_tree;

    EEPROMTree **children = current_tree->children;

    //char name_buffer[EEPROM_NAMES_MAX_LENGTH];

    uint8_t i;

    char *word;

    node_check:

    //remove extra spaces
    data_in += StringUtils::lstrip(data_in, ' ');

    //Get the word address;
    word = data_in;

    //Go to the next word;
    data_in += StringUtils::count_until_char(data_in, ' ');

    //Increment the next char if the string is not finished;
    if (*data_in) {
        *(data_in++) = 0;
    }

    //Check every sub_node
    for (i = 0; i < current_tree->nb_children; i++) {
        current_sub_tree = children[i];

        const char *c = (*current_sub_tree->name).c_str();

        //If the current word matches the current_tree's name
        if (!strcmp(c, word)) {

            //Re-initialise_hardware the current data
            current_tree = current_sub_tree;
            children = current_tree->children;

            //if the new node is not a leaf, check sub nodes
            if (current_tree->nb_children) {

                //check the new node
                goto node_check;

            } else {
                //If the sub_node is a leaf:

                //Update the data pointer
                *data = current_tree->data;

                //Return;
                return true;

            }

        }

    }

    search_log(current_tree);

    return false;

}

void EEPROMInterface::search_log(EEPROMTree *node) {

    String s = "Suggestions for " + *node->name + " : ";

    for (uint8_t i = 0; i < node->nb_children; i++) {
        s += *node->children[i]->name + ", ";
    }

    std_out(s);

}


bool EEPROMInterface::read_data_by_string(char *id_string, float *f) {

    float *p;
    if (search_tree_by_string(id_string, &p)) {
        *f = *p;
        return true;
    } else {
        return false;
    }

}


void EEPROMInterface::write_data_by_string(char *id_string, float value) {

    //TODO MEMORY LEAK ? FAIRE UNE FONCTION RECURSIVE D'AFFICHAGE D'ARBRE, POUR VOIR SI LE PB VIENT DE LA CONSTRUCTION OU D'UNE FUITE MEMOIRE.
    float *p;

    if (search_tree_by_string(id_string, &p)) {
        *p = value;
    }

}



#ifdef ENABLE_PROGRAM_INTERFACE

void EEPROMInterface::send_structure() {
    /*
     * Syntax :
     * reset_tree (beginning) :         0;
     * confirm_tree (end) :             1;
     * go_upper :                       2;
     * go_lower(k1_position_indice, name) :              3, k1_position_indice, name
     * create_leaf(k1_position_indice, name) :           4, k1_position_indice, name
     */

    TI::prepare_EEPROM_packet();\
    TI::add_char_out(0);\
    TI::add_char_out(0);\
    TI::send_packet();

#define EEPROM_UPPER\
    TI::prepare_EEPROM_packet();\
    TI::add_char_out(0);\
    TI::add_char_out(2);\
    TI::send_packet();

#define EEPROM_LOWER(i, name)\
    TI::prepare_EEPROM_packet();\
    TI::add_char_out(0);\
    TI::add_char_out(3);\
    TI::add_char_out(i);\
    TI::add_string_out(#name);\
    TI::send_packet();

#define EEPROM_LEAF(i, name)\
    TI::prepare_EEPROM_packet();\
    TI::add_char_out(0);\
    TI::add_char_out(4);\
    TI::add_char_out(i);\
    TI::add_string_out(#name);\
    TI::send_packet();


#ifdef ENABLE_CONTROL_LOOPS

    //---------------------------------------------PIDs---------------------------------------------

    EEPROM_LOWER(PID_CAT, pids);

    //For each PID, mark a case for kp, ki, and kd.
#define PID(i, ...)\
    EEPROM_LOWER(i, i);EEPROM_LEAF(0, kp);EEPROM_LEAF(1, ki);EEPROM_LEAF(2, kd);EEPROM_UPPER;

#include "../config.h"

#undef PID

    EEPROM_UPPER;

    //---------------------------------------------Control loops---------------------------------------------

    EEPROM_LOWER(LOOP_CAT, loops);
    //For each Control loop, mark a case for the period.
#define LOOP_FUNCTION(indice, ...) EEPROM_LOWER(indice, indice);EEPROM_LEAF(0, period);EEPROM_UPPER;

#include "../config.h"

#undef LOOP_FUNCTION

    EEPROM_UPPER;

#endif

    //---------------------------------------------Continuous Actuators---------------------------------------------

    EEPROM_LOWER(CONTINUOUS_CAT, binaries);
    //For each Continuous Action, mark a case for the maximum value.
#define CONTINUOUS(indice, ...) EEPROM_LOWER(indice, indice);EEPROM_LEAF(0, max_value);EEPROM_UPPER;

#include "../config.h"

#undef CONTINUOUS


    EEPROM_UPPER;

    //---------------------------------------------Servo Actuators---------------------------------------------
    EEPROM_LOWER(SERVO_CAT, continuous);
    //For each Servo Action, mark a case for the minimum and maximum values.
#define SERVO(indice, ...) EEPROM_LOWER(indice, indice);EEPROM_LEAF(0, min_value);EEPROM_LEAF(1, max_value);EEPROM_UPPER;

#include "../config.h"

#undef SERVO

    EEPROM_UPPER;


#ifdef ENABLE_STEPPER_CONTROL

    //---------------------------------------------Steppers---------------------------------------------

    EEPROM_LOWER(STEPPER_CAT, steppers);
    //For each Stepper, mark a case for the size, the number of steps per unit, and maximum regulation_speed and acceleration.
#define STEPPER_DATA(indice, ...) EEPROM_LOWER(indice, indice);\
    EEPROM_LEAF(0, size);EEPROM_LEAF(1, steps);EEPROM_LEAF(2, max_speed);EEPROM_LEAF(3, max_acceleration);EEPROM_LEAF(3, max_jerk);\
    EEPROM_UPPER;

#include "../config.h"

#undef STEPPER_DATA

    EEPROM_UPPER;

    //---------------------------------------------Cartesian Groups---------------------------------------------

    EEPROM_LOWER(CARTESIAN_GROUP_CAT, speed_groups);
    //For each group, mark a case for its maximum regulation_speed
#define CARTESIAN_GROUP(indice, ...) EEPROM_LOWER(indice, indice);\
    EEPROM_LEAF(0, max_speed);EEPROM_UPPER;

#include "../config.h"

#undef CARTESIAN_GROUP

    EEPROM_UPPER;

#endif

    //---------------------------------------------Custom parameters---------------------------------------------
    EEPROM_LOWER(CUSTOM_CAT, custom);

    int custom_indice = 0;
    //Mark a case for each custom parameter
#define EEPROM_FLOAT(name, v) EEPROM_LEAF(custom_indice, name);custom_indice++;
#define EEPROM_BOOL(n, v) EEPROM_FLOAT(n,v)
#define EEPROM_CHAR(n, v) EEPROM_FLOAT(n,v)
#define EEPROM_INT(n, v) EEPROM_FLOAT(n,v)
#define EEPROM_LONG(n, v) EEPROM_FLOAT(n,v)

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

    EEPROM_UPPER;

#undef EEPROM_UPPER
#undef EEPROM_LOWER
#undef EEPROM_LEAF

    //Confirm and fetch the transmitted tree
    TI::prepare_EEPROM_packet();\
    TI::add_char_out(0);\
    TI::add_char_out(1);\
    TI::send_packet();

}

#endif

EEPROMTree *EEPROMInterface::eeprom_tree = build_tree();