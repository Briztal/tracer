//
// Created by root on 3/2/18.
//

#ifndef TRACER_ANGLES3D_H
#define TRACER_ANGLES3D_H

/*
 * This class is a representation of a 3D vector, with 4 coordinates :
 *
 *  - the length of the vector;
 *  - the x angle, on yz plane, referenced by ux;
 *  - the y angle, on zx plane, referenced by uz;
 *  - the z angle, on xy plane, referenced by uy;
 *
 *  The system has 3 degrees of freedom, and so, a (non linear) relation exists between x, y, and z;
 *  
 *  It can be seen as derivation of the spherical system, with the azimuth expressed in two angles.
 *  
 */

class Angles3D {
    
    //--------------------------- Initialisation ---------------------------


    //--------------------------- Data ---------------------------

public:
    
    //The vector's length;
    float length;
    
    //The triplet of angle;
    float xAngle, yAngle, zAngle;
    
    
    
    


};


#endif //TRACER_ANGLES3D_H
