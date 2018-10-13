//
// Created by root on 10/13/18.
//

#include <kernel/debug/log.h>
#include <kernel/debug/debug.h>

void __kernel_first_function() {
	
	while(1) {
		
		kernel_log_("First_function");
		
		debug_delay_ms(1000);
		
	}
	
}