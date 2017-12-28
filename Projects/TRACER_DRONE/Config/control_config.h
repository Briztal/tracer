/*
  control_config.h - Part of TRACER

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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/


//------------------------------------------------- Interaction -------------------------------------------------

/*
 * Interface - data-link settings
 *                                  WARNING !!!!!
 *
 * Two interface mustn't be based on the same data link. If they to, data will be dispatched
 *  between the two interfaces, breaking its integrity.
 */


//Uncomment this line to enable the terminal interface.
#define ENABLE_TERMINAL_INTERFACE

//Uncomment this line to enable the gcode interface.
#define ENABLE_GCODE_INTERFACE

//Uncomment this line to enable the program interface.
//#define ENABLE_PROGRAM_INTERFACE


/*
 * The next lines focus on Interaction configuration. Interaction are static classes, meant only to interpret data.
 *
 * In order to receive orders from th outside, a Controller must receive its data from a Protocol, that reads and decode
 *  data over transmission layers;
 *
 * Protocols are objects, created by the Kernel;
 *
 *  Transmission layers are static data defined in hardware_language_abstraction.
 *
 *  To create a log_protocol objetc, based on a transmission layer, and piped into a controller,
 *      you may add one line like below :
 *
 *  CONTROL(controller, log_protocol, protocol_buffer_size, transmission_layer), where :
 *
 *      - controller is the name of a Controller class;
 *      - log_protocol is the name of a Protocol class (Interaction/Protocols);
 *      - protocol_buffer_size is the size of the Protocol class's input buffer;
 *      - transmission layer is the name of a transmission layer (hardware_language_abstraction_xxxx.h)
 *
 */

#define NB_CONTROLS 1

#ifdef EXTERNAL_CONTROL

EXTERNAL_CONTROL(Terminal, ClearText, 100, usb_serial)

#endif