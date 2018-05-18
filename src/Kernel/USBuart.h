//
// Created by root on 4/29/18.
//

#ifndef TRACER_USBUART_H
#define TRACER_USBUART_H


#include <Kernel/drivers/driver.h>
#include <Kernel/scheduler/semaphore.h>
#include <WString.h>

#include "Arduino.h"


void USBUART_init(uint32_t baudrate);

//void USBUART_write(String s);


#endif //TRACER_USBUART_H
