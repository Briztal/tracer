/*
  MachineControllerSystem.h - Part of TRACER

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

#ifndef CODE_GCODEPARSER_H
#define CODE_GCODEPARSER_H

#include "../Interfaces/interface_config.h"

class MachineController {


public:
    //-----------------------------------------------INIT METHOD--------------------------------------------------------

//POWER AND SPEED
static float linearPower;
public:

    static void begin();


    //---------------------------------------------INTERFACE METHODS------------------------------------------------------

    static void system_canal_function(char *dptr, unsigned char size);


#define GO_UPPER

#define GO_LOWER(i, name)

#define CREATE_LEAF(i, name)\
    static void name(char * dptr, unsigned char size);

#define CREATE_CALLABLE_LEAF(i, name)\
    CREATE_LEAF(i, name)

#include "../Interfaces/interface_config.h"

#undef GO_UPPER
#undef GO_LOWER
#undef CREATE_LEAF
#undef CREATE_CALLABLE_LEAF


    //--------------------------------------------ACTION METHODS--------------------------------------------------------

public:

    static void adjustLinearPower();

    static void adjustSpeedPower();

    //----------------------------------------------TRACER METHODS-------------------------------------------------------

public:

    static float *parameters;

    static bool *v_parameters;


    static void setLinearPower(float power);

    static void setSpeed(float speed);

    static void absoluteMove();


    //primary drawers (actually calling Steppers);
    static void homeMachine();

    static void ellipticMove();


    //Secondary drawers (calling primary drawers;

    static void relativeMove(float x, float y, float z);

    static void drawPolygon();

    //TODO UNSAFE

    static void bezierMove();


    //---------------------------------------------------Fields---------------------------------------------------------

private:

    //Watt per Millimeter per second (W/(mm/s))
    static float power; //power ratio;
    static float speed; //mm per second

    //---------------------------------------------------Methods--------------------------------------------------------


    static void parameters_system_canal(char *dptr, unsigned char size);

    static void pid_system_canal(char *dptr, unsigned char size);

    static void loop_system_canal(char *dptr, unsigned char size);

    static void steppers_system_canal(char *dptr, unsigned char size);

    static void actions_system_canal(char *dptr, unsigned char size);
};


#endif //CODE_GCODEPARSER_H
