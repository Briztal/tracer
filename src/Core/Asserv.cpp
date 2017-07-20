/*
  Asserv.cpp - Part of TRACER

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

#include "Asserv.h"

//------------------------------------------------------PID-------------------------------------------------------------

#define PID(i, name, kp, ki, kd)\
float Asserv::get_pid_##i(float error) {\
    sum_##i+=error;\
    float ret = kp*error+ki+sum_##i+kp*(error-last_##i);\
    last_##i = error;\
    return ret;\
}

#include "../config.h"

#undef PID


//-----------------------------------------------External Parameters----------------------------------------------------

//TODO Implement all GET_PARAMETER()

//-----------------------------------------------Internal parameters----------------------------------------------------

//TODO Implement all updating functions

//--------------------------------------------------Loop functions------------------------------------------------------


#define LOOP_FUNCTION(indice, name, period_ms) \
    void Asserv::enable_##indice(){\
        enable_loop_interrupt_##indice(trigger_##indice, period_ms);\
        active_##indice=true;\
    }\
    void Asserv::disable_##indice(){\
        disable_loop_interrupt_##indice();\
        active_##indice=false;\
    };\
    bool Asserv::is_active_##indice() {\
        return active_##indice;\
    }


#include "../hardware_language_abstraction.h"
#include "../config.h"

#undef LOOP_FUNCTION;

//TODO Implement all loop functions


void Asserv::trigger_0() {

}


#define LOOP_FUNCTION(indice, name, period_ms) \
bool Asserv::active_##indice = false;
#include "../config.h"

#undef LOOP_FUNCTION;


