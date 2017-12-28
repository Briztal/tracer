/*
  sensors_config.h - Part of TRACER

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
 * Thermistors :
 * For each thermistor you may want to include, add a line like the following :
 *
 * THERMISTOR(index, name, data_pin, thermistor_type)
 *
 *  - index : the thermistor index. Indices are strictly consecutive, starting by 0;
 *  - name : the name your thermistor will be labeled with;
 *  - data_pin : the pin used to acquire analog values;
 *  - thermistor_type : the type of your thermistor. It must match one of types defined in thermistor_data.h.
 */

#ifdef THERMISTOR

THERMISTOR(0, hotend_0, A26,    SEMITEC_100_47)
THERMISTOR(1, hotend_1, 22,     SEMITEC_100_47)
THERMISTOR(2, hotend_2, 21,     SEMITEC_100_47)
THERMISTOR(3, hotend_3, 20,     SEMITEC_100_47)
THERMISTOR(4, hotbed,   19,     BED100K)

#endif



/*
 * Endstops :
 * For each thermistor you may want to include, add a line like the following :
 *
 *  THERMISTOR(index, data_pin, enable_value) :
 *      - index : the index of the endstop. Indices are strictly consecutive, starting by 0;
 *      - data_pin : the pin that will be used to acquire binary data;
 *      - enable_value : the boolean value corresponding to the "enabled" state
 *
 * where indices are strictly consecutive, starting by 0, and type_table and type_size are macros names,
 *      for your thermistor. Those are defined in thermistor_data.h.
 */

#ifdef ENDSTOP

ENDSTOP(0, hotend_0, A26,    SEMITEC_100_47)
ENDSTOP(1, hotend_1, 22,     SEMITEC_100_47)
ENDSTOP(2, hotend_2, 21,     SEMITEC_100_47)
ENDSTOP(3, hotend_3, 20,     SEMITEC_100_47)
ENDSTOP(4, hotbed,   19,     BED100K)

#endif
