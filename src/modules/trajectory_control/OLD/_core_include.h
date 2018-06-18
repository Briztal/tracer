/*
  _core_include.h - Part of TRACER

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
 * This file eases the corect inclusion of KinematicsCore files, as the version is specified at compile-time;
 *
 * It will include all appropriate files related to kinematics, and define movement and sub_movement data types;
 */

#include <config.h>

#include "_kinematics_data.h"

//If the core version is provided
#ifdef CORE_VERSION

//Version for core zero
#if (CORE_VERSION == 0)

#include <LinearMovement.h>


//Version for core one;
#elif (CORE_VERSION == 1)

//Include the core 1 class;
#include <StepperControl/KinematicsCore1/KinematicsCore1.h>

//Define the core 1 movement data;
#define movement_data_t k1_movement_data

//Define the core 1 sub_movement data;
#define sub_movement_data_t k1_sub_movement_data

//Define the Kinematics alias
#define Kinematics KinematicsCore1


//Version for core two;
#elif (CORE_VERSION == 2)

//Include the core 2 class;
#include <StepperControl/KinematicsCore2/KinematicsCore2.h>

//Define the core 2 movement data;
#define movement_data_t k2_movement_data

//Define the core 2 sub_movement data;
#define sub_movement_data_t k2_sub_movement_data

//Define the Kinematics alias
#define Kinematics KinematicsCore2


#endif

#endif
