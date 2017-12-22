/*
  eeprom_config.h - Part of TRACER

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

/* EEPROM custom data definition : for each variable you need to save in EEPROM, write_data the following line
 *  with the appropriate parameters;
 *
 *  EEPROM_VARIABLE(name, default_value)
 *
 *  This function will create a float variable with the name provided in the EPROMStorage class,
 *      that will be saved in EEPROM, and take the default value provided
 */

#if defined(EEPROM_VARIABLE)

EEPROM_VARIABLE(maximum_speed, 500);

#endif