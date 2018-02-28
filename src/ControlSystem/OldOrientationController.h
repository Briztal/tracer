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
 * OldOrientationController : this class is a feedback controller, used to control the orientation of a system.
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
 *      A trivial error vector in the (r, t) plan, (Er, Et) would verify the following formula :
 *
 *          Er = cos(Tt) * Pt - cos(T) * P;
 *          Et = sin(Tt) * Pt - sin(T) * P;
 *
 *      However, according to this formula, when both targets and positions are at phi = pi, the error is not null,
 *          and depends on theta. It needs to be developed a little bit.
 *
 *      However, this formula does its job well when almost one phi coordinate is below pi/2;
 *
 *      As a consequence, the error vector verifies the formula below :
 *
 *
 *      If one phi (target or current) is <= pi/2 (case 0) :
 *
 *          Er = cos(Tt) * Pt - cos(T) * P;
 *          Et = sin(Tt) * Pt - sin(T) * P;
 *
 *      If both phi.s are > pi/2 (case 1) :
 *
 *          Er = cos(Tt) * (pi - Pt) - cos(T) * (pi - P);
 *          Et = sin(Tt) * (pi - Pt) - sin(T) * (pi - P);
 *
 *          (The behaviour in the > pi / 2 area is the symmetric of the behaviour in the <= pi / 2 zone;
 *
 *      After this calculation, two PIDs (PID_r, PID_t) are feed with appropriate error values.
 *
 *      The output vector (Or, Ot) is then determined. Both Or and Ot are outputs of their respective PIDs;
 *
 */

#ifndef TRACER_ORIENTATIONCONTROLLE_H
#define TRACER_ORIENTATIONCONTROLLE_H

#include "PID.h"

class OldOrientationController {

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
    OldOrientationController();

    //Destructor;
    ~OldOrientationController();

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

    //The angular zone of target and current phi.s : 0 = below pi / 2, 1 = beyond pi / 2;
    bool target_in_zone_1, zone_current;

    //The semi-errors for case 0 : cos(Tt) * Pt and sin(Tt) * Pt;
    float semi_error_r_0, semi_error_t_0;

    //The semi-errors for case 1 : cos(Tt) * (pi - Pt) and sin(Tt) * (pi - Pt);
    float semi_error_r_1, semi_error_t_1;

    //PIDs :
    PID *pid_r, *pid_t;


    //----------------------------------- private methods ---------------------------------------

private:

    //Update semi_errors;
    void computeSemiErrors();


    //----------------------------------- EEPROMMap Registration ---------------------------------------

public:

    //Register PIDs in the EEPROMMap;
    void EEPROMRegister(char *path, const char *name);

};


#endif //TRACER_ORIENTATIONCONTROLLE_H
