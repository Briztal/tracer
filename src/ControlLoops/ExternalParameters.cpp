/*
  ExternalParameters.cpp - Part of TRACER

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

#include "ExternalParameters.h"

//------------------------------------------------------PID-------------------------------------------------------------

#define PID(i, name, kp, ki, kd)\
float ExternalParameters::get_pid_##i(float error) {\
    sum_##i+=error;\
    float ret = kp*error+ki*sum_##i+kp*(error-last_##i);\
    last_##i = error;\
    return ret;\
}\
void ExternalParameters::reset_pid_##i() {\
    sum_##i=0;last_##i = 0;\
}\
float ExternalParameters::sum_##i;\
float ExternalParameters::last_##i;

#include "../config.h"

#undef PID


//-----------------------------------------------External Parameters----------------------------------------------------



#include "../hardware_language_abstraction.h"
#include "../config.h"

#undef LOOP_FUNCTION

//-----------------------------------------------DECLARATIONS-----------------------------------------------------------

#define LOOP_FUNCTION(indice, name, period_ms) \
bool ExternalParameters::active_##indice = false;
#include "../config.h"

#undef LOOP_FUNCTION

//---------------------------------------------TRIGGER_FUNCTIONS--------------------------------------------------------


 */
