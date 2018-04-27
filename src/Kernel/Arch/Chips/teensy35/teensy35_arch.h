#define arch teensy35


/*
 * The Interrupt supported by the Teensy35;
 */

/*

enum IRQ_NUMBER_t {
    IRQ_DMA_CH0 =		0,
    IRQ_DMA_CH1 =		1,
    IRQ_DMA_CH2 =		2,
    IRQ_DMA_CH3 =		3,
    IRQ_DMA_CH4 =		4,
    IRQ_DMA_CH5 =		5,
    IRQ_DMA_CH6 =		6,
    IRQ_DMA_CH7 =		7,
    IRQ_DMA_CH8 =		8,
    IRQ_DMA_CH9 =		9,
    IRQ_DMA_CH10 =		10,
    IRQ_DMA_CH11 =		11,
    IRQ_DMA_CH12 =		12,
    IRQ_DMA_CH13 =		13,
    IRQ_DMA_CH14 =		14,
    IRQ_DMA_CH15 =		15,
    IRQ_DMA_ERROR =		16,
    IRQ_MCM =		17,
    IRQ_FTFL_COMPLETE =	18,
    IRQ_FTFL_COLLISION =	19,
    IRQ_LOW_VOLTAGE =	20,
    IRQ_LLWU =		21,
    IRQ_WDOG =		22,
    IRQ_RNG =		23,
    IRQ_I2C0 =		24,
    IRQ_I2C1 =		25,
    IRQ_SPI0 =		26,
    IRQ_SPI1 =		27,
    IRQ_I2S0_TX =		28,
    IRQ_I2S0_RX =		29,
    IRQ_UART0_STATUS =	31,
    IRQ_UART0_ERROR =	32,
    IRQ_UART1_STATUS =	33,
    IRQ_UART1_ERROR =	34,
    IRQ_UART2_STATUS =	35,
    IRQ_UART2_ERROR =	36,
    IRQ_UART3_STATUS =	37,
    IRQ_UART3_ERROR =	38,
    IRQ_ADC0 =		39,
    IRQ_CMP0 =		40,
    IRQ_CMP1 =		41,
    IRQ_FTM0 =		42,
    IRQ_FTM1 =		43,
    IRQ_FTM2 =		44,
    IRQ_CMT =		45,
    IRQ_RTC_ALARM =		46,
    IRQ_RTC_SECOND =	47,
    IRQ_PIT_CH0 =		48,
    IRQ_PIT_CH1 =		49,
    IRQ_PIT_CH2 =		50,
    IRQ_PIT_CH3 =		51,
    IRQ_PDB =		52,
    IRQ_USBOTG =		53,
    IRQ_USBDCD =		54,
    IRQ_DAC0 =		56,
    IRQ_MCG =		57,
    IRQ_LPTMR =		58,
    IRQ_PORTA =		59,
    IRQ_PORTB =		60,
    IRQ_PORTC =		61,
    IRQ_PORTD =		62,
    IRQ_PORTE =		63,
    IRQ_SOFTWARE =		64,
    IRQ_SPI2 =		65,
    IRQ_UART4_STATUS =	66,
    IRQ_UART4_ERROR =	67,
    IRQ_UART5_STATUS =	68,
    IRQ_UART5_ERROR =	69,
    IRQ_CMP2 =		70,
    IRQ_FTM3 =		71,
    IRQ_DAC1 =		72,
    IRQ_ADC1 =		73,
    IRQ_I2C2 =		74,
    IRQ_CAN0_MESSAGE =	75,
    IRQ_CAN0_BUS_OFF =	76,
    IRQ_CAN0_ERROR =	77,
    IRQ_CAN0_TX_WARN =	78,
    IRQ_CAN0_RX_WARN =	79,
    IRQ_CAN0_WAKEUP =	80,
    IRQ_SDHC =		81,
    IRQ_ENET_TIMER =	82,
    IRQ_ENET_TX =		83,
    IRQ_ENET_RX =		84,
    IRQ_ENET_ERROR =	85

};

*/
/*
 * --------------------------------------- Supported peripherals ---------------------------------------
 */

//The teensy's hardware comprises UARTs
#define UART_SUPPORTED
