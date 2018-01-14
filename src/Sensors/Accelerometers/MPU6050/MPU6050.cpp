/*
  MPU6050.cpp - Part of TRACER

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

#include "MPU6050.h"

#include <Interaction/Interaction.h>

#include "hardware_language_abstraction.h"



#define N_ERROR                1000

#define MPU_PERIOD_US        2000        //us

#define MPU_PERIOD_MS        MPU_PERIOD_US/1000

#define MPU_ADDRESS            0x68

#define MPU_POWER            0x6B

#define MPU_POWER_SLEEP_BIT    6

#define MPU_RESET            0x68

#define MPU_RESET_VALUE        0x07    //	00000111 reset gyro/accel/temp

#define MPU_CONFIG            0x1A

#define MPU_ACCEL_MEASURE    0x3B

#define MPU_ACCEL_CALIB        0x1C

#define MPU_ACCEL_CALIB_BIT    4

#define MPU_ACCEL_CALIB_LEN    2

#define MPU_ACCEL_CALIB_AFS    0x02    //+-8

#define MPU_ACCEL_DIVIDER    (float)4096    //4096 LSB/g pour �8g

#define MPU_GYRO_MEASURE    0x43

#define MPU_GYRO_CALIB        0x1B

#define MPU_GYRO_CALIB_BIT    4

#define MPU_GYRO_CALIB_LEN    2

#define MPU_GYRO_CALIB_FS    0x02    //+-1000

#define MPU_GYRO_DIVIDER    (float)32.8    //32.8 LSB/�/s pour �1000�/s

#define MPU_CLKSEL_BIT      2

#define MPU_CLKSEL_LENGTH   3

#define MPU_CLOCK_GYRO      0x01

#define TO_DEGREES            (float)(180.0/PI)


//Complementary filter:

#define FILTER_ACCEL        (float)0.005

#define FILTER_GYRO            (float)(1.0-FILTER_ACCEL)


//The default timeout for the chip (10 ms);
#define MPU6050_TIMEOUT_US 10000



//--------------------------------------------------- Initialisation ---------------------------------------------------


/*
 * initialise_hardware : this function will initialise the connection and the chip;
 */

void MPU6050::initialise_hardware() {

    //Initialise the connection;
    reset_connection();

    //Initialise the chip;
    reset_mpu();

}


/*
 * reset_connection : this function will reset the  I2C connection with the chip;
 */

void MPU6050::reset_connection() {

    //First, let's turn on the I2C module
    Wire.begin();

    //Setup the I2C clock;
    Wire.setClock(100000);

    //Log;
    std_out("Initialising I2C connection with MPU");

    //Initialise the connection with the MPU;
    while (error) {

        //Initialise a transmission with the MPU device;
        Wire.beginTransmission(MPU_ADDRESS);

        //Try to establish the connection;
        uint8_t error_code = Wire.endTransmission(I2C_STOP, 50000);

        //If no error_code occured (zero received)
        if (!error_code) {

            //Complete;
            break;

        }

        //If the connection failed :

        //Log
        std_out("\nNo I2C connection\n...trying again\n");

        //Rest the I2C bus;
        Wire.resetBus();
        Wire.begin();
        Wire.setClock(100000);

        //Wait for 200 ms;
        delay_ms(200);

    }

    //Log;
    std_out("Yay, connected to I2C device");

}

void MPU6050::reset_mpu() {

    std_out("Resetting MPU6050");

    //Enable the chip;
    write_byte(MPU_POWER, 0x80);
    delay(100);

    //Wake the chip up;
    write_bits(MPU_POWER, 0, MPU_POWER_SLEEP_BIT, 1);    //Wake up !(it's asleep by default when reset)
    delay(100);

    //Select the gyro clock;
    write_bits(MPU_POWER, MPU_CLOCK_GYRO, MPU_CLKSEL_BIT, MPU_CLKSEL_LENGTH);    //Clock selection
    delay(100);

    //Select the gyro range : +-1000deg/s;
    write_bits(MPU_GYRO_CALIB, MPU_GYRO_CALIB_FS, MPU_GYRO_CALIB_BIT, MPU_GYRO_CALIB_LEN);    //Gyro range +-1000deg/s
    delay(100);

    //Select the Accelerometer range : +-8g;
    write_bits(MPU_ACCEL_CALIB, MPU_ACCEL_CALIB_AFS, MPU_ACCEL_CALIB_BIT, MPU_ACCEL_CALIB_LEN); //Accel range +-8g

    std_out("Ranges set");

}

void MPU6050::initialise_data() {

    //Reset the acceleration vector;
    memset(acceleration_vector, 0, 3 * sizeof(float));

    //Reset theta and phi
    acceleration_phi = acceleration_theta = 0;

}


//---------------------------------------------- Interaction with the chip ---------------------------------------------



/*
 * read_byte : this function reads and returns the byte at the address [address] in the chip's memory;
 */

uint8_t MPU6050::read_byte(uint8_t address) {


    //Initialise the TX buffer, and set the MPU as slave;
    Wire.beginTransmission(MPU_ADDRESS);

    //Add the address to the TX buffer;
    Wire.write(address);

    //Transmit data to the chip;
    uint8_t error_code = Wire.endTransmission(I2C_STOP, MPU6050_TIMEOUT_US);

    //If the transmission failed :
    if (error_code) {

        //Log;
        std_out("Error in MPU6050::read_byte : transmission error");

    }

    //Read data incoming from the chip during the default period;
    size_t bytes_received = Wire.requestFrom(MPU_ADDRESS, 1, I2C_STOP, MPU6050_TIMEOUT_US);

    //If no bytes were received :
    if (!bytes_received) {

        //Log;
        std_out("Error in MPU6050::read_byte : reception error");

        //Fail
        return 0;

    }

    //Complete;
    return (uint8_t) Wire.read();

}


/*
 * read_bytes : read [nb_int16] pair of bytes starting at [address] and save them in [values]
 */

void MPU6050::read_int16s(uint8_t address, uint8_t nb_ints, int16_t *int_array) {

    //First, multiply the number of ints by two to get the required number of bytes;
    uint8_t nb_bytes = nb_ints << 1;

    //Initialise the TX buffer, and set the MPU as slave;
    Wire.beginTransmission(MPU_ADDRESS);

    //Add the address to the TX buffer;
    Wire.write(address);

    //Transmit data to the chip;
    uint8_t error_code = Wire.endTransmission(I2C_STOP, MPU6050_TIMEOUT_US);

    //If the transmission failed :
    if (error_code) {

        //Log;
        std_out("Error in MPU6050::read_byte : transmission error");

    }


    //Read data incoming from the chip during the default period;
    size_t bytes_received = Wire.requestFrom(MPU_ADDRESS, 6, I2C_STOP, MPU6050_TIMEOUT_US);

    //If the incorrect number of bytes were received : (shift to multiply by 2);
    if (bytes_received != nb_bytes) {

        //Log;
        std_out("Error in MPU6050::read_bytes : expected " + String(nb_bytes) + " bytes, received " +
                String(bytes_received));

        //Fail
        return;

    }

    //For each required int :
    for (int i = 0; i < nb_ints; i++) {

        //Save the int, as the concatenation of multiple uint8_t;
        int_array[i] = ((uint16_t) Wire.read() << 8) | (uint16_t) Wire.read();

    }

}


/*
 * write_byte : this function writes [value] at the address [address] in the chip's memory;
 */

void MPU6050::write_byte(uint8_t address, uint8_t data) {

    //Initialise the TX buffer, and set the MPU as slave;
    Wire.beginTransmission(MPU_ADDRESS);

    //Add the address to the TX buffer;
    Wire.write(address);

    //Add the data to the TX buffer;
    Wire.write(data);

    //Transmit data to the chip;
    uint8_t error_code = Wire.endTransmission(I2C_STOP, MPU6050_TIMEOUT_US);

    //If the transmission failed :
    if (error_code) {

        //Log;
        std_out("Error in MPU6050::write_byte : transmission error");

    }

}


/*
 * write_byte : this function writes [value] at the address [address] in the chip's memory;
 */

bool MPU6050::check_byte(uint8_t address, const uint8_t data) {

    //Read the current value;
    uint8_t current_value = read_byte(address);

    //Return true if data and current_value are the same;
    return (data == current_value);

}


//-------------------------------------------------- Data manipulation -------------------------------------------------


/*
 * write_bits : Write the [length] first bits of [data] in [adress] in the chip, offseted of [offset];
 */

void MPU6050::write_bits(uint8_t address, uint8_t data, uint8_t offset, uint8_t length) {

    //If the section to write is zero-length, do nothing;
    if (!length)
        return;

    //First, let's read the content at [address];
    uint8_t current_value = read_byte(address);

    //Then, we will compute the mask.

    //The mask bits (mask with no offset) are [length] consecutive bits;
    uint8_t mask = ((uint8_t) 1) << (length - (uint8_t) 1);

    //TODO IF DOESN'T WORK :
    //offset -= (length - (uint8_t) 1);

    //The mask, is the mask bits variable, shifted of [offset];
    mask <<= offset;

    //The data must be shifted of the same offset;
    data <<= offset;

    //First, clear the mask part in the current value;
    current_value &= ~mask;

    //Then, add the data part;
    current_value |= data;

    //Finally, write the data;
    write_byte(address, current_value);

    //If the data couldn't be saved;
    if (!check_byte(address, current_value)) {

        //Log;
        std_out("Error in  MPU6050::write_bits : the data was not registered");

    }

}


//----------------------------------------------------- Computation ----------------------------------------------------


/*
 * compute_acceleration_angles : this function get data from the chip, computes acceleration angles, and returns them;
 */

void MPU6050::compute_acceleration_angles(float *phi, float *theta) {

    //Read acceleration values;
    read_int16s(MPU_ACCEL_MEASURE, 6, acceleration_vector);

    //Cache vars :
    float acc_x = (float) acceleration_vector[0];
    float acc_y = (float) acceleration_vector[1];
    float acc_z = (float) acceleration_vector[2];

    //Get the norm of the vector
    float norm = sqrtf(acc_x * acc_x + acc_y * acc_y + acc_z * acc_z);

    //Determine phi
    *phi = acceleration_phi = atanf(acc_y / acc_x);

    //Determine theta
    *theta = acceleration_theta = acosf(norm / acc_z);

}


//------------------------------------------------- Data transmission --------------------------------------------------

/*
 * get_acceleration_angles : this function returns acceleration angles from caches in the class;
 */

void MPU6050::get_acceleration_angles(float *phi, float *theta) {

    //SImply update both angles;
    *phi = acceleration_phi;
    *theta = acceleration_theta;

}


/*
 * get_acceleration_angles : this function returns acceleration coordinates from caches in the class;
 */

void MPU6050::get_acceleration_vector(int16_t *acc_x, int16_t *acc_y, int16_t *acc_z) {

    //Simply update the three variables;
    *acc_x = acceleration_vector[0];
    *acc_y = acceleration_vector[1];
    *acc_z = acceleration_vector[2];

}

//----------------------------------------- Static declarations - definitions ------------------------------------------



//The acceleration angle;
int16_t mpu_acc_vec[3]{0};
int16_t *MPU6050::acceleration_vector = mpu_acc_vec;

//The acceleration vector's polar angle (x origin);
float MPU6050::acceleration_phi = 0;

//The acceleration vector's azimuth angle (z origin);
float MPU6050::acceleration_theta = 0;


/*

void MPU6050::reset() {
    angles[0] = accelAngleX;
    angles[1] = accelAngleY;
    angles[2] = 0;
}

 */

/*

void MPU6050::computeGyroAngle() {

    read_int16s(MPU_GYRO_MEASURE, 6, gyroRaws);

    for (int i = 0; i < 3; i++) {
        gyroSpeeds[i] = (gyroRaws[i] / MPU_GYRO_DIVIDER) - gyroErrors[i];
        angles[i] = angles[i] + dt * gyroSpeeds[i];
    }

    //angle Z dans [-pi;pi]
    float buffer = fmodf(angles[2] + 180, 360);
    if (buffer < 0) {
        buffer += 360;
    }
    angles[2] = buffer - 180;
}

 */



/*

void MPU6050::computeErrors() {


    for (int i = 0; i < N_ERROR; i++) {
        read_int16s(MPU_GYRO_MEASURE, 3, gyroRaws);

        for (int i = 0; i < 3; i++) {
            gyroErrors[i] = gyroErrors[i] + gyroRaws[i] / MPU_GYRO_DIVIDER;
        }
    }

    for (int i = 0; i < 3; i++) {
        gyroErrors[i] = gyroErrors[i] / N_ERROR;
    }
    std_out("Mean gyro errors:");
    Serial.print("X:\t");
    std_out(gyroErrors[0], 10);
    Serial.print("Y:\t");
    std_out(gyroErrors[1], 10);
    Serial.print("Z:\t");
    std_out(gyroErrors[2], 10);

}


 */
/*
void MPU6050::update() {
    static uint32_t thisMeasure = 0;
    static uint32_t lastMeasure = 0;
    thisMeasure = micros();
    //if (thisMeasure - lastMeasure > MPU_PERIOD) {
    dt = (float) (thisMeasure - lastMeasure) / 1000000;

    //Updating and computing angles
    computeGyroAngle();
    computeAccelAngle();

    //The filter itself(it can't help with Z axis)
    angles[0] = FILTER_GYRO * angles[0] + FILTER_ACCEL * accelAngleX;
    angles[1] = FILTER_GYRO * angles[1] + FILTER_ACCEL * accelAngleY;

    //TODO:magnetometer for angle Z correction, otherwise ~ 0.5degrees/minutes of drift

    //printAngles();

    lastMeasure = thisMeasure;
    //}
}

 */


/*
void MPU6050::printAngles() {

    std_out("\nAngles:");
    Serial.print("X:\t");
    std_out(angles[0]);
    Serial.print("Y:\t");
    std_out(angles[1]);
    Serial.print("Z:\t");
    std_out(angles[2]);

}

 */

/*
void MPU6050::read_bytes(uint8_t address, float* values)
{
	int16_t intvalues[3];
	read_bytes(address, intvalues);

	for (int i = 0; i < 3; i++) {
		values[i] = intvalues[i];
	}
}*/

