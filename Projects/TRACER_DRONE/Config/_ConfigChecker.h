/*
  _ConfigChecker.h Part of TRACER

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
 * _ConfigChecker : this class handles the configuration checking for all enabled modules.
 *
 *  It has only one public method, check_config, that will call the config check functions for all sub_modules.
 *
 *  These functions are private;
 */


#ifndef TRACER_CONFIGCHECKER_H
#define TRACER_CONFIGCHECKER_H


#include <hardware_language_abstraction.h>

class _ConfigChecker {

public:

    //Call all unit config check;
    static bool check_config(String *message);


private:

    //Check the actions configuration file;
    static bool check_actions(String *message);

    //Check the control structure;
    static bool check_transmission(String *message);

    //Check the control loops config;
    static bool check_control_loops(String *message);

    //Check all enabled controllers;
    static bool check_controllers(String *message);

    //Check the sensors config;
    static bool check_sensors(String *message);

    //Check the stepper control module config;
    static bool check_stepper_control(String *message);

};


#endif //TRACER_CONFIGCHECKER_H