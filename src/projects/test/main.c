//
// Created by root on 10/13/18.
//

#include <kernel/debug/log.h>
#include <kernel/debug/debug.h>
#include <kernel/sched/sched.h>


void kernel_second_process(void *args, size_t args_size){
	
	while(1) {
		
		uint32_t a = 2000;
		
		const char *st = "BB %h";
		
		kernel_log(st, a);
		
		debug_delay_ms(a);
		
	}
	
}


void __kernel_first_function(void *args, size_t args_size) {
	
	struct prc_desc desc = {
		.function = &kernel_second_process,
		.args_size = 0,
		.args = 0,
	};
	
	struct prc_req req = {
		.stack_size = 2048,
		.activity_time = 3000,
		.ram_size = 4096
	};
	
	sched_create_prc(&desc, &req);
	
	while(1) {
		
		uint32_t a = 500;
		
		const char *st = "SUUS %h";
		
		kernel_log(st, a);
		
		debug_delay_ms(a);
		
	}
	
}