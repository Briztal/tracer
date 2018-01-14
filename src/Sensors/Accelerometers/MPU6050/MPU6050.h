/*
  MPU6050.h - Part of TRACER

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

#ifndef TRACER_MPU6050_H
#define TRACER_MPU6050_H


#include "stdint.h"


class MPU6050 {

    //----------------------------------- Initialisation -----------------------------------

public:

    //Initialise the connection and the sensor;
    static void initialise_hardware();

    //Initialise all fields to a safe state;
    static void initialise_data();


private:

    //Reset the connection;
    static void reset_connection();

    //Reset the chip;
    static void reset_mpu();


    //----------------------------------- Interaction with the chip -----------------------------------

private:

    //Read a single byte at the given address [address];
    static uint8_t read_byte(uint8_t address);

    //Read 6 bytes at [adress]
    static void read_int16s(uint8_t address, uint8_t nb_ints, int16_t *int_array);

    //Write [data] at the address [address];
    static void write_byte(uint8_t address, uint8_t data);

    //Check that the byte at [address] is [data];
    static bool check_byte(uint8_t address, const uint8_t data);


    //----------------------------------- Data manipulation -----------------------------------

private:

    //Write the [length] first bits of [data] in [adress] in the chip, offseted of [offset];
    static void write_bits(uint8_t address, uint8_t data, uint8_t offset, uint8_t length);


    //----------------------------------- Computation -----------------------------------

    //Compute the acceleration's theta and phi;
    static void compute_acceleration_angles(float *phi, float *theta);


    //----------------------------------- Data transmission -----------------------------------

    //Get the acceleration angles;
    static void get_acceleration_angles(float *phi, float *theta);

    //Get the acceleration vector;
    static void get_acceleration_vector(int16_t *acc_x, int16_t *acc_y, int16_t *acc_z);


    //----------------------------------- Fields -----------------------------------

private:

    //The acceleration vector;
    static int16_t *acceleration_vector;

    //The acceleration vector's polar angle (x origin);
    static float acceleration_phi;

    //The acceleration vector's azimuth angle (z origin);
    static float acceleration_theta;



};


#endif //TRACER_MPU6050_H
