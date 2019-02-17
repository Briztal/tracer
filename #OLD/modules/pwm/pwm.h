//
// Created by root on 11/4/18.
//

#ifndef TRACER_PWM_H
#define TRACER_PWM_H




struct pwm_config {
	
	/*
	 * Timer configuration;
	 */
	
	//Timer file name;
	const char *timer_name;
	
	//Timer init config struct;
	const void *timer_config;
	
	//Timer init config struct size;
	const size_t timer_config_size;
	
	
	/*
	 * Timer configuration;
	 */
	
	//Gpio file name;
	const char *gpio_name;
	
	//Gpio init config struct;
	const void *gpio_config;
	
	//Gpio init config struct size;
	const size_t gpio_config_size;
	
};


#endif //TRACER_PWM_H
