/*
  GCodeInterface.cpp - Part of TRACER

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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <Config/control_config.h>

#ifdef ENABLE_GCODE_INTERFACE

#include "GCode.h"
#include "GCodeTreeGenerator.h"
#include "GCodeArguments.h"
#include <DataStructures/StringUtils.h>


//----------------------------------------------------Initialisation----------------------------------------------------


/*
 * initialise_data : this function resets initialises all data of the class
 */

void GCode::initialise_data(Protocol *protocol) {

    //Initialise the output log_protocol;
    output_protocol = protocol;

    //Arguments initialisation;
    GCodeArguments::clear();

    //Tree initialisation.

    //delete the current tree;
    delete (command_tree);

    //Create a new command tree;
    command_tree = GCodeTreeGenerator::generate_tree();

}



/*
 * The following function displays a logo at the initialisation of the code.
 */

void GCode::init_message() {

    respond("start");

}


//---------------------------------------------------------Parse--------------------------------------------------------


/*
 * parse : this function parses the received GCode Command.
 *
 *  It is called when a GCode command has been entirely received
 */

bool GCode::parse(char *message) {

    //Remove extra spaces;
    message += StringUtils::lstrip(message, ' ');

    //Save the pointer to the first char of the command id.
    char *command_id = message;

    //Get the command id length;
    uint8_t command_id_length = StringUtils::count_until_char(message, ' ');

    //Abort if the first word was a space.
    if (!command_id_length)
        return false;

    //Update data_in
    message += command_id_length;

    //Analyse the command id.
    schedule_command(command_id, message);

    //Success
    return true;

}


/*
 * parse : this function analyses the packet received, and parses the command part.
 *
 *  When it has found the GCodeCommand identified by this string, it saves the rest of the packet (the arguments part)
 *      in the argument storage, and then, schedules the execution of this function.
 *
 *  This command will be executed, by the intermediary of the execute_command function, defined below.
 *      (see the doc above the execute_command function's definition for more explanations).
 */

void GCode::schedule_command(const char *command_id, const char *arguments) {

    //Initialise the current current_tree to the root;
    const GCodeTree *current_tree = command_tree;

    log("COMMAND : " + String(command_id));
    //The current depth
    uint8_t depth = 0;

    //Must declare the int before the jump label.
    uint8_t i;

    //A cache for the child of the current node we are focused on.
    const GCodeTree *current_child;

    //The current char in the command_char;
    char command_char;
    //--------------------------Tree Iteration--------------------------

    //As the algorithm involves two for - while loops, we must use a goto label to restart with a new node.
    node_check:

    //Get the command_char identifier, and increment the depth;
    command_char = command_id[depth++];


    //We now must compare each child of the current node, and compare its identifier with the read_data command_char identifier.

    //Check every sub_node
    for (i = 0; i < current_tree->nb_children; i++) {

        //We now need to get the child.

        //Declare a flag;
        bool tree_flag = false;

        //Update the current sub_node
        current_child = current_tree->get_child(i, &tree_flag);

        const char tree_id = current_child->name;

        //If the current command_identifier matches the current_tree's name
        if (command_char == tree_id) {

            //A matching sub_node has been found. It can be a single
            //Update the current node and the current children array.
            current_tree = current_child;

            //if the new node is not a leaf, check sub nodes
            if (current_tree->nb_children) {

                //Restart the process with the new node
                goto node_check;

            } else {

                //If arguments can be enqueued :
                if (GCodeArguments::available_spaces()) {


                    //The argument_t's index
                    uint8_t index;

                    //Save the arguments sequence.
                    if (GCodeArguments::insert_argument(arguments, &index)) {


                        //Create a struct in the heap to contain argument_t-related data.
                        interface_data_t *data = new interface_data_t();
                        data->arguments_index = index;
                        data->function = current_tree->function;


                        /*
                         * Schedule a type 255 (asap) task, to parse the required function,
                         *  with data as arguments, the log function, and the output protocol;
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

    //If the command_char didn't match any know child tree, send a warning and complete;
    log("Warning : Command not supported");
    respond("ok");
    
    

}


/*
 * execute_command : this command eases the redaction of TerminalCommands :
 *
 *  Normally, if scheduled directly, a GCodeCommand receives a void *, that must be casted in a char *,
 *      the address of the arguments sequence. After the execution, if the command mustn't be reprogrammed,
 *      those arguments must be removed from the argument container.
 *
 *      As this procedure is common to all GCodeCommands, this function's goal is to execute this procedure.
 *
 *  Now, a GCodeCommand receives a simple char *, and this function does the following :
 *      - extracting arguments (char *);
 *      - executing the GCodeCommand, passing these args.
 *      - eventually removing the arguments.
 */

task_state_t GCode::execute_command(void *data_pointer) {

    //Get the terminal interface data back
    interface_data_t *data = (interface_data_t *) data_pointer;

    //Cache var for arguments index.
    uint8_t arguments_index = data->arguments_index;

    //Cache for arguments.
    char *arguments = GCodeArguments::get_argument(arguments_index);

    //Execute the required TerminalCommand function, and get the execution state
    const task_state_t state = (*data->function)(arguments);

    /*remove arguments arguments, if the task mustn't be reprogrammed*/
    if (state != reprogram) {
        GCodeArguments::remove_argument(arguments_index);
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

void GCode::confirm_command_execution(const interface_data_t *data) {

    //Switch the return state.
    switch (data->return_state) {

        //If the task completed correctly
        case complete:
            respond("ok");
            return;

            //If the task completed correctly
        case invalid_arguments:
            log("WARNING Invalid Arguments");
            respond("ok");
            return;

            //If the task must be reprogrammed:
        case reprogram:
            //log("Command Reprogrammed.");
            return;

            //If the task completed correctly
        default:
            log("WARNING Unrecognised return state");
            respond("ok");
            return;

    }


}


//----------------------------------------------------System aliases----------------------------------------------------


/*
 * log : this function encodes a string and transmits it with the output protocol;
 */

void GCode::external_log(Protocol *protocol, const string_t msg) {

    protocol->encode_data("// " + msg + "\n");

}


/*
 * log : this function encodes a string and transmits it with the provided protocol;
 */

void GCode::log(const string_t msg) {

    output_protocol->encode_data("// " + msg + "\n");

}



/*
 * respond : this function is an alias for the system log command.
 *
 *  It echoes text data on the link layer
 */

void GCode::respond(const string_t msg) {

    output_protocol->encode_data(msg + "\n");

}

//-------------------------------------------Static declarations / definitions------------------------------------------

#define m GCode


//Create an empty temporary tree;
GCodeTree *m::command_tree = new GCodeTree(' ', 0, nullptr);

//The log_protocol used to transmit all communication interfaces;
Protocol *m::output_protocol;

#undef m

#endif

/*

      FONCTION RECURSIVE. Args: tableau, taille, ligne de debut, index de debut. retour, arbre correspondant

      Premier appel, taille 8, ligne 0, index 0.

      G
      G0
      G01
      G06
      G076
      G1
      M
      M320
      M6


      TRI A BULLES DE lA LISTE

      Comptage des chars 0 différents avec "" précédant : deux.

      Creation de l'arbre de retour, à deux fils.

      Pas de feuille ajoutée, pas d'incrementation de ligne.

      Itération de deux appels


          Appel ligne 0, index 1;

          comptage des chars 1 différents avec G précédement

          Creation de l'arbre de retour G, quatre fils.

          Commande 0 de taille 1 (égale à l'index), ajout de la fonction

          Iteration sur 4 appels


              Appel ligne 1, index 2

              ...

          Appel ligne 6, index 1;

          comptage des chars 1 différents avec M précédement

          Creation de l'arbre de retour M, deux fils.

          Commande 0 de taille 1 (égale à l'index), ajout de la fonction

          Iteration sur 2 appels


              Appel ligne 7, index 2;

              comptage des chars 2 différents avec M3 précédement;

              Creation de l'arbre de retour M3, un fils;

              Iteration sur 1 appel;


                  Appel ligne 7, index 3;

                  comptage des chars 3 différents avec M32 précédement;

                  Creation de l'arbre de retour M32, un fils;

                  Iteration sur 1 appel;


                      Appel ligne 7, index 4;

                      comptage des chars 4 différents avec M320 précédement;

                      Creation de l'arbre de retour M320, zero;

                      Commande 0 de taille 1 (égale à l'index), ajout de la fonction


              Appel ligne 8, index 2

              comptage des chars 2 différents avec M6 précédement;

              Creation de l'arbre de retour M6, zero fils;

              Commande 0 de taille 1 (égale à l'index), ajout de la fonction







      ALgo a partir d'une liste triée.

          arguments : line number, index.

          Get the command prexif (command from zero to index - 1)

          Count the number of different chars at position [index] in commands beginning with the prefix.

          Create a tree with the given number of children;

          If the prefix matches the line at [line number] (info may be given by the counting function),
              add the function to the tree, and increment the line number.

          Fill the tree recalling the function for every argument.

    */







