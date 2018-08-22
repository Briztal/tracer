//
// Created by root on 8/22/18.
//


#include <stdint.h>
#include <stddef.h>

//BaudRate bounds
#define SWUART_BR_MAX 1000000
#define SWUART_BR_MIN 160


//Determine the bit time in microseconds for a given BaudRate; BT = 10^6 / BR
//1 < bit_time < 65535  =>  152 < BaudRate < 1000000
uint16_t sw_uart_get_bit_time(uint32_t baudrate);

//Encode a char to an array of durations;
void sw_uart_send(uint16_t bit_time, char c);
