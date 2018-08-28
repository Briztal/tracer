//
// Created by root on 7/23/18.
//

#ifndef TRACER_TEENSY_35_MUX_H
#define TRACER_TEENSY_35_MUX_H


/*
 * the teensy35's port idendifiers;
 */

#define PORT_A 0
#define PORT_B 1
#define PORT_C 2
#define PORT_D 3
#define PORT_E 4


/*
 * the teensy35's IO MUX configs; This is a transcription of K64 datasheet;
 */


//------------------------------------------ PORT B ------------------------------------------


//------------------ PORT B, pin 12(this marker will separate pins configs from each other)

#define PTB12 			{PORT_B, 12, 1};
#define UART3_RTS_b  	{PORT_B, 12, 2};
#define FTM1_CH0 	 	{PORT_B, 12, 3};
#define FTM0_CH4 	 	{PORT_B, 12, 4};
#define FTM1_QD_PHA 	{PORT_B, 12, 6};

//------------------ PORT B, pin 13

#define PTB13			{PORT_B, 13, 1};
#define UART3_CTS_b  	{PORT_B, 13, 2};
#define FTM1_CH1 	 	{PORT_B, 13, 3};
#define FTM0_CH5 	 	{PORT_B, 13, 4};
#define FTM1_QD_PHB 	{PORT_B, 13, 6};


//------------------ PORT B, pin 16


#define PTB16			{PORT_B, 16, 1};
#define SPI1_OUT		{PORT_B, 16, 2};
#define UART0_RX		{PORT_B, 16, 3};
#define FTM_CLKIN0		{PORT_B, 16, 4};
#define FB_AD17			{PORT_B, 16, 5};
#define EWIM_IN			{PORT_B, 16, 6};


//------------------ PORT B, pin 17

#define PTB17			{PORT_B, 17, 1};
#define SPI1_IN			{PORT_B, 17, 2};
#define UART0_TX		{PORT_B, 17, 3};
#define FTM_CLKIN1		{PORT_B, 17, 4};
#define FB_AD16			{PORT_B, 17, 5};
#define EWIM_OUT_b		{PORT_B, 17, 6};



//------------------------------------------ PORT C ------------------------------------------

//------------------ PORT C, pin 0

#define ADC0_SE14		{PORT_C, 0, 0};
#define PTC0			{PORT_C, 0, 1};
#define SPI0_PCS4		{PORT_C, 0, 2};
#define PDB0_EXTRG		{PORT_C, 0, 3};
#define USB_SOF_OUT		{PORT_C, 0, 4};
#define FB_AD14			{PORT_C, 0, 5};
#define I2S0_TXD1		{PORT_C, 0, 6};


//------------------ PORT C, pin 1

#define ADC0_SE15		{PORT_C, 1, 0};
#define PTC1			{PORT_C, 1, 1};
#define LLWU_P6			{PORT_C, 1, 1};
#define SPI0_PCS3		{PORT_C, 1, 2};
#define UART1_RTS_b		{PORT_C, 1, 3};
#define FTM0_CH0		{PORT_C, 1, 4};
#define FB_AD13			{PORT_C, 1, 5};
#define I2S0_TXD0		{PORT_C, 1, 6};


//------------------ PORT C, pin 2

#define ADC0_SE4b		{PORT_C, 2, 0};
#define CMP1_IN0		{PORT_C, 2, 0};
#define PTC2			{PORT_C, 2, 1};
#define SPI0_PCS2		{PORT_C, 2, 2};
#define UART1_CTS_b		{PORT_C, 2, 3};
#define FTM0_CH1		{PORT_C, 2, 4};
#define FB_AD12			{PORT_C, 2, 5};
#define I2S0_TX_FS		{PORT_C, 2, 6};


//------------------ PORT C, pin 3

#define CMP1_IN1		{PORT_C, 3, 0};
#define PTC3			{PORT_C, 3, 1};
#define LLWU_P7			{PORT_C, 3, 1};
#define SPI0_PCS1		{PORT_C, 3, 2};
//#define UART1_RX		{PORT_C, 3, 3};
#define FTM0_CH2		{PORT_C, 3, 4};
#define CLKOUT			{PORT_C, 3, 5};
#define I2S0_BCLK		{PORT_C, 3, 6};


//------------------ PORT C, pin 4

#define PTC4			{PORT_C, 4, 1};
#define LLWU_P8			{PORT_C, 4, 1};
#define SPI0_PCS0		{PORT_C, 4, 2};
//#define UART1_TX		{PORT_C, 4, 3};
#define FTM0_CH3		{PORT_C, 4, 4};
#define FB_AD11			{PORT_C, 4, 5};
#define CMP1_OUT		{PORT_C, 4, 6};


//------------------ PORT C, pin 5

#define PTC5			{PORT_C, 5, 1};
#define LLWU_P9			{PORT_C, 5, 1};
#define SPI0_SCK		{PORT_C, 5, 2};
#define LPTMR0_ALT2		{PORT_C, 5, 3};
#define I2S0_RXD0		{PORT_C, 5, 4};
#define FB_AD10			{PORT_C, 5, 5};
#define CMP0_OUT		{PORT_C, 5, 6};
//TODO#define FTM0_CH2		{PORT_C, 5, 7};


//------------------ PORT C, pin 6

#define CMP0_IN0		{PORT_C, 6, 0};
#define PTC6			{PORT_C, 6, 1};
#define LLWU_P10		{PORT_C, 6, 1};
#define SPI0_SOUT		{PORT_C, 6, 2};
#define PSB0_EXTRG		{PORT_C, 6, 3};
#define I2S0_RX_BCLK	{PORT_C, 6, 4};
#define FB_AD9			{PORT_C, 6, 5};
#define I2S0_MCLK		{PORT_C, 6, 6};


//------------------ PORT C, pin 7

#define CMP0_IN1		{PORT_C, 7, 0};
#define PTC7			{PORT_C, 7, 1};
#define SPI0_SIN		{PORT_C, 7, 2};
//TODO#define USB_SOF_OUT		{PORT_C, 7, 3};
#define I2S0_RX_FS		{PORT_C, 7, 4};
#define FB_AD8			{PORT_C, 7, 5};

/*
 * TODO
#define 		{PORT_C, , 0};
#define 		{PORT_C, , 1};
#define 		{PORT_C, , 2};
#define 		{PORT_C, , 3};
#define 		{PORT_C, , 4};
#define 		{PORT_C, , 5};
#define 		{PORT_C, , 6};
#define 		{PORT_C, , 7};
 */



//------------------------------------------ PORT E ------------------------------------------

//------------------ PORT E, pin 0

#define ADC1_SE4a		{PORT_E, 0, 0};
#define PTE0			{PORT_E, 0, 1};
#define SPI1_PCS1		{PORT_E, 0, 2};
#define UART1_TX		{PORT_E, 0, 3};
#define SDHC0_D1		{PORT_E, 0, 4};
#define TRACE_CLKOUT	{PORT_E, 0, 5};
#define I2C1_SDA		{PORT_E, 0, 6};
#define RTC_CLKOUT		{PORT_E, 0, 7};


//------------------ PORT E, pin 1

#define ADC1_SE5a		{PORT_E, 1, 0};
#define PTE1			{PORT_E, 1, 1};
#define LLWU_P0			{PORT_E, 1, 1};
#define SPI1_SOUT		{PORT_E, 1, 2};
#define UART1_RX		{PORT_E, 1, 3};
#define SDHC0_D0		{PORT_E, 1, 4};
#define TRACE_D3		{PORT_E, 1, 5};
#define I2C1_SCL		{PORT_E, 1, 6};
#define SPI1_SIN		{PORT_E, 1, 7};


//------------------ PORT E, pin 2

#define ADC0_DP2 		{PORT_E, 2, 0};
#define ADC0_1_SE6a		{PORT_E, 2, 0};
#define PTE2			{PORT_E, 2, 1};
#define LLWU_P1			{PORT_E, 2, 1};
#define SPI1_SCK		{PORT_E, 2, 2};
#define UART_1_CTS_b	{PORT_E, 2, 3};
#define SDHC0_DCLK		{PORT_E, 2, 4};
#define TRACE_D2		{PORT_E, 2, 5};


//------------------ PORT E, pin 3

#define ADC0_DM2		{PORT_E, 3, 0};
#define ADC1_SE7A		{PORT_E, 3, 0};
#define PTE3			{PORT_E, 3, 1};
//TODO#define SPI1_SIN		{PORT_E, 3, 2};
//TODO##define UART1_RTS_b		{PORT_E, 3, 3};
#define SDHC0_CMD		{PORT_E, 3, 4};
#define TRACE_D1		{PORT_E, 3, 5};
//TODO##define SPI1_SOUT		{PORT_E, 3, 7};

//------------------ PORT E, pin 4

#define PTE4			{PORT_E, 4, 1};
#define LLWU_P2			{PORT_E, 4, 1};
#define SPI1_PCS0		{PORT_E, 4, 2};
#define UART3_TX		{PORT_E, 4, 3};
#define SDHC0_D3		{PORT_E, 4, 4};
#define TRACE_D0		{PORT_E, 4, 5};


//------------------ PORT E, pin 5

#define PTE5			{PORT_E, 5, 1};
#define SPI1_PCS2		{PORT_E, 5, 2};
#define UART3_RX		{PORT_E, 5, 3};
#define SDHC0_D2		{PORT_E, 5, 4};
#define FTM3_CH0		{PORT_E, 5, 6};


//------------------ PORT E, pin 6

#define PTE6			{PORT_E, 6, 1};
#define SPI1_PCS3		{PORT_E, 6, 2};
//TODO##define UART3_CTS_b		{PORT_E, 6, 3};
//TODO##define I2S0_MCLK		{PORT_E, 6, 4};
#define FTM3_CH1		{PORT_E, 6, 6};
//TODO##define USB_SOF_OUT		{PORT_E, 6, 7};


//------------------ PORT E, pin 7

#define PTE7			{PORT_E, 7, 1};
//TODO##define UART3_RTS_b		{PORT_E, 7, 3};
//TODO##define I2S0_RXD0		{PORT_E, 7, 4};
#define FTM3_CH2		{PORT_E, 7, 6};


//------------------ PORT E, pin 8

#define PTE8			{PORT_E, 8, 1};
#define I2S0_RXD1		{PORT_E, 8, 2};
#define UART5_TX		{PORT_E, 8, 3};
//TODO##define I2S0_RX_FS		{PORT_E, 8, 4};
#define FTM3_CH3		{PORT_E, 8, 6};


//------------------ PORT E, pin 9

//TODO##define I2S0_TXD1		{PORT_E, 9, 1};
#define PTE9			{PORT_E, 9, 2};
#define UART5_RX		{PORT_E, 9, 3};
//TODO##define I2S0_RX_BCLK	{PORT_E, 9, 4};
#define FTM3_CH4		{PORT_E, 9, 6};


//------------------ PORT E, pin 10

#define PTE10			{PORT_E, 10, 1};
#define UART5_CTS_b		{PORT_E, 10, 3};
//TODO##define I2S0_TXD0		{PORT_E, 10, 4};
#define FTM3_CH5		{PORT_E, 10, 6};


//------------------ PORT E, pin 11

#define PTE11			{PORT_E, 11, 1};
#define UART5_RTS_b		{PORT_E, 11, 3};
//TODO##define I2S0_TX_FS		{PORT_E, 11, 4};
#define FTM3_CH6		{PORT_E, 11, 6};


//------------------ PORT E, pin 12

#define PTE12			{PORT_E, 12, 1};
#define I2S0_TX_BCLK	{PORT_E, 12, 4};
#define FTM3_CH7		{PORT_E, 12, 6};

/*
#define 		{PORT_E, , 0};
#define 		{PORT_E, , 1};
#define 		{PORT_E, , 2};
#define 		{PORT_E, , 3};
#define 		{PORT_E, , 4};
#define 		{PORT_E, , 5};
#define 		{PORT_E, , 6};
#define 		{PORT_E, , 7};

#define 		{PORT_E, , 0};
#define 		{PORT_E, , 1};
#define 		{PORT_E, , 2};
#define 		{PORT_E, , 3};
#define 		{PORT_E, , 4};
#define 		{PORT_E, , 5};
#define 		{PORT_E, , 6};
#define 		{PORT_E, , 7};
 */


#endif //TRACER_TEENSY_35_MUX_H
