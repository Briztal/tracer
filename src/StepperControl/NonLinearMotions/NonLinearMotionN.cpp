/*
  NonLinearMotionN.cpp - Part of TRACER

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


#include "NonLinearMotionN.h"
#include "../../Interfaces/CommandInterface.h"


/*
void NonLinearMotionN::initialise_motion() {
    unsigned char a;
    for (unsigned char axis = 0; axis < motion_size; axis++) {
        a = axis_copy_t[axis] = axis_t[axis];
        steps[axis] = EEPROMStorage::steps[a];
        assignFunction(a, dir_set_functions + axis);
    }
}

void NonLinearMotionN::set_last_position() {
    Motion::set_end_position(last_positions, axis_t, motion_size);
}


void NonLinearMotionN::move() {

    //Dir Setting

    CI::echo("SET DIR");
    for (unsigned char axis = 0; axis < motion_size; axis++) {
        (**(dir_set_functions + axis))(true);
    }
    memset(dirs, true, motion_size);


    set_signatures();

    set_speed_coefficients();


    increment_provided = false;

    //Speed axis setting
    set_speed_axis(*axis_t, 1);

    //Child class init
    init_position_parameters();

    setup_engagement_move();

    initialise_motion_vars();

    draw();

    send_position();

}

void NonLinearMotionN::setup_engagement_move() {
    Motion::set_end_position(first_positions, axis_t, motion_size);
    memcpy (destinations, first_positions,  motion_size<<2 );
}


void NonLinearMotionN::set_speed_coefficients() {
    float invsteps0 = 1.0 / *steps;
    for (unsigned char axis = 1; axis < motion_size; axis++) {
        speed_coeffs[axis] = steps[axis] * invsteps0;
    }
}

void NonLinearMotionN::set_end_distances() {
    bool * d = dirs;
    int i;
    for (int axis = motion_size-1; axis--;) {
        i = (int) (last_pos_t[axis] - pos_t[axis]);
        bool is_pos = (i>0);
        elementary_dists[axis] = is_pos ? (unsigned char) (i) : (unsigned char) (-i);
        if (*d^is_pos) {
            (*(dir_set_functions + axis))(is_pos);
            *d= is_pos;
        }
    }

    memcpy(positions, last_positions, motion_size<<2);
}


void NonLinearMotionN::initialise_motion_vars() {

    //Determine increment if necessary;
    if (!increment_provided) {
        increment = extract_increment(min, max, 0.0005, processing_steps);
    }

    //Initialise positions
    memcpy(pos_t, first_pos_t, motion_size<<2);

    //initialise alpha for the first move;
    alpha = min + increment;

    //Initialise the motion counter
    count = (unsigned int) ((max - min) / increment);

}


float NonLinearMotionN::extract_increment(float point, float end, float increment, unsigned int processing_steps) {
    const unsigned char ms = motion_size;

    long pos[ms], dest[ms];
    unsigned int dist;
    float tmp;

    increment = adjust_increment(point, increment);
    point += increment;
    get_position(point, pos);

    while (point + increment < end) {
        get_position(point + increment, dest);

        dist = get_distance(dest, pos);

        if (dist < processing_steps) {
            tmp = increment / 3.;
            while (dist < processing_steps) {
                increment += tmp;
                get_position(point + increment, dest);
                dist = get_distance(dest, pos);
            }
        }

        point += increment;

        memcpy(pos, dest, ms<<2);
    }

    return increment;
}


float NonLinearMotionN::adjust_increment(float point, float step) {


    unsigned char dist;

    long pos[motion_size], dest[motion_size];
    get_position(point, pos);
    get_position(point + step, dest);
    dist = get_distance(dest, pos);

    bool lastSign = ((dist) > processing_steps);
    float incr = step / 5.;

    //if dist>processing steps, incr must be opposed to step
    if (lastSign)
        incr *= -1;

    //Dichotomy
    while (dist != processing_steps) {
        //If the state changed :
        if (lastSign ^ (dist > processing_steps)) {
            incr *= -1. / 5.;
            lastSign = (dist > processing_steps);
        }

        step += incr;
        get_position(point + step, dest);
        dist = get_distance(dest, pos);
    }

    return step;

}

unsigned char NonLinearMotionN::get_distance(long *dest, long *pos) {
    int d;
    unsigned char dist;
    unsigned char maxi = 0;

#define maximum(a, b) ((a<b) ? b : a)

    for (unsigned char axis = 0; axis < motion_size; axis++) {
        d = (int) (dest[axis] - pos[axis]);
        dist = (unsigned char) ((d < 0) ? -d : d);
        if (maxi < dist) {
            maxi = dist;
        }
    }

    return maxi;
}


void NonLinearMotionN::step_and_delay(unsigned char sig) {
    //simple_delay(StepperController::fastStepDelay(sig));
}


void NonLinearMotionN::set_signatures() {
    //Steppers axis_signatures assignment
    for (int axis = 0; axis < motion_size; axis++) {
        axis_signatures[axis] = (unsigned char) 1 << axis_copy_t[axis];
    }
}

void NonLinearMotionN::checkPositionByStep(float *offsets, unsigned int d) {
    //reset_data_pointers();
    pos_data *p = pos_data_pointer;
    for (int i = 0; i < count; i++, alpha += increment) {
        while (!process_position(p));
        *processors_state = false;

        CI::prepare_data_out();

        float t[motion_size];
        for (int axis = 0; axis < motion_size; axis++) {
            t[axis] = pos_t[axis] / steps[axis] + offsets[axis];
            CI::add_float_out(pos_t[axis] / steps[axis] + offsets[axis]);
        }

        CI::send_position();
        delayMicroseconds(d);
    }
}


void NonLinearMotionN::checkPosition(float *offsets, unsigned int d) {
    long c[NB_STEPPERS];

    for (alpha = min; alpha < max; alpha += increment) {
        get_position(alpha, c);
        for (int axis = 0; axis < motion_size; axis++) {
            CI::add_float_out(c[axis] / steps[axis] + offsets[axis]);
        }

        CI::send_position();
        delayMicroseconds(d);
    }
}

void *nt_spd_ptr;
float tmpdelay;
unsigned char axis;

void NonLinearMotionN::process_speed() {
    if (processors_state[3])
        goto
    *nt_spd_ptr;
    processors_state[3] = true;

    //speed_processing_1 :
    speed_processing_required = false;
    inverse = invert(distance_square_root);
    nt_spd_ptr =&&speed_processing_2;
    return ;//false;

    speed_processing_2 :
    tmpdelay = delay_numerator * inverse;
    StepperController::delays[*axis_t] = delay0 = (unsigned int) tmpdelay;
    axis = motion_size - (unsigned char) 1;
    nt_spd_ptr =&&speed_processing_3;
    return ;//false;

    speed_processing_3:
    StepperController::delays[axis] = (unsigned int) (speed_coeffs[axis] * tmpdelay);

    return ;//(!(--axis));

}

void NonLinearMotionN::provide_increment(float i) {
    increment = i;
    increment_provided = true;
}

void NonLinearMotionN::draw() {

    //count alias;
    unsigned int count = this->count;

    Motion::pre_draw();

    //Do all regular moves;
    while (--count) {
        alpha += increment;
        //prepare_next_sub_motion(false);
    }

    //Last regular move, without process
    //prepare_next_sub_motion(true);

    //set end distances
    set_end_distances();

    //Process last move
    //reset_data_pointers();

    /*mov_data *m = mov_data_pointer;
    while (!process_next_move(m));*/

    //Last move (irregular);
    //prepare_next_sub_motion(true);
/*
    Motion::post_draw();
    return;
}


#define m NonLinearMotionN

unsigned char m::motion_size;
float m::alpha, m::increment;
bool m::increment_provided = false;
float m::min, m::max;


unsigned char taxis[NB_STEPPERS];
unsigned char *const m::axis_copy_t = taxis;

float tstps[NB_STEPPERS];
float *const m::steps = tstps;

float tsc[NB_STEPPERS];
float *const m::speed_coeffs = tsc;

long tpt[NB_STEPPERS];
long *const m::pos_t = tpt;

long tlpt[NB_STEPPERS];
long *const m::last_pos_t = tlpt;

long tfpt[NB_STEPPERS];
long *const m::first_pos_t = tfpt;

long tfpos[NB_STEPPERS];
long *const m::first_positions = tfpos;

long tlpos[NB_STEPPERS];
long *const m::last_positions = tlpos;

void (*t_fdirs[NB_STEPPERS])(bool dir);

void (**m::dir_set_functions)(bool dir) = t_fdirs;


*/