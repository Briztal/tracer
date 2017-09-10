/*
  TerminalInterface.cpp - Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <config.h>

#ifdef ENABLE_TERMINAL_INTERFACE


#include "TerminalInterface.h"
#include "TerminalInterfaceCommands.h"
#include "../../hardware_language_abstraction.h"


/*
 * begin : this function initialises the serial, and sets up the command processing environment.
 *
 */
void UI::begin() {

    //Initialise the serial
    serial_begin(BAUDRATE);

    //Wait for the serial to  correctly initialise
    delay_ms(10);

    //Turn on the led
    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);

    //Setup a correct command environment
    reset();

}


/*
 * echo : this function sends a String over the serial.
 *
 */

void TerminalInterface::echo(const string_t msg) {
    terminal_interface_link_t::send_str(msg + "\n");
}

/*
 * read_serial : this function reads and saved the serial available data.
 *
 */

void TerminalInterface::read_serial() {

    while (terminal_interface_link_t::available()) {

        //Read the serial
        char read_char = terminal_interface_link_t::read();

        //If the recieved char is a line feed or a carriage return
        if ((read_char == 10) || (read_char == 13)) {

            //If a char has effectively been received
            if (command_size) {

                //Parse and execute the command
                execute();

                //Reset the data_in
                reset();

            }

        } else {

            //Append the read_output char to data_in
            *(data_in++) = read_char;
            command_size++;
        }
    }
}


/*
 * reset : this function resets the command reception environment
 */

void TerminalInterface::reset() {
    command_size = 0;
    data_in = data_in_0;
}


/*
 * get_next_word : this function extracts the next word of the recieved command, and saves it in word_buffer.
 *
 */

unsigned char TerminalInterface::get_next_word() {

    word_buffer = word_buffer_0;

    do {
        //Stop the iteration if no more chars are available
        if (!command_size--) {
            *word_buffer = 0;
            return 0;
        }
        //copy the current char and re-iterate if it is a space.
    } while ((*word_buffer = *(data_in++)) == ' ');

    //One non-space char has been added, so increment the size and the word buffer
    unsigned char size = 1;
    word_buffer++;

    //local for the current char
    char t;

    //While there still are chars to process, and that the current char is not a space
    while ((command_size) && ((t = *(data_in++)) != ' ')) {

        //Append the char to the current word
        *(word_buffer++) = t;

        //Increment the size
        size++;

        command_size--;
    }

    //Mark the end of the word, for strcmp.
    *word_buffer = 0;

    return size;
}


/*
 * execute : this function parses the recieved command, and if the parsing leads to an existing function,
 *      executes this function.
 *
 *  If not, the fail_log function is called, so that a relevant log message is displayed.
 *
 */

void TerminalInterface::execute() {

    //Mark the end of the the recieved command
    *data_in = 0;

    //Display the revieved command
    echo("\n"+str(SOFTWARE_NAME)+"> "+str(data_in_0));

    //Setup and save the message state
    data_in = data_in_0;
    saved_command_size = command_size;

    //Get the first word
    get_next_word();

    //If the current word matched the new name, then go to the lower level
#define GO_LOWER(name, description)\
    if (!strcmp(#name, word_buffer_0)) {\
        get_next_word();

    //If the current word matched the new name, then execute the associated function. If the function fails, fail.
#define CREATE_LEAF(name, function, description, syntax)\
    if (!strcmp(#name, word_buffer_0)) {\
        if (!TerminalInterfaceCommands::function(data_in, command_size)) {fail_log();}\
        return;\
    } else

    //if all sub_nodes have been checked and no one was relevant, fail.
#define GO_UPPER\
    fail_log();\
    return;\
    }

#include "terminal_interface_config.h"

#undef GO_UPPER
#undef GO_LOWER
#undef CREATE_LEAF

    //If no function was triggered at the first stage, fail.
    fail_log();

}


/*
 * fail_log : this function computes the log message to display after a command's parsing failure.
 *
 *  It starts to restore the command's environment, and computes the log message, fllowing a similar algorithm than
 *      the one used to process the command.
 *
 *  This function is not included in the parsing, because it is very unoptimised, process-greedy, and not required
 *      if the command was correctly formatted.
 *
 *  If and only if the command contains an error, that prevents its parsing, this function will be called.
 *
 */

void TerminalInterface::fail_log() {

    //Restore the previous message state
    command_size = saved_command_size;
    data_in = data_in_0;

    //get the first word
    get_next_word();

    //The log message
    string_t log_message = "";

    //The command part
    string_t syntax_command = "";

    //If the current word matched the new name, then reset the log message, and add the name to the syntax command.
#define GO_LOWER(name, description)\
        log_message+=str(#name) +"\t\t : "+str(#description)+"\n";\
        if (!strcmp(#name, word_buffer_0)) {\
            get_next_word();\
            log_message = "";\
            syntax_command+=String(#name)+" ";

    //If the current word matched the new name, the function ha failed : show the syntax description.
#define CREATE_LEAF(name, function, description, syntax);\
    log_message+=str(#name) +"\t\t : "+str(#description)+"\n";\
    if (!strcmp(#name, word_buffer_0)) {\
        echo(str("Usage : ")+syntax_command+""+str(#name)+" "+str(#syntax));\
        return;\
    }

    //if all sub_nodes have been checked and no one was relevant, display the parent node content message.
#define GO_UPPER\
    echo(log_message);\
    return;\
    }

#include "terminal_interface_config.h"

#undef GO_UPPER
#undef GO_LOWER
#undef CREATE_LEAF

    echo(log_message);

}



#define m TerminalInterface


unsigned char m::command_size = 0;
unsigned char m::saved_command_size = 0;

char tdatain_terminal[PACKET_SIZE];
char *m::data_in = tdatain_terminal;
char *const m::data_in_0 = tdatain_terminal;


char twrd[WORD_MAX_SIZE];
char *m::word_buffer = twrd;
char *const m::word_buffer_0 = twrd;

#undef m

#endif