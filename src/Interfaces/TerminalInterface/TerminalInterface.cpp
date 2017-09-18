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
#include "TerminalNode.h"


/*
 * begin : this function initialises the serial, and sets up the command processing environment.
 *
 */
void UI::begin() {

    pinMode(13, OUTPUT);
    digitalWriteFast(13, HIGH);
    //digitalWrite(13, !digitalRead(13));

    //Initialise the serial
    terminal_interface_link_t::begin();

    //Wait for the serial to  correctly initialise
    delay_ms(100);

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


    pinMode(13, OUTPUT);
    digitalWriteFast(13, HIGH);

    while (terminal_interface_link_t::available()) {

        //Read the serial
        char read_char = terminal_interface_link_t::read();

        //If the recieved char is a line feed or a carriage return
        if ((read_char == 10) || (read_char == 13)) {

            //If a char has effectively been received
            if (command_size) {

                //Parse and execute_progmem_style the command
                execute_tree_style();

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


void TerminalInterface::prepare_execution() {
    //Mark the end of the the recieved command
    *data_in = 0;

    //Display the revieved command
    echo("\n" + str(SOFTWARE_NAME) + "> " + str(data_in_0));

    //Setup and save the message state
    data_in = data_in_0;
    saved_command_size = command_size;

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

//-----------------------------------------------Tree execution and log-------------------------------------------------

TerminalNode *TerminalInterface::generate_tree() {
    uint16_t command_counter = 0;

    uint8_t root_sons_nb = get_sub_nodes_nb(command_counter++);

    TerminalNode *root = new TerminalNode(new String("root"), root_sons_nb, new String("root"),  new String("none"), 0);

    //Initialise the current tree and the history.
    TerminalNode *current_tree = root;
    TerminalNode *tree_history[MAX_DEPTH];

    //Initialise the indices history
    uint8_t indices_history[MAX_DEPTH];
    uint8_t current_index = 0;
    uint8_t depth = 0;

    uint8_t tmp_nb;


    /*
     * Procedure for a go_lower :
     *  - save the index
     *  - save the current node
     *  - get the children number for the current node;
     *  - create a new current node
     *  - increment the depth;
     */

#define GO_LOWER(name, desc)\
    indices_history[depth] = current_index;\
    current_index = 0;\
    tree_history[depth] = current_tree;\
    tmp_nb = get_sub_nodes_nb(command_counter++);\
    current_tree = new TerminalNode(new String(#name), tmp_nb, new String(#desc), new String(""), 0);\
    depth++;


    /*
     * Procedure for a go_upper :
     *  - decrement the depth;
     *  - restore the previous index;
     *  - use it to add the current node to the previous node's sons;
     *  - increment the index;
     *  - restore the previous node;
     *  - increment the command counter.
     */

#define GO_UPPER()\
    depth--;\
    current_index = indices_history[depth];\
    tree_history[depth]->sub_nodes[current_index++] = current_tree;\
    current_tree = tree_history[depth];\
    command_counter++;


    /*
     * Procedure for a leaf creation :
     *  - create a leaf node;
     *  - add it to the current node;
     *  - increment the index;
     *  - increment the command counter.
     */

#define CREATE_LEAF(name, function, desc, args)\
    current_tree->sub_nodes[current_index++] = new TerminalNode(new String(#name), 0, new String(#desc), new String(#args), TerminalInterfaceCommands::function);\
    command_counter++;

#include "terminal_interface_config.h"

#undef GO_LOWER

#undef GO_UPPER

#undef CREATE_LEAF

    return root;
}


/*
 * get_sub_nodes_nb : this function determines the number of direct sub_nodes of a particular indice in the sub_nodes
 *  string.
 *
 */

uint8_t TerminalInterface::get_sub_nodes_nb(uint16_t command_index) {

    uint16_t max = (uint16_t) tree_summary->length();

    //If the command index is below the command nb, return 0 for safety
    if (command_index > max) return 0;

    //If the first command is a create_leaf of go_upper, 0 sons.
    char v = tree_summary->charAt(command_index++);
    if (v == (char)0 || v == (char)1)
        return 0;

    //If not, we will count to the next go_upper (0);

    uint8_t sons_nb = 0;
    uint8_t depth = 0;

    v = tree_summary->charAt(command_index++);

    //We stop counting when we detect a zero at depth zero, or when the all command have been listed.
    // -> keep while the depth is not zero or the value is not zero, and command are still to be processed.
    while (((depth || v)) && (command_index != max)) {
        //correct the depth if we go upper of lower
        if (v == 0) {
            depth--;
        } else if (v == (char)1) {
            if (!depth) sons_nb++;
        } else if (v == (char)2) {
            if (!depth) sons_nb++;
            depth++;
        }
        v = tree_summary->charAt(command_index++);
    }

    return sons_nb;

}


/*
 * generate_sub_nodes_string : this function generates a string, containing the summary of the tree creation :
 *  - 0 means go_upper
 *  - 1 means create_leaf
 *  - 2 means go_lower
 *
 *  This string is used to determine the number of sub_nodes of a particular node.
 *
 */
String * TerminalInterface::build_tree_summary() {

    String *s = new String();

    //Set the initial go_lower
    s->append((char)2);

#define GO_LOWER(...) s->append((char)2);

#define GO_UPPER(...) s->append((char)0);

#define CREATE_LEAF(...) s->append((char)1);

#include "terminal_interface_config.h"

#undef GO_UPPER

#undef GO_LOWER

#undef CREATE_LEAF

    return s;

}

void TerminalInterface::execute_tree_style() {

    prepare_execution();

    //Initialise the current current_node to the root;
    TerminalNode *current_node = command_tree;
    TerminalNode *current_sub_node;

    TerminalNode **sub_nodes = current_node->sub_nodes;
    //get the first word
    get_next_word();

    uint8_t i;

    node_check:

    //Check every sub_node
    for (i = 0; i < current_node->sub_nodes_nb; i++) {
        current_sub_node = sub_nodes[i];

        const char* c = (*current_sub_node->name).c_str();

        //If the current word matches the current_node's name
        if (!strcmp(c, word_buffer_0)) {

            //Go to the lower level
            get_next_word();

            //Re-initialise the current data
            current_node = current_sub_node;
            sub_nodes = current_node->sub_nodes;

            //if the new node is a leaf, execute its function
            if (!current_node->sub_nodes_nb) {

                //If the function fails,
                if (!current_node->function(data_in, command_size)) {

                    //Log for the current node : log the node's args
                    log_tree_style(current_node, true);
                }

                //check the new node
                goto node_check;
            }
        }
    }

    //If the current node didn't contain the required command : log the node's content
    log_tree_style(current_node, false);

}


void TerminalInterface::log_tree_style(TerminalNode *log_node, bool log_args) {

    if (log_args) {

        //If the args parsing failed, display the correct syntax for args
        echo("Usage : " + *log_node->name + " " + *log_node->args_log);\

    } else {

        //If the command parsing failes, display the last correct node's content.

        //initialise an empty string
        String s = "";

        //Fill it with the name and description of direct sub_nodes
        for (int i = 0; i < log_node->sub_nodes_nb; i++) {
            TerminalNode * t = log_node->sub_nodes[i];
            s += *t->name + "\t\t : " + *t->desc_log + "\n";
        }

        //Display
        echo(s);

    }

}

//---------------------------------------------Prog_mem execution and log-----------------------------------------------

/*
 * execute_progmem_style : this function parses the received command, and if the parsing leads to an existing function,
 *      executes this function.
 *
 *  If not, the fail_log_progmem_style function is called, so that a relevant log message is displayed.
 *
 */

void TerminalInterface::execute_progmem_style() {

    prepare_execution();

    //Get the first word
    get_next_word();

    //If the current word matched the new name, then go to the lower level
#define GO_LOWER(name, description)\
    if (!strcmp(#name, word_buffer_0)) {\
        get_next_word();

    //If the current word matched the new name, then execute_progmem_style the associated function. If the function fails, fail.
#define CREATE_LEAF(name, function, description, syntax)\
    if (!strcmp(#name, word_buffer_0)) {\
        if (!TerminalInterfaceCommands::function(data_in, command_size)) {fail_log_progmem_style();}\
        return;\
    } else

    //if all sub_nodes have been checked and no one was relevant, fail.
#define GO_UPPER()\
    fail_log_progmem_style();\
    return;\
    }

#include "terminal_interface_config.h"

#undef GO_UPPER
#undef GO_LOWER
#undef CREATE_LEAF

    //If no function was triggered at the first stage, fail.
    fail_log_progmem_style();

}


/*
 * fail_log_progmem_style : this function computes the log message to display after a command's parsing failure.
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


void TerminalInterface::fail_log_progmem_style() {

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
            syntax_command+=str(#name)+" ";

    //If the current word matched the new name, the function ha failed : show the syntax description.
#define CREATE_LEAF(name, function, description, syntax);\
    log_message+=str(#name) +"\t\t : "+str(#description)+"\n";\
    if (!strcmp(#name, word_buffer_0)) {\
        echo(str("Usage : ")+syntax_command+""+str(#name)+" "+str(#syntax));\
        return;\
    }

    //if all sub_nodes have been checked and no one was relevant, display the parent node content message.
#define GO_UPPER()\
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


String *m::tree_summary = m::build_tree_summary();

//Create the command tree summary

//Build the command tree
TerminalNode *m::command_tree = m::generate_tree();


#undef m

#endif