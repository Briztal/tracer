//
// Created by root on 4/29/18.
//

#ifndef TRACER_USBUART_H
#define TRACER_USBUART_H


#include <Kernel/Drivers/Driver.h>
#include <Kernel/Scheduler/Semaphore.h>

namespace USBuart {

    void init(uint32_t baudrate);

    void write(String s);

};



#endif //TRACER_USBUART_H
