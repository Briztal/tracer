/*
  OrientationController.h - Part of TRACER

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
 * OrientationController : this class is a feedback controller, used to control the orientation of a system.
 *
 *  Its target is expressed in the (incomplete) spherical coordinate system, (theta, phi), where :
 *      - theta represents the inclination orientation angle [0;2Pi]
 *      - phi represents the inclination intensity [0;PI];
 *
 *
 *                                    Upper view
 *                                               theta
 *                                            r |__  /
 *       (O, l, theta) view                     |  \/
 *      .                                       |  /
 *      |\                                      | /
 *      | \                                    l|/         t
 *      |  \                        ------------.-------------
 *      |__/\                                   |
 *      |    \                                  |
 *     l| phi \                                 |
 *                                              |
 *                                              |
 *
 * Inputs :
 *
 *      target position  (theta, phi)
 *
 *      current position (theta, phi)
 *
 *
 *  output : rotation vector, in the (r, t) plan.
 *
 *      this vector is the correction vector, that will lead the system to its target;
 *
 *      It is obtained from two PIDs, one for r, one for t.
 *
 *
 *
 * Process :
 *
 *      Notations :
 *          - target_theta = Tt, target_phi = Pt;
 *          - current_theta = T, current phi = P;
 *
 *      The Error vector in the (r, t) plan, (Er, Et) verifies the following formula :
 *
 *          Er = cos(Tt) * Pt - cos(T) * P;
 *          Et = sin(Tt) * Pt - sin(T) * P;
 *
 *      After this calculation, two PIDs (PID_r, PID_t) are feed with appropriate error values.
 *
 *      The output vector (Or, Ot) is then determined. Both Or and Ot are outputs of their respective PIDs;
 *
 */

#ifndef TRACER_ORIENTATIONCONTROLLE_H
#define TRACER_ORIENTATIONCONTROLLE_H

#include "PID.h"

class OrientationController {

public:

    struct rotation_vector {

        //The r coordinate;
        float r = 0;

        //The t coordinate;
        float t = 0;

    };

    //----------------------------------- Init ---------------------------------------

public:

    //Constructor;
    OrientationController();

    //Destructor;
    ~OrientationController();

    //Reset all data;
    void reset();


    //----------------------------------- Computation ---------------------------------------

public:

    //Set the target theta and phi;
    void setTaget(float target_theta, float target_phi);

    //Get the correction vector corresponding to the current position;
    void getCorrectionVector(float current_theta, float current_phi, rotation_vector *vector);


    //----------------------------------- Fields ---------------------------------------

private:

    //The target position, in the spherical coordinates;
    float target_theta, target_phi;

    //The semi-errors : cos(Tt) * Pt and sin(Tt) * Pt;
    float semi_error_r, semi_error_t;

    //PIDs :
    PID *pid_r, *pid_t;


    //----------------------------------- private methods ---------------------------------------

private:

    //Update semi_errors;
    void computeSemiErrors();

};


#endif //TRACER_ORIENTATIONCONTROLLE_H
