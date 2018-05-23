//
// Created by root on 4/29/18.
//

/*
#include <Kernel/Scheduler/systick.h>

#include "USBuart.h"

namespace USBuart {

    Semaphore semaphore(1);

}

void USBuart::init(uint32_t baudrate) {

    Serial.begin((long)baudrate);

}

void USBuart::write(String s) {

    semaphore.P();

    Serial.println(s);

    semaphore.V();

}
*/