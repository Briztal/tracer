/*
  control_loops_config.h - Part of TRACER

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
 * This section sets up the asserv code structure.
 * The structure is composed of external parameters, representing the information you possess about your environment,
 * and PIDs, computing data computed from external parameters. GENERATE_PID Outputs can be computed, to call actions.
 */


/*
 * StaticPID : simple StaticPID utilities;
 */

//Set the number of PIDs
#define NB_PIDS 5

/*
 * For each StaticPID, put one line like behind and provide the three required parameter
 *
 * StaticPID(i, name, kp, ki, kd)
 *
 *  - i :   the index of the StaticPID, used to control it in the code;
 *              Indices start at 0 and are strictly consecutive;
 *  - name : the name you choose to give to the StaticPID;
 *  - kp :  the default kp value;
 *  - ki :  the default ki value;
 *  - kd :  the default kd value;
 */

#ifdef GENERATE_PID

GENERATE_PID(0, hotend_0, 20, 0, 0)
GENERATE_PID(1, hotend_1, 20, 0, 0)
GENERATE_PID(2, hotend_2, 20, 0, 0)
GENERATE_PID(3, hotend_3, 20, 0, 0)
GENERATE_PID(4, hotbed, 20, 0, 0)

#endif


/*
 * Loop Functions : interrupt called functions
 */

//Set the number of loops;
#define NB_LOOPS 1

/*
 *
 * For each loop function, put one line like behind and provide the two required parameter
 *
 * LOOP_FUNCTION(i, name, period_ms)
 *
 *
 *  - i :           the index of the StaticPID, used to control it in the code;
 *                      Indices start at 0 and are strictly consecutive;
 *  - name :        the name you choose to give to the StaticPID;
 *  - period_ms :   the period of the loop function;
 */

#ifdef LOOP_FUNCTION
//Use LOOP_FUNCTION here

LOOP_FUNCTION(0, temperature, 10)

#endif

