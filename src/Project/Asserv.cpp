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
    float ret = kp*error+ki*sum_##i+kp*(error-last_##i);\
    last_##i = error;\
    return ret;\
}\
void Asserv::reset_pid_##i() {\
    sum_##i=0;last_##i = 0;\
}\
float Asserv::sum_##i;\
float Asserv::last_##i;

#include "../config.h"

#undef PID


//-----------------------------------------------External Parameters----------------------------------------------------



//--------------------------------------------------Loop functions------------------------------------------------------


#define LOOP_FUNCTION(indice, name, period_ms) \
    void Asserv::enable_##indice(){\
        init_loop_##indice();\
        setup_loop_interrupt_##indice(trigger_##indice, period_ms);\
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

#undef LOOP_FUNCTION

//-----------------------------------------------DECLARATIONS-----------------------------------------------------------

#define LOOP_FUNCTION(indice, name, period_ms) \
bool Asserv::active_##indice = false;
#include "../config.h"

#undef LOOP_FUNCTION

//---------------------------------------------TRIGGER_FUNCTIONS--------------------------------------------------------



#include "../interface.h"


void Asserv::init_loop_0() {
    reset_pid_0();
}

#include <string>

void Asserv::trigger_0() {

    disable_loop_interrupt_0();

    float error = 0.1;

    float p = get_pid_0(error);


    enable_loop_interrupt_0();
}

void Asserv::init_loop_1() {
    reset_pid_1();
}

void Asserv::trigger_1() {

    disable_loop_interrupt_0();

    float error = 0.1;

    float p = get_pid_1(error);


    CI::echo("hotbed "+str(p));

    enable_loop_interrupt_0();
}




