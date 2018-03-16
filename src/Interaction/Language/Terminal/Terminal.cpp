/*
  Terminal.cpp - Part of TRACER

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

#include <Config/control_config.h>

#ifdef ENABLE_TERMINAL_INTERFACE

#include "Terminal.h"

#include <Config/controller_terminal_config.h>

#include "TerminalTreeGenerator.h"
#include "TerminalArguments.h"
#include <Interaction/Interaction.h>
#include <DataStructures/StringUtils.h>


/*
 * initialise_hardware : this function initialises the serial, and sets up the command processing environment.
 */

void Terminal::initialise_data(Delimiter *protocol) {

    //Initialise the delimiter;
    output_protocol = protocol;

    //Arguments initialisation;
    TerminalArguments::clear();

    //Tree initialisation.

    //delete the current tree;
    delete (command_tree);

    //Create a new command tree;
    command_tree = TerminalTreeGenerator::generate_tree();


}


/*
 * The following function displays a logo at the initialisation of the code.
 */

void Terminal::init_message() {

    log("");
    log("    dBBBBBBP  dBBBBb  dBBBBb     dBBBP  dBBBP  dBBBBb");
    log("      dBP     dP  dP   dP BB    dP     dP      BP  dP");
    log("     dBP     dBBBBK'  dP  BB   dP     dBBP    dBBBBK'");
    log("    dBP     dBP  BB  dBBBBBB  dP     dP      dBP  BB");
    log("   dBP     dBP  dB' dBP   BB dBBBBP dBBBBP  dBP  dB'");
    log("");
    log("\nTRACER initialised and ready. Waiting for commands.\n\n");

}


//-----------------------------------------------------Execution--------------------------------------------------------


/*
 * parse : this function analyses the packet received, and parses the command part.
 *
 *  When it has found the TerminalCommand identified by this tstring, it saves the rest of the packet (the content part)
 *      in the argument_t storage, and then, schedules the execution of this function.
 *
 *  This command will be executed, by the intermediary of the execute_command function, defined below.
 *      (see the doc above the execute_command function's definition for more explanations).
 */

void Terminal::parse(char *message) {

    //Display the revieved command
    std_out(tstring("\n") + PROJECT_NAME + "> " + message);


    //Initialise the current current_node to the root;
    const TerminalTree *current_node = command_tree;

    //Must declare the int before the jump label.
    uint8_t i;


    //--------------------------Tree Iteration--------------------------

    //As the algorithm involves two for - while loops, we must use a goto label to restart with a new node.
    node_check:

    //We must first get the next word of the command.

    //Remove unnecessary nb_spaces;
    message += StringUtils::lstrip(message, ' ');

    //Update th beginning of the current command_identifier.
    char *command_identifier = message;

    //get the first node identifier, in the command_identifier buffer
    message += StringUtils::count_until_char(message, ' ');

    //Nullify and update only if the tstring is not finished.
    if (message) {

        //Null terminate the word, to make a tstring of it.
        *message = '\0';

        //Go to the next char
        message++;

    }

    //We now must compare each child of the current node, and compare its identifier with the read_data command identifier.

    //Check every sub_node
    for (i = 0; i < current_node->nb_children; i++) {

        //Flag for the tree query execution;
        bool tree_flag = false;

        //Cache the current sub_node
        const TerminalTree *current_sub_node = current_node->get_child(i, &tree_flag);

        //Check the correct execution of the command :
        if (!tree_flag) {

            //Log
            log("Error in Terminal::parse : Unable to query the required child tree.");

            //Fail;
            return;

        }

        const char *node_identifier = (*current_sub_node->name).data();

        //If the current command_identifier matches the current_node's name
        if (!strcmp(node_identifier, command_identifier)) {

            //A matching sub_node has been found. It can be a single
            //Update the current node and the current nb_children array.
            current_node = current_sub_node;

            //if the new node is not a leaf, check sub nodes
            if (current_node->nb_children) {

                //Restart the process with the new node
                goto node_check;

            } else {

                if (TerminalArguments::available_spaces()) {

                    //The argument_t's index
                    uint8_t index;

                    //Save the content sequence.
                    if (TerminalArguments::insert_argument(message, &index)) {


                        //Create a struct in the heap to contain argument_t-related data.
                        controller_data_t *data = new controller_data_t();
                        data->arguments_index = index;
                        data->function = current_node->function;

                        /*
                         * Schedule a type 255 (asap) task, to parse the required function,
                         *  with data as content.
                         */

                        TaskScheduler::schedule_task(255, execute_command, (void *) data, external_log, output_protocol);

                        //Complete
                        return;

                    } //else : too long argument_t, log message sent by argument_container.

                } else {

                    //If no more space was available in the argument_t container : display an error message
                    log(
                            "ERROR in Terminal::parse : the argument_t container has no more space "
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




/*uint8_t
 * log_parsing_error : displays a message, after a parsing problem.
 *
 *  It displays the node's nb_children.
 */

void Terminal::log_parsing_error(const TerminalTree *const log_node) {

    //Display the last correct node's content.

    //initialise_hardware an empty tstring
    tstring s;

    //Fill it with the name and description of direct nb_children
    for (uint8_t i = 0; i < log_node->nb_children; i++) {

        //Create a flag to check the execution of the command;
        bool tree_flag = false;

        //Get the child tree;
        const TerminalTree *t = log_node->get_child(i, &tree_flag);

        //If the query succeeded;
        if (tree_flag) {

            //Complete the tstring
            s += *t->name + "\t\t : " + *t->description + "\n";


        } else {
            //If the query fails :

            s += string("error \n");

        }
    }

    //Display
    std_out(s);

}


/*
 * execute_command : this command eases the redaction of TerminalCommands :
 *
 *  Normally, if scheduled directly, a TerminalCommand receives a void *, that must be casted in a char *,
 *      the address of the content sequence. After the execution, if the command mustn't be reprogrammed,
 *      those content must be removed from the argument_t container.
 *
 *      As this procedure is common to all TerminalCommands, this function's goal is to execute this procedure.
 *
 *  Now, a TerminalCommand receives a simple char *, and this function does the following :
 *      - extracting content (char *);
 *      - executing the TerminalCommand, passing these args.
 *      - eventually removing the content.
 */

task_state_t Terminal::execute_command(void *data_pointer) {

    //Get the terminal interface data back
    controller_data_t *data = (controller_data_t *) data_pointer;

    //Cache var for content index.
    uint8_t arguments_index = data->arguments_index;

    //Cache for content.
    char *arguments = TerminalArguments::get_argument(arguments_index);

    //Execute the required TerminalCommand function, and get the execution state
    const task_state_t state = (*data->function)(arguments);

    //Save the state
    data->return_state = state;

    //Remove content content, if the task mustn't be reprogrammed
    if (state != reprogram) {
        TerminalArguments::remove_argument(arguments_index);
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

void Terminal::confirm_command_execution(const controller_data_t *data) {

//The log will occur only if the command flag is set.

#ifdef TERMINAL_EXECUTION_CONFIRMATION

    //Switch the return state.
    switch (data->return_state) {

        //If the task completed correctly
        case complete:
            log("Command complete.");
            return;

            //If the task completed correctly
        case invalid_arguments:
            log("Invalid Arguments.");
            return;

            //If the task must be reprogrammed:
        case reprogram:
            log("Command Reprogrammed.");
            return;

            //If the task completed correctly
        default:
            log("WARNING Unrecognised return state");
            return;

    }

#endif


}

//------------------------------------ Output -----------------------------



/*
 * log : this function encodes a tstring and transmits it with the output delimiter;
 */

void Terminal::external_log(Delimiter *protocol, const char * msg) {

    output_protocol->process(msg);
    output_protocol->process("\n");


}


/*
 * log : this function encodes a tstring and transmits it with the provided delimiter;
 */

void Terminal::log(const char *msg) {

    output_protocol->process(msg);
    output_protocol->process("\n");

}



/*
 * respond : this function is an alias for the system log command.
 *
 *  It echoes text data on the link layer
 */

void Terminal::respond(const char * msg) {

    output_protocol->process(msg);
    output_protocol->process("\n");


}
//---------------------------------Static declarations / definitions------------------------------


#define m Terminal

//Build the command tree
const TerminalTree *m::command_tree = new TerminalTree(new string(), 0, new string(), nullptr);

Delimiter *m::output_protocol;

#undef m

#endif