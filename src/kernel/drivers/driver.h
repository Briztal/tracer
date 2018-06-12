/*
  driver.h Part of TRACER

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
    void (*initialise)(void *data, const driver_config_t *);

    //De-initialise the UART;
    void (*exit)(const void *data);

} driver_t;

#define DRIVER(data_p, init_p, exit_p) {.data = (data_p), .initialise = (init_p), .exit = (exit_p)}


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
