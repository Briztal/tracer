#include "teensy35.h"

/*
 * The teensy35 supports 5 UARTS;
 */

/*
 * The instantiation of an UART requires a bit of work, that is the same for all UARTs. A Macro will simplify our work;
 */

#define INSTANTIATE_UART_DRIVERS(i, address, frequency, rxsize, txsize, status_index, error_index)\
    teensy35::UARTRegisters *reg##i = ((teensy35::UARTRegisters *) (address));\
    teensy35::UARTData UARTData##i{reg##i, (frequency)};\
    void status_link_##i() {UART##i->interrupt();}\
    void error_link_##i() {UART##i->error();}\
    teensy35::UART *UART##i = new teensy35::UART(&UARTData##i, rxsize, txsize, status_index, error_index, status_link_##i, error_link_##i);


/*
 * Now, we can instantiate all our drivers
 */

/*
INSTANTIATE_UART_DRIVERS(0, 0x4006A000, F_CPU, teensy35::size_8, teensy35::size_8, IRQ_UART0_STATUS, IRQ_UART0_ERROR);
INSTANTIATE_UART_DRIVERS(1, 0x4006B000, F_CPU, teensy35::size_1, teensy35::size_1, IRQ_UART1_STATUS, IRQ_UART1_ERROR);
INSTANTIATE_UART_DRIVERS(2, 0x4006C000, F_BUS, teensy35::size_1, teensy35::size_1, IRQ_UART2_STATUS, IRQ_UART2_ERROR);
INSTANTIATE_UART_DRIVERS(3, 0x4006D000, F_BUS, teensy35::size_1, teensy35::size_1, IRQ_UART3_STATUS, IRQ_UART3_ERROR);
INSTANTIATE_UART_DRIVERS(4, 0x400EA000, F_BUS, teensy35::size_1, teensy35::size_1, IRQ_UART4_STATUS, IRQ_UART4_ERROR);
INSTANTIATE_UART_DRIVERS(5, 0x400EB000, F_BUS, teensy35::size_1, teensy35::size_1, IRQ_UART5_STATUS, IRQ_UART5_ERROR);
*/