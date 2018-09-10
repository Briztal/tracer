//
// Created by root on 9/10/18.
//

#include <kernel/debug/debug.h>
#include "stdout.h"

#include "string.h"

#define SEPARATOR '%'


void stdout_str(void (*print)(char), const char *msg) {

	//Cache the current char;
	char c;

	//While the current char is not null;
	while ((c = *(msg++))) {

		//Send it;
		(*print)(c);

		//Debug
		print('X');

	}



}


//Convert an integer to its string expression in the required base, and send it;
void stdout_int(void (*print)(char), uint32_t integer, uint8_t base) {

	//Cache an array of the appropriate length;
	char t[32];

	debug_log("");
	debug_log_hex(t);

	debug_log_hex(t + 32);

	//Convert the integer;
	char *str = itoa(integer, t, 32, base);
	debug_log_hex(str);



	//Log the value;
	stdout_str(print, str);

	//Debug
	print('X');

}


void stdout(void (*print)(char), const char *str, const uint32_t *args, size_t args_size) {

	//The current char;
	char c;

	//The current state; set if the previous char was a separator;
	bool argument_char = false;

	//While the current char is not a null termination :
	while ((c = (*(str++)))) {

		if (argument_char) {

			//Reset the flag;
			argument_char = false;

			//If the char is a separator :
			if (c == SEPARATOR) {

				//Print the separator;
				(*print)(SEPARATOR);

			} else {

				//If we have no more arguments to process, continue;
				if (!args_size) {
					break;
				}

				//Evaluate the argument type :
				switch (c) {

					//Binary value;
					case 'b':
						stdout_int(print, *args, 2);
						break;

						//Octal value;
					case 'o':
						stdout_int(print, *args, 8);
						break;

						//Decimal value;
					case 'd':
						stdout_int(print, *args, 10);

						//Debug
						print('V');
						break;

						//Hexadecimal value;
					case 'h':
						stdout_int(print, *args, 16);
						break;

					default:
						break;

				}

				/*

				//Update the current arg, and decrease the number of arguments;
				args++;
				args_size--;
				 */

			}

		} else {

			//If the current char is a separator :
			if (c == SEPARATOR) {

				//Set the argument char is set;
				argument_char = true;

			} else {

				//If the current char is regular, print it;
				(*print)(c);
			}

		}

	}

	//Print a newline and carriage return;
	(*print)('\n');
	(*print)('\r');

}
