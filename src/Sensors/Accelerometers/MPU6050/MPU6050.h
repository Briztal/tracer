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


#include <DataStructures/CoordinateSystems/Vector3D.h>
#include <math.h>
#include <DataStructures/CoordinateSystems/Angles3D.h>
#include "stdint.h"


class MPU6050 {

    //----------------------------------- Initialisation -----------------------------------

public:

    MPU6050();

    ~MPU6050();

    //Initialise the connection and the sensor;
    void initialise_hardware();

    //Initialise all fields to a safe state;
    void initialise_data();


private:

    //Reset the connection;
    void reset_connection();

    //Reset the chip;
    void reset_mpu();


    //----------------------------------- Interaction with the chip -----------------------------------

private:

    //Read a single byte at the given address [address];
    uint8_t read_byte(uint8_t address);

    //Read 6 bytes at [adress]
    void read_int16s(uint8_t address, uint8_t nb_ints, int16_t *int_array);

    //Write [data] at the address [address];
    void write_byte(uint8_t address, uint8_t data);

    //Check that the byte at [address] is [data];
    bool check_byte(uint8_t address, const uint8_t data);

    //Write the [length] first bits of [data] in [adress] in the chip, offseted of [offset];
    void write_bits(uint8_t address, uint8_t data, uint8_t offset, uint8_t length);


    //----------------------------------- Data Query -----------------------------------

public:

    //Query accelerometer data:
    void compute_accelerometer_data();

    //Query gyrometer data;
    void compute_gyrometer_data();

    //Query both accelerometer and gyrometer data;
    void compute_data();


    //----------------------------------- Data transmission -----------------------------------

public:

    //Get the acceleration values;
    void get_accelerometer_data(Vector3D &output);

    //Get the gyrometer values;
    void get_gyrometer_data(Triplet &output);


    //----------------------------------- Fields -----------------------------------

private:

    //The accelerometer array;
    Vector3D accelerometer_data;

    //The gyrometer array;
    Triplet gyrometer_data;

    //The ratio to convert gyro array to rad/s;
    float gyrometer_ratio = ((float) 1000  * (float) M_2_PI) / ((float) 65536 * (float(180)));

};


#endif //TRACER_MPU6050_H
