//
// Created by root on 8/20/18.
//

#ifndef TRACER_IC_H
#define TRACER_IC_H

//-------------------------------------------------- Critical sections -------------------------------------------------

//Enter a critical section;
void ic_enter_critical_section();

//Leave a critical section;
void ic_leave_critical_section();

//Force leaving a critical section. Unsafe, must be executed by the kernel only;
void ic_force_critical_section_exit();


//--------------------------------------------------- Implementation ---------------------------------------------------

//The INTERRUPT CONTROLLER HEADER macro is definedby the core, and leads to the file where are ic statics and headers;
#include INTERRUPT_CONTROLLER_HEADER


#endif //TRACER_IC_H
