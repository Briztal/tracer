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
#include <interface.h>
#include <DataStructures/StringUtils.h>
#include <Project/InterfaceCommands/TerminalCommands.h>

/*
 * init : this function initialises the serial, and sets up the command processing environment.
 */

void TerminalInterface::init() {

    //Initialise the serial
    terminal_interface_link_t::begin();

    //Wait for the serial to  correctly init
    delay_ms(100);

    //Setup a correct command environment
    reset();

}


/*
 * echo : this function sends a String over the serial.
 */

void TerminalInterface::echo(const string_t msg) {
    terminal_interface_link_t::send_str(msg + "\n");
}


/*
 * read_data : this function reads and saved the serial available data.
 *
 *  It returns true if data is still available in the data link buffer, and false if the buffer is empty;
 */

bool TerminalInterface::read_data() {

    //Don't process any data if no space is available in the argument_t sequence container
    if (!arguments_storage.available_spaces())

        //Return true if the data_link buffer is not empty.
        return (bool)(terminal_interface_link_t::available());

    while (terminal_interface_link_t::available()) {

        //Read the serial
        char read_char = terminal_interface_link_t::read();

        //If the recieved char is a line feed or a working_carriage return
        if ((read_char == 10) || (read_char == 13)) {

            //If a non empty uncorrupted packet has effectively been received
            if (command_size && !corrupted_packet) {

                //Parse and execute the command
                schedule_command();

                //Reset the data_in
                reset();

                //If no more task is schedulable, complete.
                if (!TaskScheduler::available_spaces(255)) {

                    //Return true if the data_link buffer is not empty.
                    return (bool)(terminal_interface_link_t::available());

                }

                //Don't process any data if no space is available in the argument_t sequence container
                if (!arguments_storage.available_spaces()) {

                    //Return true if the data_link buffer is not empty.
                    return (bool)(terminal_interface_link_t::available());

                }

            }

            //If the received packet was too long for the input buffer :
            if (corrupted_packet) {

                //Display an error message
                CI::echo("WARNING in TerminalInterface::read_data : the received packet was too long for "
                                 "the input buffer. Please check your terminal_interface_config.h");


            }
            //If the packet was corrupted, or empty
            reset();


        } else {
            //If the packet hasn't been entirely received

            //If data still can be inserted in the buffer
            if (data_spaces) {

                //Append the get_read_adress char to data_in;
                *(data_in++) = read_char;

                //Increment the command size;
                command_size++;

                //Decrement the number of nb_spaces available;
                data_spaces--;

            } else {

                //Mark the current packet as corrupted;
                corrupted_packet = true;
            }

        }
    }
}


/*
 * reset : this function resets the command reception environment
 */

void TerminalInterface::reset() {

    //No data;
    command_size = 0;

    //Clear the corruption flag;
    corrupted_packet = false;

    //Maximum numbers of char nb_spaces;
    data_spaces = TERMINAL_MAX_SIZE;

    //data insertion at the origin;
    data_in = data_in_0;

}



//-----------------------------------------------------Execution--------------------------------------------------------


/*
 * schedule_command : this function analyses the packet received, and parses the command part.
 *
 *  When it has found the TerminalCommand identified by this string, it saves the rest of the packet (the arguments part)
 *      in the argument_t storage, and then, schedules the execution of this function.
 *
 *  This command will be executed, by the intermediary of the execute_command function, defined below.
 *      (see the doc above the execute_command function's definition for more explanations).
 */

void TerminalInterface::schedule_command() {

    prepare_execution();

    //Initialise the current current_node to the root;
    TerminalNode *current_node = command_tree;

    //Cache var for the sub nodes.
    TerminalNode **sub_nodes = current_node->sub_nodes;

    //Must declare the int before the jump label.
    uint8_t i;

    //A cache for the child of the current node we are focused on.
    TerminalNode *current_sub_node;

    //--------------------------Tree Iteration--------------------------

    //As the algorithm involves two for - while loops, we must use a goto label to restart with a new node.
    node_check:

    //We must first get the next word of the command.

    //Remove unnecessary nb_spaces;
    data_in += StringUtils::lstrip(data_in, ' ');

    //Update th beginninhg of the current command_identifier.
    char *command_identifier = data_in;

    //get the first node identifier, in the command_identifier buffer
    data_in += StringUtils::count_until_char(data_in, ' ');

    //Nullify and update only if the string is not finished.
    if (data_in) {

        //Null terminate the word, to make a string of it.
        *data_in = '\0';

        //Go to the next char
        data_in++;

    }


    //We now must compare each child of the current node, and compare its identifier with the read command identifier.

    //Check every sub_node
    for (i = 0; i < current_node->sub_nodes_nb; i++) {

        //Update the current sub_node
        current_sub_node = sub_nodes[i];

        const char *node_identifier = (*current_sub_node->name).c_str();

        //If the current command_identifier matches the current_node's name
        if (!strcmp(node_identifier, command_identifier)) {

            //A matching sub_node has been found. It can be a single
            //Update the current node and the current sub_nodes array.
            current_node = current_sub_node;
            sub_nodes = current_node->sub_nodes;

            //if the new node is not a leaf, check sub nodes
            if (current_node->sub_nodes_nb) {

                //Restart the process with the new node
                goto node_check;

            } else {

                if (arguments_storage.available_spaces()) {

                    //The argument_t's index
                    uint8_t index;

                    //Save the arguments sequence.
                    if (arguments_storage.insert_argument(data_in, &index)) {


                        //Create a struct in the heap to contain argument_t-related data.
                        interface_data_t *data = new interface_data_t();
                        data->arguments_index = index;
                        data->function = current_node->function;

                        /*
                         * Schedule a type 255 (asap) task, to schedule_command the required function,
                         *  with data as arguments.
                         */

                        TaskScheduler::schedule_task(255, execute_command, (void *) data);

                        //Complete
                        return;

                    } //else : too long argument_t, log message sent by argument_container.

                } else {

                    //If no more space was available in the argument_t container : display an error message
                    CI::echo(
                            "ERROR in TerminalInterface::schedule_command : the argument_t container has no more space "
                                    "available, this is not supposed to happen");

                    //Fail
                    return;

                }

            }

        }

    }

    //If the current node didn't contain the required command : log the node's content
    log_parsing_error(current_node);

}


/*
 * prepare execution : this function is called before parsing a packet.
 */

void TerminalInterface::prepare_execution() {

    //Mark the end of the the received command
    *data_in = 0;

    //Display the revieved command
    echo("\n" + str(PROJECT_NAME) + "> " + str(data_in_0));

    //Setup and save the message state
    data_in = data_in_0;

}


/*uint8_t
 * log_parsing_error : displays a message, after a parsing problem.
 *
 *  It displays the node's children.
 */

void TerminalInterface::log_parsing_error(TerminalNode *log_node) {

    //Display the last correct node's content.

    //init an empty string
    String s = "";

    //Fill it with the name and description of direct sub_nodes
    for (int i = 0; i < log_node->sub_nodes_nb; i++) {
        TerminalNode *t = log_node->sub_nodes[i];
        s += *t->name + "\t\t : " + *t->desc_log + "\n";
    }

    //Display
    echo(s);

}


/*
 * execute_command : this command eases the redaction of TerminalCommands :
 *
 *  Normally, if scheduled directly, a TerminalCommand receives a void *, that must be casted in a char *,
 *      the address of the arguments sequence. After the execution, if the command mustn't be reprogrammed,
 *      those arguments must be removed from the argument_t container.
 *
 *      As this procedure is common to all TerminalCommands, this function's goal is to execute this procedure.
 *
 *  Now, a TerminalCommand receives a simple char *, and this function does the following :
 *      - extracting arguments (char *);
 *      - executing the TerminalCommand, passing these args.
 *      - eventually removing the arguments.
 */

task_state_t TerminalInterface::execute_command(void *data_pointer) {

    //Get the terminal interface data back
    interface_data_t *data = (interface_data_t *) data_pointer;

    //Cache var for arguments index.
    uint8_t arguments_index = data->arguments_index;

    //Cache for arguments.
    char *arguments = arguments_storage.get_argument(arguments_index);

    //Execute the required TerminalCommand function, and get the execution state
    const task_state_t state = (*data->function)(arguments);

    //Save the state
    data->return_state = state;

    //Remove arguments arguments, if the task mustn't be reprogrammed
    if (state != reprogram) {
        arguments_storage.remove_argument(arguments_index);
    }

    //Confirm the command execution
    confirm_command_execution(data);

    //Return the execution state.
    return state;
}


/*
 * confirm_command_execution : this command notifies the interlocutor of the execution state of the command.
 *
 *  As the terminal interface is a human-only interface, we will simply display a message to the user.
 */

void TerminalInterface::confirm_command_execution(const interface_data_t *data) {

//The log will occur only if the command flag is set.

#ifdef TERMINAL_EXECUTION_CONFIRMATION

    //Switch the return state.
    switch (data->return_state) {

        //If the task completed correctly
        case complete:
            CI::echo("Command complete.");
            return;

            //If the task completed correctly
        case invalid_arguments:
            CI::echo("Invalid Arguments.");
            return;

            //If the task must be reprogrammed:
        case reprogram:
            CI::echo("Command Reprogrammed.");
            return;

            //If the task completed correctly
        default:
            CI::echo("WARNING Unrecognised return state");
            return;

    }

#endif


}


//--------------------------------------Arguments Processing--------------------------------------

/*
 * parse_arguments : this function parses an argument_t sequence, whose format is like below :
 *
 *  -i0 arg0 -i1 arg1 ... -in argn
 *
 *  After the parsing, arguments can be get with the get_argument methods.
 *
 *  Remarks :
 *      - argi can be empty.
 *      - The function DOES NOT COPY ARGS. All it does is saving the indices (char),
 *          and their eventual args's positions (char *). The argument sequence must NEVER be changed,
 *          or args values will be corrupted.
 */

bool TerminalInterface::parse_arguments(char *argument_sequence) {

    //A cache with a more relevant name
    char *current_position = argument_sequence;

    //First, reset the argument_t parsing structure :

    //reset the arguments counter
    nb_identifiers = 0;


    do {

        //go to the closest argument_t identifier
        current_position += StringUtils::count_until_char(current_position, '-');

        //If we have reached the end of the sequence
        if (!*current_position)
            return true;

        //Get the pointer to the identifier;
        char identifier = *(current_position + 1);

        //Get the argument_t identifier
        uint8_t identifier_size = StringUtils::count_until_char(current_position, ' ');

        //Go to the next unprocessed char
        current_position += identifier_size;


        //If the identifier is invalid
        if (identifier_size != 2) {

            //Display an error message
            CI::echo("INVALID ARGUMENT IDENTIFIER");

            //Fail
            return false;

        }

        //If the argument_t container is already full;
        if (nb_identifiers == TERMINAL_MAX_ARGS_NB) {

            //Display an error message
            CI::echo(
                    "The TerminalInterface hasn't been configured to accept more than " + String(TERMINAL_MAX_ARGS_NB) +
                    " arguments. Please check your terminal_interface_config.h file.");

            //Fail
            return false;

        }


        //remove extra nb_spaces;
        current_position += StringUtils::lstrip(current_position, ' ');

        char *arg;
        char *space_position;
        uint8_t argument_size;

        //If the argument sequence is finished;
        if (!*current_position) {

            //The arg will point to the zero ending the sequence
            arg = current_position;

            //An empty argument must be added;
            goto insert_arg;

        }

        //The current position corresponds now to a '-', or to the beginning of an argument.
        //We now must parse the argument, if it is provided.

        //Count the numbers of chars between the current position and an argument identifier (or the end of the string);
        argument_size = StringUtils::count_until_char(current_position, '-');

        //If no argument is provided :
        if (!argument_size) {

            //The arg will point to the space (changed in zero below) before the current position;
            arg = current_position - 1;

            //Change the space to a zero;
            *arg = '\0';

            //An empty argument must be added;
            goto insert_arg;

        }

        //The argument now exists for sure, but we must ensure that there is a '\0' at its end.

        //If we ended the argument sequence, no need to add the zero.
        if (!*(current_position + argument_size)) {

            //The arg will point to the zero ending the sequence
            arg = current_position;

            goto insert_arg;

        }

        //We are now sure that a '-' is present.

        //Normally, a space should be present just before the '-';
        space_position = current_position + argument_size - 1;

        //If the char at the space position is effectively a char:
        if (*space_position == ' ') {

            //Nullify it;
            *space_position = '\0';

            //Arg will point to the current position (the beginning of the argument.
            arg = current_position;

            //Update the current position the new '-' (or at the end of the sequence).
            current_position += argument_size;

            //Insert the non empty arg;
            goto insert_arg;

        } else {

            //If the '-' was in a word, display an error message;
            CI::echo("Invalid arguments, please do not use '-' in a word.");

            //Fail;
            return false;

        }

        //----------------------------------Argument saving----------------------------------


        insert_arg:

        //Save the relation between the identifier and the argument_t location.
        argument_t *argument = arguments + nb_identifiers;
        argument->identifier = identifier;
        argument->arg = arg;


        //Increase the number of parsed arguments
        nb_identifiers++;

        //If we have reached the end of the sequence
        if (!*current_position)
            return true;


    } while (true);


}

/*
 * get_argument_value : this function presupposes that the argument_t referenced
 *  by the identifier id is a numeric value, and returns that value.
 */

float TerminalInterface::get_argument_value(char id) {

    //Get the argument_t pointer.
    char *ptr = get_argument(id);

    //return the float equivalent.
    if (ptr != nullptr) {
        return str_to_float(ptr);
    } else return 0;

}


/*
 * get_argument : this function returns a pointer to the argument_t (char *) referenced
 *  by the identifier id.
 */

char *TerminalInterface::get_argument(char id) {

    //For every identifier
    for (uint8_t i = 0; i < nb_identifiers; i++) {

        //Cache for the link
        argument_t *link = arguments + i;

        //If the links'identifier matches the provided one
        if (link->identifier == id) {

            //Get the location of the argument_t (from the link), and convert the argument_t into a float.
            return link->arg;

        }

    }

    return nullptr;

}


/*
 * verify_all_identifiers_presence : this function return true only if ALL arguments contained
 *  in the arguments string have been extracted during the previous parsing.
 *
 *  the identifier string is a string, where all letters are arguments to check .
 *      ex "arp" triggers the checking for arguments a, r and p.
 */

bool TerminalInterface::verify_all_identifiers_presence(const char *identifiers) {

    //Cache for the current char
    char c = *(identifiers++);

    //For every identifier in the string (stop at null byte);
    while (c) {

        //If the identifier is not present
        if (!verify_identifier_presence(c)) {

            //Fail
            return false;

        }

        //Update the char
        c = *(identifiers++);

    }

    //All arguments are present, succeed.
    return true;

}


/*
 * verify_all_identifiers_presence : this function return true only if ONE arguments contained
 *  in the arguments string has been extracted during the previous parsing.
 *
 *  the identifier string is a string, where all letters are arguments to check .
 *      ex "arp" triggers the checking for arguments a, r and p.
 */

bool TerminalInterface::verify_one_identifiers_presence(const char *identifiers) {

    //Cache for the current char
    char c = *(identifiers++);

    //For every identifier in the string (stop at null byte);
    while (c) {

        //If the identifier is present
        if (verify_identifier_presence(c)) {

            //Succeed
            return true;

        }

        //Update the char
        c = *(identifiers++);

    }

    //No identifier detected, fail.
    return false;

}


/*
 * verify_identifier_presence : this function return true if the identifier id has been extracted
 *  during the previous parsing.
 */

bool TerminalInterface::verify_identifier_presence(char id) {

    //Check every parsed identifier
    for (uint8_t i = 0; i < nb_identifiers; i++) {

        //Cache for the link
        argument_t *link = arguments + i;

        //If the links'identifier matches the provided one, return true
        if (link->identifier == id) return true;

    }

    //if there was no matches, return false.
    return false;

}


//--------------------------------------------------Tree generation-----------------------------------------------------


/*
 * generate_tree : this function generates the TerminalNode that will be used to parse commands.
 */

TerminalNode *TerminalInterface::generate_tree() {
    uint16_t command_counter = 0;

    //Get the number of sons of root.
    uint8_t root_sons_nb = get_sub_nodes_nb(command_counter++);

    //Create the root tree.
    TerminalNode *root = new TerminalNode(new String("root"), root_sons_nb, new String("root"), nullptr);

    //Initialise the current tree and the history.
    TerminalNode *current_tree = root;
    TerminalNode *tree_history[TERMINAL_MAX_DEPTH];

    //Initialise the indices history
    uint8_t indices_history[TERMINAL_MAX_DEPTH];
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
    current_tree = new TerminalNode(new String(#name), tmp_nb, new String(#desc), 0);\
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

#define CREATE_LEAF(name, function, desc)\
    current_tree->sub_nodes[current_index++] = new TerminalNode(new String(#name), 0, new String(#desc), TerminalCommands::function);\
    command_counter++;

#include "Project/Config/terminal_interface_config.h"

#undef GO_LOWER

#undef GO_UPPER

#undef CREATE_LEAF

    return root;
}


/*
 * get_sub_nodes_nb : this function determines the number of direct sub_nodes of a particular indice in the sub_nodes
 *  string.
 */

uint8_t TerminalInterface::get_sub_nodes_nb(uint16_t command_index) {

    uint16_t max = (uint16_t) tree_summary->length();

    //If the command index is below the command nb, return 0 for safety
    if (command_index > max) return 0;

    //If the first command is a create_leaf of go_upper, 0 sons.
    char v = tree_summary->charAt(command_index++);
    if (v == (char) 0 || v == (char) 1)
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
        } else if (v == (char) 1) {
            if (!depth) sons_nb++;
        } else if (v == (char) 2) {
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
 */

String *TerminalInterface::build_tree_summary() {

    String *s = new String();

    //Set the initial go_lower
    s->append((char) 2);

#define GO_LOWER(...) s->append((char)2);

#define GO_UPPER(...) s->append((char)0);

#define CREATE_LEAF(...) s->append((char)1);

#include "Project/Config/terminal_interface_config.h"

#undef GO_UPPER

#undef GO_LOWER

#undef CREATE_LEAF

    return s;

}

//---------------------------------Static declarations / definitions------------------------------


#define m TerminalInterface

unsigned char m::command_size = 0;

char tdatain_terminal[TERMINAL_MAX_SIZE];
char *m::data_in = tdatain_terminal;
char *const m::data_in_0 = tdatain_terminal;

//Identifiers in a parsed argument_t sequence
argument_t terminal_arguments_t[TERMINAL_MAX_ARGS_NB];
argument_t *const m::arguments = terminal_arguments_t;

//Number of arguments in a sequence
uint8_t m::nb_identifiers = 0;

ArgumentsContainer m::arguments_storage = ArgumentsContainer(TERMINAL_MAX_ARGS_NB * (TERMINAL_MAX_WORD_SIZE + 4) + 1,
                                                             TERMINAL_MAX_PENDING_COMMANDS);
//The current number of available nb_spaces in the data bugger
uint8_t m::data_spaces = TERMINAL_MAX_SIZE;

//A flag set if the current packet is corrupted (too long for the data buffer)
bool m::corrupted_packet = false;

//Create the command tree summary.
String *m::tree_summary = m::build_tree_summary();

//Build the command tree
TerminalNode *m::command_tree = m::generate_tree();



#undef m

#endif