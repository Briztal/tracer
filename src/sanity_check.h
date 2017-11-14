/*
  sanity_check.h - Part of TRACER

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

#include "hardware_language_abstraction.h"
#include <config.h>

/*
 * Control Interface checking.
 *  We must check that only one interface is the main one, and that the main interface is enabled.
 *
 */

#ifdef MAIN_CI_TERMINAL
#if defined(MAIN_CI_GCODE) || defined(MAIN_CI_PROGRAM)
#error "You have two main interfaces selected. Please disable one."
#endif
#ifndef ENABLE_TERMINAL_INTERFACE
#error "Your main interface (the TerminalInterface) is not enabled."
#endif
#endif

#ifdef MAIN_CI_GCODE
#if defined(MAIN_CI_TERMINAL) || defined(MAIN_CI_PROGRAM)
#error "You have two main interfaces selected. Please disable one."
#endif
#ifndef ENABLE_GCODE_INTERFACE
#error "Your main interface (the GCodeInterface) is not enabled."
#endif
#endif

#ifdef MAIN_CI_PROGRAM
#if defined(MAIN_CI_TERMINAL) || defined(MAIN_CI_TERMINAL)
#error "You have two main interfaces selected. Please disable one."
#endif
#ifndef ENABLE_PROGRAM_INTERFACE
#error "Your main interface (the TreeInterface) is not enabled."
#endif
#endif


/*
 * Control loops checking.
 *  We must verify that there are enough timers for the required control loops number
 *
 */

#if (NB_CONTROL_LOOP_TIMERS < NB_LOOPS)
#error You have required too much control loops (NB_LOOPS) in your configuration file. Your board supports at most NB_CONTROL_LOOP_TIMERS loops.
#endif


/*
 * Stepper Control Kernel checking.
 *  we must check the following flags :
 *      - EEPROM for all kernels
 *      - FPU for Kernel2
 *      - 32 bits for kernel2
 */

#ifdef ENABLE_STEPPER_CONTROL

#ifndef KERNEL
#error "The Kernel version is not defined in your config.h"
#endif

#ifndef HL_EEPROM_FLAG
#error "KinematicsCores require the EEPROM support, and you board doesn't support it."
#endif

#if (KERNEL == 0)
//NO verification required, this kernel works on every damn thing...

#elif ((KERNEL == 1) || (KERNEL == 2))
//KinematicsCore1 and KinematicsCoreV2 require a 32 bits processor with an FPU

//FPU
#ifndef HL_FPU_FLAG
#error "KinematicsCore2 requires an FPU, and your processor doesn't have one."
#endif

//32 Bits
#ifndef HL_32BITS_FLAG
#error "KinematicsCore2 requires  a 32 bit processor. Your processor has a lower native type."
#endif

#else
#error "Please enter a valid kernel name. You may enter one of the following : 0, 1, and 2."
#endif

#endif //ENABLE_STEPPER_CONTROL