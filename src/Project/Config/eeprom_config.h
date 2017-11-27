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

//TODO DESCRIPTION

/* EEPROM custom data definition : for each variable you need to save in EEPROM, write one of the following lines
 *
 * EEPROM_BOOL(name, default_value)
 * EEPROM_INT8(name, default_value)
 * EEPROM_INT16(name, default_value)
 * EEPROM_INT32(name, default_value)
 * EEPROM_FLOAT(name, default_value)
 *
 * Each one of these functions will create a variable of the specified type with the name provided
 *  in the EPROMStorage class, that will be saved in EEPROM, and take the default value provided
 */


//The maximum number of characters in the name of an eeprom variable (must be greater than 15)

#define EEPROM_NAMES_MAX_LENGTH 15

#if defined(EEPROM_VARIABLE)

EEPROM_VARIABLE(maximum_speed, 500);

#endif