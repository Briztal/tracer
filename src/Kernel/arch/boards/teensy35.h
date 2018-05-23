#ifndef TRACER_ARCH_TEENSY35_H
#define TRACER_ARCH_TEENSY35_H

#include "Kernel/arch/processors/arm_cortex_m4f.h"



#define arch teensy35


/*
 * --------------------------------------- GPIO ---------------------------------------
 */

//We will use the kinetis GPIO module;
#include "Kernel/arch/peripherals/kinetis/kinetis_GPIO.h"


/*
 * --------------------------------------- UARTS ---------------------------------------
 */

//To define UART drivers we must include the UART driver header;
#include <Kernel/drivers/UART.h>

/*
 * The teensy35 supports 5 UARTS;
 */

UART_DRIVER_DECLARE(0);
UART_DRIVER_DECLARE(1);
UART_DRIVER_DECLARE(2);
UART_DRIVER_DECLARE(3);
UART_DRIVER_DECLARE(4);
UART_DRIVER_DECLARE(5);

#endif