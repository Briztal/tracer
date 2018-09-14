/*
  kx_mux.h Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

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


#ifndef TRACER_TEENSY_35_MUX_H
#define TRACER_TEENSY_35_MUX_H


/*
 * the k64 IO MUX configs; This is a transcription of K64 datasheet;
 */

/*
//------------------------------------------ PORT B ------------------------------------------

//------------------ PORT B, pin 12(this marker will separate pins configs from each other)

B12_GPIO 		1
B12_UART3_RTS_b 2
B12_FTM1_CH0 	3
B12_FTM0_CH4 	4
B12_FTM1_QD_PHA 6

//------------------ PORT B, pin 13

B13_PTB13		1
B13_UART3_CTS_b 2
B13_FTM1_CH1 	3
B13_FTM0_CH5 	4
B13_FTM1_QD_PHB 6


//------------------ PORT B, pin 16


B16_PTB16		1
B16_SPI1_OUT	2
B16_UART0_RX	3
B16_FTM_CLKIN0	4
B16_FB_AD17		5
B16_EWIM_IN		6


//------------------ PORT B, pin 17

B17_PTB17		1
B17_SPI1_IN		2
B17_UART0_TX	3
B17_FTM_CLKIN1	4
B17_FB_AD16		5
B17_EWIM_OUT_b	6



//------------------------------------------ PORT C ------------------------------------------

//------------------ PORT C, pin 0

C0_ADC0_SE14	0
C0_PTC0			1
C0_SPI0_PCS4	2
C0_PDB0_EXTRG	3
C0_USB_SOF_OUT	4
C0_FB_AD14		5
C0_I2S0_TXD1	6


//------------------ PORT C, pin 1

C1_ADC0_SE15	0
C1_PTC1			1
C1_LLWU_P6		1
C1_SPI0_PCS3	2
C1_UART1_RTS_b	3
C1_FTM0_CH0		4
C1_FB_AD13		5
C1_I2S0_TXD0	6


//------------------ PORT C, pin 2

C2_ADC0_SE4b	0
C2_CMP1_IN0		0
C2_PTC2			1
C2_SPI0_PCS2	2
C2_UART1_CTS_b	3
C2_FTM0_CH1		4
C2_FB_AD12		5
C2_I2S0_TX_FS	6


//------------------ PORT C, pin 3

C3_CMP1_IN1		0
C3_PTC3			1
C3_LLWU_P7		1
C3_SPI0_PCS1	2
C3_UART1_RX		3
C3_FTM0_CH2		4
C3_CLKOUT		5
C3_I2S0_BCLK	6


//------------------ PORT C, pin 4

C4_PTC4			1
C4_LLWU_P8		1
C4_SPI0_PCS0	2
C4_UART1_TX		3
C4_FTM0_CH3		4
C4_FB_AD11		5
C4_CMP1_OUT		6

//------------------ PORT C, pin 5

C5_PTC5			1
C5_LLWU_P9		1
C5_SPI0_SCK		2
C5_LPTMR0_ALT2	3
C5_I2S0_RXD0	4
C5_FB_AD10		5
C5_CMP0_OUT		6
C5_FTM0_CH2		7


//------------------ PORT C, pin 6

C6_CMP0_IN0		0
C6_PTC6			1
C6_LLWU_P10		1//TODO ERROR, CHECK;
C6_SPI0_SOUT	2
C6_PSB0_EXTRG	3
C6_I2S0_RX_BCLK	4
C6_FB_AD9		5
C6_I2S0_MCLK	6


//------------------ PORT C, pin 7

C7_CMP0_IN1		0
C7_PTC7			1
C7_SPI0_SIN		2
C7_USB_SOF_OUT	3
C7_I2S0_RX_FS	4
C7_FB_AD8		5




//------------------------------------------ PORT E ------------------------------------------

//------------------ PORT E, pin 0

E0_ADC1_SE4a	0
E0_PTE0			1
E0_SPI1_PCS1	2
E0_UART1_TX		3
E0_SDHC0_D1		4
E0_TRACE_CLKOUT	5
E0_I2C1_SDA		6
E0_RTC_CLKOUT	7


//------------------ PORT E, pin 1

E1_ADC1_SE5a	0
E1_PTE1			1
E1_LLWU_P0		1
E1_SPI1_SOUT	2
E1_UART1_RX		3
E1_SDHC0_D0		4
E1_TRACE_D3		5
E1_I2C1_SCL		6
E1_SPI1_SIN		7


//------------------ PORT E, pin 2

E2_ADC0_DP2 	0
E2_ADC0_1_SE6a	0
E2_PTE2			1//TODO ERROR CHECK
E2_LLWU_P1		1
E2_SPI1_SCK		2
E2_UART_1_CTS_b	3
E2_SDHC0_DCLK	4
E2_TRACE_D2		5


//------------------ PORT E, pin 3

E3_ADC0_DM2		0//TODO ERROR CHECK BORDEL
E3_ADC1_SE7A	0
E3_PTE3			1
E3_SPI1_SIN		2
E3_UART1_RTS_b	3
E3_SDHC0_CMD	4
E3_TRACE_D1		5
E3_SPI1_SOUT	7

//------------------ PORT E, pin 4

E4_PTE4			1//TODO ERROR CHECK
E4_LLWU_P2		1
E4_SPI1_PCS0	2
E4_UART3_TX		3
E4_SDHC0_D3		4
E4_TRACE_D0		5


//------------------ PORT E, pin 5

E5_PTE5			1
E5_SPI1_PCS2	2
E5_UART3_RX		3
E5_SDHC0_D2		4
E5_FTM3_CH0		6


//------------------ PORT E, pin 6

E6_PTE6			1
E6_SPI1_PCS3	2
E6_UART3_CTS_b	3
E6_I2S0_MCLK	4
E6_FTM3_CH1		6
E6_USB_SOF_OUT	7


//------------------ PORT E, pin 7

E7_PTE7			1
E7_UART3_RTS_b	3
E7_I2S0_RXD0	4
E7_FTM3_CH2		6


//------------------ PORT E, pin 8

E8_PTE8			1
E8_I2S0_RXD1	2
E8_UART5_TX		3
E8_I2S0_RX_FS	4
E8_FTM3_CH3		6


//------------------ PORT E, pin 9

E9_I2S0_TXD1	1
E9_PTE9			2
E9_UART5_RX		3
E9_I2S0_RX_BCLK	4
E9_FTM3_CH4		6


//------------------ PORT E, pin 10

E10_PTE10		1
E10_UART5_CTS_b	3
E10_I2S0_TXD0	4
E10_FTM3_CH5	6


//------------------ PORT E, pin 11

E11_PTE11		1
E11_UART5_RTS_b	3
E11_I2S0_TX_FS	4
E11_FTM3_CH6	6


//------------------ PORT E, pin 12

E12_PTE12			1
E12_I2S0_TX_BCLK	4
E12_FTM3_CH7		6

 */


#endif //TRACER_TEENSY_35_MUX_H
