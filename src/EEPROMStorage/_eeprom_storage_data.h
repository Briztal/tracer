
#ifndef TRACER_EEPROM_STORAGE_DATA_H
#define TRACER_EEPROM_STORAGE_DATA_H


#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL

//The stepper data structure : contains all data related to a stepper motor
typedef struct stepper_data_t {
    float size;
    float steps;
    float maximum_speed;
    float acceleration;
    float jerk;
} stepper_data_t;


//

#define COORD_INTERFACE_VARIABLE(name, default_value) float name = default_value;

//The coordinate interface structure : contains all persistent data required in config_files.h
typedef struct coordinate_interface_data_t {

#include <config.h>

} coordinate_interface_data_t;

#undef COORD_INTERFACE_VARIABLE

#endif


#ifdef ENABLE_ASSERV

typedef struct pid_data_t {
    float kp;
    float ki;
    float kd;
} pid_data_t;

#endif



typedef struct continuous_data_t {
    float max;
} continuous_data_t;



typedef struct servo_data_t {
    float min;
    float max;
} servo_data_t;


//Custom Variables

//Custom data
#define EEPROM_BOOL(name, default_value) static bool name;
#define EEPROM_CHAR(name, default_value) static char name;
#define EEPROM_INT(name, default_value) static int name;
#define EEPROM_LONG(name, default_value) static int32_t name;
#define EEPROM_FLOAT(name, default_value) static float name;

typedef struct custom_data_t {
#include "../config.h"
} custom_data_t;

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

#endif //TRACER_EEPROM_STORAGE_DATA_H
