
#include "teensy35.h"

/*
 * --------------------------------------- PORT ---------------------------------------
 */
//TODO PJRC TRIBUTE

//Define port structs addresses;
#define PORT_A_DATA     (volatile uint32_t *)0x40049000
#define PORT_B_DATA     (volatile uint32_t *)0x4004A000
#define PORT_C_DATA     (volatile uint32_t *)0x4004B000
#define PORT_D_DATA     (volatile uint32_t *)0x4004C000
#define PORT_E_DATA     (volatile uint32_t *)0x4004D000
#define GPIO_A_DATA     (volatile uint32_t *)0x400FF000
#define GPIO_B_DATA     (volatile uint32_t *)0x400FF040
#define GPIO_C_DATA     (volatile uint32_t *)0x400FF080
#define GPIO_D_DATA     (volatile uint32_t *)0x400FF0C0
#define GPIO_E_DATA     (volatile uint32_t *)0x400FF100

//Define ports;
KINETIS_PORT_DEFINE(A, PORT_A_DATA, GPIO_A_DATA);
KINETIS_PORT_DEFINE(B, PORT_B_DATA, GPIO_B_DATA);
KINETIS_PORT_DEFINE(C, PORT_C_DATA, GPIO_C_DATA);
KINETIS_PORT_DEFINE(D, PORT_D_DATA, GPIO_D_DATA);
KINETIS_PORT_DEFINE(E, PORT_E_DATA, GPIO_E_DATA);


/*
 * --------------------------------------- UARTS ---------------------------------------
 */

//Include the kinetis driver header;
#include <Kernel/arch/peripherals/kinetis/kinetis_UART.h>

//The teensy35 supports 5 UARTS;
KINETIS_UART_DEFINE(0, 0x4006A000, F_CPU, 8, 8, IRQ_UART0_STATUS, IRQ_UART0_ERROR)

/*
KINETIS_UART_DEFINE(1, 0x4006B000, F_CPU, 8, 8, IRQ_UART1_STATUS, IRQ_UART1_ERROR)
KINETIS_UART_DEFINE(2, 0x4006C000, F_BUS, 1, 1, IRQ_UART2_STATUS, IRQ_UART2_ERROR)
KINETIS_UART_DEFINE(3, 0x4006D000, F_BUS, 1, 1, IRQ_UART3_STATUS, IRQ_UART3_ERROR)
KINETIS_UART_DEFINE(4, 0x400EA000, F_BUS, 1, 1, IRQ_UART4_STATUS, IRQ_UART4_ERROR)
KINETIS_UART_DEFINE(5, 0x400EB000, F_BUS, 1, 1, IRQ_UART5_STATUS, IRQ_UART5_ERROR)

 */