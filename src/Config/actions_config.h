/*
  actions_config.h - Part of TRACER

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

/* Binary Actuators (on - off)
 * For each actuator you want to control in on-off mode, put one line like behind and provide the four required parameter
 * BINARY(i, name, powerPin, enableValue)
 */

#ifdef BINARY


#endif


/* Continuous Actuators : a power command, output in [|0, 255|]
 * For each actuator you want to control in linear, put one line like behind and provide the three required parameter
 * CONTINUOUS(i, name, powerPin, maxValue)
 */

#define NB_CONTINUOUS 6

#ifdef CONTINUOUS

CONTINUOUS(0,   hotend_0,   2,      100)
CONTINUOUS(1,   hotend_1,   3,      100)
CONTINUOUS(2,   hotend_2,   4,      100)
CONTINUOUS(3,   hotend_3,   5,      100)
CONTINUOUS(4,   hotbed,     6,      100)
CONTINUOUS(5,   cooling,    23,     100)

#endif

/* Servomotor Actuators
 * For each servo you want to control, put one line like behind and provide the three required parameter
 * SERVO(i, name, dataPin, minValue, maxValue)
 */

#define NB_SERVOS 1

#define SERVO_PERIOD_US 20

#ifdef SERVO

SERVO(0, servo, 4, 0, 1, 1.5, 2.5)

#endif

