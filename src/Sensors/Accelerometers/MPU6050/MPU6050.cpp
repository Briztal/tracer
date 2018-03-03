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
 * Constructor : initialises data arrays;
 */

MPU6050::MPU6050() : accelerometer_data(new int16_t[3]), gyrometer_data(new int16_t[3]) {

    //Initialise the hardware;
    initialise_hardware();

    //Initialise fields;
    initialise_data();

}


/*
 * Destructor : deletes all dynamic fields (data arrays);
 */

MPU6050::~MPU6050() {

    //Delete the accelerometer array;
    delete[] accelerometer_data;

    //Delete the accelerometer array;
    delete[] gyrometer_data;
}


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

    //Reset the accelerometer array;
    memset(accelerometer_data, 0, 3 * sizeof(int16_t));

    //Reset the gyrometer array;
    memset(gyrometer_data, 0, 3 * sizeof(int16_t));


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
    size_t bytes_received = Wire.requestFrom(MPU_ADDRESS, nb_bytes, I2C_STOP, MPU6050_TIMEOUT_US);

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
 * write_bits : Write the [length] first bits of [data] in [address] in the chip, offseted of [offset];
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


/*
 * get_accelerometer_data : gets both accelerometer and gyrometer data;
 */

void MPU6050::compute_data() {

    //Compute acceleration values;
    compute_accelerometer_data();

    //Read gyrometer values;
    compute_gyrometer_data();
}


/*
 * compute_accelerometer_data : this function queries the accelerometer data;
 */

void MPU6050::compute_accelerometer_data() {

    //Read acceleration values;
    read_int16s(MPU_ACCEL_MEASURE, 6, accelerometer_data);

}


/*
 * compute_gyrometer_data : this function queries the accelerometer data;
 */

void MPU6050::compute_gyrometer_data() {

    //Read gyrometer values;
    read_int16s(MPU_GYRO_MEASURE, 6, gyrometer_data);

}


//------------------------------------------------- Data transmission --------------------------------------------------

/*
 * get_acceleration_values : this function returns acceleration coordinates from caches in the class;
 */

void MPU6050::get_accelerometer_values(int16_t *acc_x, int16_t *acc_y, int16_t *acc_z) {

    //Simply update the three variables;
    *acc_x = accelerometer_data[0];
    *acc_y = accelerometer_data[1];
    *acc_z = accelerometer_data[2];

}


/*
 * get_acceleration_array: this function copies the acceleration array ;
 */

void MPU6050::get_accelerometer_array(int16_t *array) {

    //Fast copy;
    memcpy(array, accelerometer_data, 3 * sizeof(int16_t));

}


/*
 * get_acceleration_values : this function returns acceleration coordinates from caches in the class;
 */

void MPU6050::get_gyrometer_values(int16_t *acc_x, int16_t *acc_y, int16_t *acc_z) {

    //Simply update the three variables;
    *acc_x = gyrometer_data[0];
    *acc_y = gyrometer_data[1];
    *acc_z = gyrometer_data[2];

}


/*
 * get_acceleration_array: this function copies the acceleration array ;
 */

void MPU6050::get_gyrometer_array(int16_t *array) {

    //Fast copy;
    memcpy(array, gyrometer_data, 3 * sizeof(int16_t));

}
