/*
  SpeedManager.h - Part of TRACER

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

#include "../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_SPEEDMANAGER_H
#define TRACER_SPEEDMANAGER_H

class SpeedManager {

public :

    //static long *const end_distances;

    static void heuristic_distance();

    static void regulate_speed();

    static void set_speed_distance(unsigned long distance_to_end);

    static void go_to_speed_distance(unsigned long distance_to_end);

    static void set_speed_distance_fast(bool dir, unsigned int increment);


    static void set_delay_parameters(unsigned int tmp_delay_0, unsigned int tmp_delay_numerator, float ratio, unsigned char processing_steps);

    static unsigned int distance_square_root;
    static unsigned int delay0;
    static float delay_numerator;

    static bool speed_processing_required;

private :

    static unsigned char processing_steps;

    static unsigned long distance_to_end;
    static unsigned int regulation_delay;
    static bool speed_incr;
    static bool regulation_stop_enabled;


    static bool regulation_unreached;
    static unsigned long speed_distance;
    static unsigned int square_inf, square_sup, square_increments;

};

#endif //TRACER_SPEEDMANAGER_H

#endif