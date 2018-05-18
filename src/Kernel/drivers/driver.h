//
// Created by root on 5/16/18.
//

#ifndef TRACER_DRIVER_H
#define TRACER_DRIVER_H


/*
 * The transmission configuration structure;
 */


typedef enum {

    UART,

} driver_type_t;


/*
 * The driver struct. It is the base for all driver configurations.
 *
 *  It defines the driver type;
 */

typedef struct {

    //The driver type;
    driver_type_t driver_type;

} driver_config_t;


//The constructor for the driver configuration;
#define DRIVER_CONFIG(type) {.driver_type = (type)}


typedef struct {

    //Data structure;
    void *data;

    //Driver initialisation;
    void (*initialise)(void *data, driver_config_t *);

    //De-initialise the UART;
    void (*exit)(void *data);


} driver_t;



/*
 * An inline function to initialise the driver; Eases the redaction;
 */

inline void driver_init(driver_t *driver, driver_config_t *config) {

    //Initialise the driver;
    driver->initialise(driver->data, config);

}


/*
 * An inline function to initialise the driver; Eases the redaction;
 */

inline void driver_exit(driver_t *driver) {

    //Initialise the driver;
    driver->exit(driver->data);

}


#endif //TRACER_DRIVER_H
