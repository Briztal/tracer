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

#include <hardware_language_abstraction.h>
#include <config.h>

//Control Interface checking

#if defined(MAIN_CI_TREE) && defined(MAIN_CI_GCODE)
#error : "There can be only one main interface. Please enable only one."
#endif


//Stepper Control Kernel checking
#ifdef ENABLE_STEPPER_CONTROL

#ifndef KERNEL
#error "The Kernel version is not defined in your config.h"
#endif

#if (KERNEL == 0)
//NO verification required, this kernel works on every damn thing...

#elif ((KERNEL == 1) || (KERNEL == 2))
//Kernel1 and Kernel2 require a 32 bits processor with an FPU



//FPU
#ifndef HL_FPU_FLAG
#error "Kernel1 requires an FPU, and your processor doesn't have one."
#endif


//FPU
#ifndef HL_32BITS_FLAG
#error "Kernel1 requires  a 32 bit processor. Your processor has a lower native type."
#endif

#else
#error "Please enter a valid kernel name. You may enter one of the following : 0, 1, and 2."
#endif

#endif //ENABLE_STEPPER_CONTROL