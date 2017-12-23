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

/*
 * Binary GPIO (on - off)
 *
 * For each actuator you want to control in on-off mode, put one line like behind and provide the four required parameter
 *
 * BINARY(i, name, power_pin, enable)
 *
 *  - i :           the index of the servo, used to control the GPIO in the code (passed in argument);
 *                      Indices start at 0 and are strictly consecutive;
 *  - name :        the name you choose to give to the servo;
 *  - power_pin :   the pin on which the pwm will be triggered;
 *  - enable :      the boolean value of an enabled state;
 */

#ifdef BINARY


#endif


/*
 * PWM GPIO : a power command, output in [|0, 255|]
 *
 * For each actuator you want to control in linear, put one line like behind and provide the three required parameter
 *
 * CONTINUOUS(i, name, power_pin, max_value)
 *
 *  - i :           the index of the PWM, used to control the PWM in the code.
 *                      Indices start at 0 and are strictly consecutive;
 *  - name :        the name you choose to give to the servo;
 *  - power_pin :   the pin on which the pwm will be triggered;
 *  - max_value :   the maximum of the control value (as pwm is a power relay,
 *                      the control value will be btw 0 and max_value
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



//------------------------------------------------ Servomotors ------------------------------------------------

//The number of different servos
#define NB_SERVOS 2

//The total period of a servo cycle;
#define SERVO_PERIOD_US 20000


/*
 * For each servo you want to control, put one line like behind and provide the three required parameter
 *
 * SERVO(i, name, data_pin, min_value, max_value)
 *
 *  - i :           the index of the servo, used to control the servo in the code (passed in argument);
 *                      Indices start at 0 and are strictly consecutive;
 *  - name :        the name you choose to give to the servo;
 *  - data_pin :    the pin that will be used to communicate with the servo;
 *  - min_value :   the lower bound of the control value
 *  - max_value :   the upper bound of the control value
 *  - min_period :  the lower bound of the control period, in us (typically 1500)
 *  - max_period :   he upper bound of the control period, in us (typically 2500)
 */

#ifdef SERVO

SERVO(0, servo, 0, 0, 1, 1500, 2500)
SERVO(1, servoo, 1, 0, 1, 1500, 2500)

#endif

