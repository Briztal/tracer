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
#include <config.h>

#ifdef ENABLE_GCODE_INTERFACE

#include "GCodeInterface.h"
#include "GCodeTreeGenerator.h"
#include <DataStructures/StringUtils.h>
#include <Interfaces/Interfaces.h>


//----------------------------------------------------Initialisation----------------------------------------------------

/*
 * setup_hardware : this function initialises the link layer.
 *
 *  It is automatically called by the Kernel at initialisation
 */

void GCodeInterface::initialise_hardware() {

    //Initialise the data link;
    gcode_interface_link_t::begin();

    //Wait a bit for data link init to complete.
    delay_ms(100);

}


/*
 * initialise_data : this function resets initialises all data of the class
 */

void GCodeInterface::initialise_data() {
    
    //Command Parsing initialisation;
    reset();

    //Arguments initialisation;
    arguments_storage.clear();
    
    
    //Argument parsing initialisation;
    nb_identifiers = 0;
    
    
    //Tree initialisation.

    //delete the current tree;
    delete(command_tree);
    
    //Create a new command tree;
    command_tree = GCodeTreeGenerator::generate_tree();


}


//-----------------------------------------------------Data reading-----------------------------------------------------

/*
 * read_data : this function reads and processes data.
 *
 *  It reads data on the link layer, saves it, and eventually processes it.
 */

bool GCodeInterface::read_data() {


    //Don't process any data if no space is available in the argument_t sequence container
    if (!arguments_storage.available_spaces()) {

        //Return true if the data_link buffer is not empty.
        return (bool) gcode_interface_link_t::available();

    }

    while (gcode_interface_link_t::available()) {

        //Read the data link
        char read_char = gcode_interface_link_t::read();

        //If the recieved char is a line feed or a working_extruder return
        if ((read_char == 10) || (read_char == 13)) {

            //If a non empty uncorrupted packet has effectively been received
            if (command_size && !corrupted_packet) {

                //Parse the GCode
                parse();

                //Reset the data_in
                reset();

                //Return true if the data_link buffer is not empty.
                return (bool)gcode_interface_link_t::available();

            }

            //If the received packet was too long for the input buffer :
            if (corrupted_packet) {

                //Display an error message
                GI::echo("WARNING in TerminalInterface::read_data : the received packet was too long for "
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

    //No more data available;
    return false;
}


/*
 * reset : this function resets the parsing structure;
 */

void GCodeInterface::reset() {

    //No data;
    command_size = 0;

    //Clear the corruption flag;
    corrupted_packet = false;

    //Maximum numbers of char nb_spaces;
    data_spaces = GCODE_MAX_SIZE;

    //data insertion at the origin;
    data_in = data_in_0;
}


//---------------------------------------------------------Parse--------------------------------------------------------


/*
 * parse : this function parses the received GCode Command.
 *
 *  It is called when a GCode command has been entirely received
 */

bool GCodeInterface::parse() {

    //Initialise the parsing.
    init_parsing();


    //Remove extra spaces;
    data_in += StringUtils::lstrip(data_in, ' ');
    
    //Save the pointer to the first char of the command id.
    char *command_id = data_in;
    
    //Get the command id length;
    uint8_t command_id_length = StringUtils::count_until_char(data_in, ' ');
    
    //Abort if the first word was a space.
    if (!command_id_length)
        return false;

    //Update data_in
    data_in += command_id_length;

    //Analyse the command id.
    schedule_command(command_id);

    return true;

}


/*
 * init_parsing : this function initialises the data parsing.
 *
 *  It positions the data pointer to the beginning of the command, and resets all parameters flags.
 */

void GCodeInterface::init_parsing() {

    //Mark the end of the the received command
    *data_in = 0;

    //Reset the data pointer
    data_in = data_in_0;

}


/*
 * schedule_command : this function analyses the packet received, and parses the command part.
 *
 *  When it has found the GCodeCommand identified by this string, it saves the rest of the packet (the arguments part)
 *      in the argument storage, and then, schedules the execution of this function.
 *
 *  This command will be executed, by the intermediary of the execute_command function, defined below.
 *      (see the doc above the execute_command function's definition for more explanations).
 */

void GCodeInterface::schedule_command(char *command) {
    
    //Initialise the current current_tree to the root;
    const GCodeTree *current_tree = command_tree;

    CI::echo("COMMAND : "+String(command));
    //The current depth
    uint8_t depth = 0;

    //Must declare the int before the jump label.
    uint8_t i;

    //A cache for the child of the current node we are focused on.
    const GCodeTree *current_child;

    //The current char in the command;
    char command_id;
    //--------------------------Tree Iteration--------------------------

    //As the algorithm involves two for - while loops, we must use a goto label to restart with a new node.
    node_check:

    //Get the command identifier, and increment the depth;
    command_id = command[depth++];


    //We now must compare each child of the current node, and compare its identifier with the read command identifier.

    //Check every sub_node
    for (i = 0; i < current_tree->nb_children; i++) {

        //We now need to get the child.

        //Declare a flag;
        bool tree_flag = false;

        //Update the current sub_node
        current_child = current_tree->get_child(i, &tree_flag);

        const char tree_id = current_child->name;

        //If the current command_identifier matches the current_tree's name
        if (command_id == tree_id) {

            //A matching sub_node has been found. It can be a single
            //Update the current node and the current children array.
            current_tree = current_child;

            //if the new node is not a leaf, check sub nodes
            if (current_tree->nb_children) {

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
                        data->function = current_tree->function;

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

    //If the command_id didn't match any know child tree, return;

}

/*
 * schedule : this function schedules a GCodeCommands command.
 *
 *  It saves the arguments part in the argument storage, and then, schedules the execution of the function passed
 *      in arguments
 *
 *  This command will be executed, by the intermediary of the execute_command function, defined below.
 *      (see the doc above the execute_command function's definition for more explanations).
 */

void GCodeInterface::schedule(task_state_t (*f)(char *)) {

    //Save the current arguments
    uint8_t index;

    //Copy the argument string in the argument storage, and get the index it was inserted at.
    arguments_storage.insert_argument(data_in, &index);

    //Create a struct in the heap to contain argument-related data.
    interface_data_t *data = new interface_data_t();
    data->arguments_index = index;
    data->function = f;

    //Schedule the task
    TaskScheduler::schedule_task(255, execute_command, (void *) data);

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

task_state_t GCodeInterface::execute_command(void *data_pointer) {

    //Get the terminal interface data back
    interface_data_t *data = (interface_data_t *) data_pointer;

    //Cache var for arguments index.
    uint8_t arguments_index = data->arguments_index;

    //Cache for arguments.
    char *arguments = arguments_storage.get_argument(arguments_index);

    //Execute the required TerminalCommand function, and get the execution state
    const task_state_t state = (*data->function)(arguments);

    /*remove arguments arguments, if the task mustn't be reprogrammed*/
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

void GCodeInterface::confirm_command_execution(const interface_data_t *data) {

//The log will occur only if the command flag is set.

#ifdef GCODE_EXECUTION_CONFIRMATION

    //Switch the return state.
    switch(data->return_state) {

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
            //CI::echo("Command Reprogrammed.");
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
 * parse_arguments : this function parses an argument sequence, whose format is like below :
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

bool GCodeInterface::parse_arguments(char *args_current_position) {

    //First, reset the argument parsing structure :

    //reset the arguments counter
    nb_identifiers = 0;

    do {

        //First, remove unnecessary nb_spaces;
        uint8_t dec = StringUtils::lstrip(args_current_position, ' ');

        args_current_position += dec;
        //The current position is now on an argument identifier, or the argument sequence's end.

        //Get the argument identifier.
        char argument_identifier = *args_current_position;

        //If we reached the end of the argument string :
        if (!argument_identifier) {

            //Complete;
            return true;

        }

        //Now we are sure to be parsing a new argument.

        //If the argument container is already full;
        if (nb_identifiers == GCODE_MAX_ARGS_NB) {

            //Display an error message
            CI::echo("The TerminalInterface hasn't been configured to accept more than " + String(GCODE_MAX_ARGS_NB) +
                     " arguments. Please check your terminal_interface_config.h file.");

            //Fail
            return false;

        }

        //We are now sure not to be at the string's end position.
        //The eventual value is just after the current position.

        //Increment the current position (legit, as the string is not finished) and cache the new value.
        char *arg = ++args_current_position;

        //Make a local flag, that will determine if the parsing keeps on after the current argument.
        bool finished;


        //If the string end here (no value provided, and string end);
        if (!*arg) {

            //The argument is already at '\0' position, no modification is required.

            //Plan the stop of the loop
            finished = true;

            //Enqueue an empty argument
            goto insert_arg;

        }

        //Go to the word's end (if we are already, we will simply not move the position.
        args_current_position += StringUtils::count_until_char(args_current_position, ' ');

        //If the word's end is '\0' (String terminated), plan the loop stop.
        finished = (!*args_current_position);

        //Nullify the word's end (reason why we cached the bool.
        *args_current_position = '\0';

        //Shift the current position
        args_current_position++;

        //---------------------------------- Argument saving ----------------------------------

        insert_arg:

        //Save the relation between the identifier and the argument location.
        argument_t *argument = arguments + nb_identifiers;
        argument->identifier = argument_identifier;
        argument->arg = arg;

        //Increase the number of parsed arguments
        nb_identifiers++;

        //If we have reached the end of the sequence
        if (finished) {

            //Complete
            return true;

        }


    } while (true);


}


/*
 * get_argument_value : this function presupposes that the argument referenced
 *  by the identifier id is a numeric value, and returns that value.
 */

float GCodeInterface::get_argument_value(char id) {

    //Get the argument pointer.
    char *ptr = get_argument(id);

    //return the float equivalent.
    if (ptr != nullptr) {

        return str_to_float(ptr);

    } else return 0;

}


/*
 * get_argument : this function returns a pointer to the argument (char *) referenced
 *  by the identifier id.
 */

char *GCodeInterface::get_argument(char id) {

    //For every identifier
    for (uint8_t i = 0; i < nb_identifiers; i++) {

        //Cache for the link
        argument_t *link = arguments + i;

        //If the links'identifier matches the provided one
        if (link->identifier == id) {

            //Get the location of the argument (from the link), and convert the argument into a float.
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

bool GCodeInterface::verify_all_identifiers_presence(const char *identifiers) {

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

bool GCodeInterface::verify_one_identifiers_presence(const char *identifiers) {

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
 * verify_all_identifiers_presence : this function return true if the identifier id has been extracted
 *  during the previous parsing.
 */

bool GCodeInterface::verify_identifier_presence(char id) {

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


//--------------------------------------------------- Tree Generation --------------------------------------------------


//----------------------------------------------------System aliases----------------------------------------------------


/*
 * echo : this function is an alias for the system echo command.
 *
 *  It echoes text data on the link layer
 */

void GCodeInterface::echo(const string_t msg) {

    gcode_interface_link_t::send_str(msg + "\n");

}

//-------------------------------------------Static declarations / definitions------------------------------------------

#define m GCodeInterface




//Arguments container

ArgumentsContainer m::arguments_storage = ArgumentsContainer(GCODE_MAX_ARGS_NB * (GCODE_MAX_WORD_SIZE + 4) + 1,
                                                             GCODE_MAX_PENDING_COMMANDS);


//Identifiers in a parsed argument_t sequence
argument_t t_gcode_arguments[GCODE_MAX_ARGS_NB];
argument_t *const m::arguments = t_gcode_arguments;

//Number of arguments in a sequence
uint8_t m::nb_identifiers = 0;

//The command size
unsigned char m::command_size;

//The current number of available nb_spaces in the data bugger
uint8_t m::data_spaces = GCODE_MAX_SIZE;

//A flag set if the current packet is corrupted (too long for the data buffer)
bool m::corrupted_packet = false;

//Data pointers
char tdatain_gcode[GCODE_MAX_SIZE];
char *m::data_in = tdatain_gcode;
char *const m::data_in_0 = tdatain_gcode;

//Create an empty temporary tree;
GCodeTree *m::command_tree = new GCodeTree(' ', 0, nullptr);

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







