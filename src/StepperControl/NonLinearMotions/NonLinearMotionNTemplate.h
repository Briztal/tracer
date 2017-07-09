/*
  NonLinearMotionNTemplate.h - Part of TRACER

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

#ifndef TRACER_NONLINEARMOTIONNTEMPLATE_H
#define TRACER_NONLINEARMOTIONNTEMPLATE_H


#include "NonLinearMotionN.h"

class NonLinearMotionNTemplate : public NonLinearMotionN {

    //-------------------inherited methods

protected:

    bool prepare_motion();

    void move();

    void init_position_parameters();

    bool process_position();

    void get_position(float a, long *c);



    //-------------------fields and methods

protected:

    //TODO YOUR FIELDS AND METHODS

};


#endif //TRACER_NONLINEARMOTIONNTEMPLATE_H
