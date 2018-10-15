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
	
	//sched_create_prc(&desc, &req);
	
	while(1) {
		
		uint32_t msp;
		uint32_t psp;
		uint32_t sp;
		uint32_t psr;
		uint32_t ctrl;
		
		__asm__ __volatile__ ("mrs %0, msp" : "=r" (msp):);
		__asm__ __volatile__ ("mrs %0, psp" : "=r" (psp):);
		__asm__ __volatile__ ("mov %0, r13" : "=r" (sp):);
		__asm__ __volatile__ ("mrs %0, psr" : "=r" (psr):);
		__asm__ __volatile__ ("mrs %0, control" : "=r" (ctrl):);
		
		kernel_log("msp : %h , psp : %h, sp : %h, psr : %h, ctrl : %h", msp, psp, sp, psr, ctrl);
		
		debug_delay_ms(1000);
		
		//__preemption_set_pending();
		
		
	}
	
}