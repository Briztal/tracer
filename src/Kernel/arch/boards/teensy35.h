#ifndef TRACER_ARCH_TEENSY35_H
#define TRACER_ARCH_TEENSY35_H

#include "Kernel/ARCH/processors/arm_cortex_m4f.h"

#include <Kernel/drivers/UART.h>

#define arch teensy35

/*
 * --------------------------------------- Supported peripherals ---------------------------------------
 */


/*
 * The teensy35 supports 5 UARTS;
 */

extern UART_driver_t UART0;
extern UART_driver_t UART1;
extern UART_driver_t UART2;
extern UART_driver_t UART3;
extern UART_driver_t UART4;



#endif