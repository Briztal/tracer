/*
  MotorRegisterer.h - Part of TRACER

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

#ifndef TRACER_MOTORREGISTERER_H
#define TRACER_MOTORREGISTERER_H

#include "stdint.h"

class MotorRegisterer {


    //----------------------------- Initialisation -----------------------------

protected:

    //Constructor;
    explicit MotorRegisterer(uint8_t nb_motors);

    //Destructor;
    ~MotorRegisterer();


    //----------------------------- Registration -----------------------------

protected:

    //Register a motor;
    void setRegistered(uint8_t index);


    //----------------------------- Checks -----------------------------

protected:

    //Check is a motor is already registered;
    bool isRegistered(uint8_t index);

    //Check if all motors have been registered;
    bool allRegistered();


    //----------------------------- Fields -----------------------------

    //The number of motors to register;
    const uint8_t nbMotors;

    //The array of flags : i-th set -> the i-th motor is registered;
    bool *const registrationFlags;

};


#endif //TRACER_MOTORREGISTERER_H
