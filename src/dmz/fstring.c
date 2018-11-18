//
// Created by root on 9/10/18.
//


#include <stdint.h>

#include <string.h>

#include "../../dmz/string.h"


#define SEPARATOR '%'

static const char separator_v = SEPARATOR;
static const char true_v = '1';
static const char false_v = '0';

static void str_decode(void (*exec)(void *arg, const char *data, size_t size), void *arg,  const char *const msg) {
	
	const size_t size = strlen(msg);
	
	(*exec)(arg, msg, size);

}


//Convert an integer to its string expression in the required base, and send it;
static void int_decode(void (*exec)(void *arg, const char *data, size_t size), void *arg, uint32_t integer, uint8_t base) {

	//Cache an array of the appropriate length;
	char t[33];

	//Convert the integer;
	char *str = itoa(integer, t, 32, base);

	//Log the value;
	str_decode(exec, arg, str);

}


void fdecode(void (*exec)(void *arg, const char *data, size_t size), void *arg, const char *str, const void **args, size_t args_size) {

	//The current char;
	char c;

	//The current state; set if the previous char was a separator;
	bool argument_char = false;

	//The current message block;
	const char *block = str;
	size_t bsize = 0;
	
	//While the current char is not a null termination :
	while ((c = (*(str++)))) {

		if (argument_char) {

			//Reset the flag;
			argument_char = false;

			//If the char is a separator :
			if (c == SEPARATOR) {

				//Print the separator;
				(*exec)(arg, &separator_v, 1);

			} else {

				//If we have no more arguments to process, continue;
				if (!args_size) {
					continue;
				}

				//Evaluate the argument type :
				switch (c) {

					//boolean value;
					case '?':
						(*exec)(arg, (*args) ? &true_v : &false_v, 1);
						break;

						//Binary value;
					case 'b':
						int_decode(exec, arg, (uint32_t) * args, 2);
						break;

						//Octal value;
					case 'o':
						int_decode(exec, arg, (uint32_t) * args, 8);
						break;

						//Decimal value;
					case 'd':
						int_decode(exec, arg, (uint32_t) * args, 10);

						break;

						//Hexadecimal value;
					case 'h':
						int_decode(exec, arg, (uint32_t) * args, 16);
						break;


						//String insertion;
					case 's':
						str_decode(exec, arg, (char *) *args);
						break;

					default:
						break;

				}

				//Update the current arg, and decrease the number of arguments;
				args++;
				args_size--;
				
				//Update the message block;
				block = str;
				bsize = 0;

			}

		} else {

			//If the current char is a separator :
			if (c == SEPARATOR) {

				//Set the argument char is set;
				argument_char = true;
				
				//Print the current block;
				(*exec)(arg, block, bsize);

			} else {

				//Add the current char to the current block;
				bsize++;
			}

		}

	}

}
