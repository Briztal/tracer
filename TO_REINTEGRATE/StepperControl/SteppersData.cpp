/*
  SteppersData.cpp - Part of TRACER

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

#include <Kernel/Storage/EEPROMMap.h>
#include <Kernel/Interaction/Interaction.h>
#include "SteppersData.h"


/*
 * reset : this function resets data, and memory it in the EEPROMMap;
 */
void SteppersData::initialise_data() {

    //Reset data to its default values;
    reset_data();

    //Register data in the EEPROMMap;
    EEPROM_registration();


}


/*
 * EEPROM_registration : this function register steppers-related data in the EEPROMMap;
 */

void SteppersData::EEPROM_registration() {

    //Cache the current tree;
    EEPROMTree *parent_tree = new EEPROMTree(string("steppers"), EEPROMData(), 0);

    //cache for the stepper data;
    stepper_data_t *stepper_data;

    //Cache for the stepper tree
    EEPROMTree *stepper_tree;

    //Register one stepper data;
#define STEPPER_DATA(i, m_steps_per_unit, m_speed, m_acceleration, m_jerk)\
    /*Create the tree for the current stepper.*/\
    stepper_tree  = new EEPROMTree(string(#i), EEPROMData(), 4);\
    /*Add the stepper tree to the parent tree;.*/\
    parent_tree->addChild(stepper_tree);\
    /*Cache the stepper data pointer.*/\
    stepper_data = steppers_data + (i);\
    /*Add sub_trees for steps per unit, speed, acceleration and jerk.*/\
    stepper_tree->addChild(new EEPROMTree(string("steps"), EEPROMData(&stepper_data->steps_per_unit, m_steps_per_unit), 0));\
    stepper_tree->addChild(new EEPROMTree(string("speed"), EEPROMData(&stepper_data->maximum_speed, m_speed), 0));\
    stepper_tree->addChild(new EEPROMTree(string("acceleration"), EEPROMData(&stepper_data->acceleration, m_acceleration), 0));\
    stepper_tree->addChild(new EEPROMTree(string("jerk"), EEPROMData(&stepper_data->jerk, m_jerk), 0));\


    //Add all steppers trees;
#include <Config/stepper_control_config.h>

    //Undef the macro for safety;
#undef STEPPER_DATA

    //Null string;
    char c = 0;

    //Add the steppers tree to the EEPROMMap;
    EEPROMMap::add_branch(&c, parent_tree);

}


/*
 * reset_data : this function resets steppers data to its default value;
 */

void SteppersData::reset_data() {

    stepper_data_t *step_p;

    //Initialise one stepper data;
#define STEPPER_DATA(i, d_steps, d_speed, d_acc, d_jerk)\
    step_p = steppers_data+(i);\
    step_p->steps_per_unit = d_steps;\
    step_p->maximum_speed = d_speed;\
    step_p->acceleration = d_acc;\
    step_p->jerk = d_jerk;

    //Initialise all steppers;
#include <Config/stepper_control_config.h>

    //Undef the macro for safety;
#undef STEPPER_DATA

}

stepper_data_t t_std_t[NB_STEPPERS];
stepper_data_t *const SteppersData::steppers_data = t_std_t;
