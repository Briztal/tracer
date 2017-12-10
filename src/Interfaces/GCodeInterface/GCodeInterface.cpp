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
#include "GCodeNode.h"
#include <Project/InterfaceCommands/GCodeCommands.h>
#include <DataStructures/StringUtils.h>


//----------------------------------------------------Initialisation----------------------------------------------------

/*
 * init : this function initialises the link layer.
 *
 *  It is automatically called by the Scheduler at initialisation
 */

void GCodeInterface::init() {

    gcode_interface_link_t::begin();

    delay_ms(100);

}


//-----------------------------------------------------Data reading-----------------------------------------------------



/*
 * read_data : this function reads and processes data.
 *
 *  It reads data on the link layer, saves it, and eventually processes it.
 */

void GCodeInterface::read_data() {


    //Don't process any data if no space is available in the argument_t sequence container
    if (!arguments_storage.available_spaces())
        return;

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

                if (!TaskScheduler::available_spaces(255)) {
                    return;
                }

                //Don't process any data if no space is available in the argument_t sequence container
                if (!arguments_storage.available_spaces()) {
                    return;
                }

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

    //Declare the command id length (ex : 4 for G130).
    unsigned char command_id_length = 0;

    //Declare an array to receive the command id (must add one byte for null termination)
    char command_id[GCODE_MAX_DEPTH + 1];

    //Get the command id and its length, by calling the StringUtils.
    command_id_length = StringUtils::get_next_word(data_in, command_id, GCODE_MAX_DEPTH + 1);

    //Abort if the first word was a space.
    if (!command_id_length)
        return false;

    //Update data_in
    data_in += command_id_length;

    //Analyse the command id.
    analyse_command(command_id, command_id_length);

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
 * analyse_command : this function determines the function associated to a GCode Command.
 *
 *  If it succeeds, it calls the scheduling function, passing a pointer to the determines function.
 */

void GCodeInterface::analyse_command(char *command, unsigned char command_size) {

    //Initialise a char containing the letter we focus on
    char c = *(command++);

    //As the first letter is saved, decrease the size.
    command_size--;

    echo(String(command_size));\

    //TODO COMMENT THE PARSING


#define COMMAND(i, fname) \
    case i : \
        schedule(GCodeCommands::fname);\
        return;

#define GO_LOWER(i) \
    case i : \
        if (!(command_size--)) return;\
        c = *(command++);\
        switch(c) {\

#define GO_LOWER_COMMAND(i, fname) \
    case i : \
        echo(String(command_size));\
        if (!(command_size--)) {schedule(GCodeCommands::fname);return;}\
        c = *(command++);\
        switch(c) {\


#define GO_UPPER()\
        default : return;\
    }\

    switch (c) {

#include <Project/Config/gcode_interface_config.h>

        default:
            return;
    }

#undef COMMAND
#undef GO_LOWER
#undef GO_LOWER_COMMAND
#undef GO_UPPER

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

//TODO MAkE A SEPARATE CLASS;

/*
 * generate_tree : this function generates the TerminalNode that will be used to parse commands.
 */

GCodeNode *GCodeInterface::generate_tree() {

    

}



//TODO COMMENT
struct command_line {

    char name[GCODE_MAX_DEPTH + 1];

    String *desc;

    task_state_t (*function)(char *);

};


void


/*
 * check_prefix_presence : this function returns true if both strings have exactly the same content.
 */

bool check_prefix_presence(const char *string, const char *prefix) {

    //The index to compare in strings;
    uint8_t index = 0;

    do {

        //Get the char in the prefix;
        char p = prefix[index];

        //If the prefix is finished :
        if (!p) {

            //Complete
            return true;
        }

        //Get the char in the string
        char s = string[index];

        //If the prefix letter is not the string letter (prefix is not contained in string) :
        if (p != s) {

            //Fail;
            return false;

        }

        //No need to check that s is not null, as p is not, and p == s.

        //Set the current letter to the next one.
        index++;

    } while (true);//Return cases are already handled inside the loop.

}


/*
 * count_children : this is used to count the number of direct children of a tree represented by its prefix.
 *
 *  More explicitly, it counts the number of different chars right after [prefix], in lines of the [command]
 *      array, until it examines a string that doesn't start with [prefix].
 *      It start the counting from line [line_number], and so, all lines before [line_number] will not be counted.
 *
 *  When
 *      - the [commands] array is sorted in the alphanumeric order of its command strings;
 *
 *      This algorithm accomplished the function given at the first line of this paragraph.
 */

bool
count_children(command_line **commands, uint8_t nb_commands, uint8_t line_number, char *prefix, uint8_t *nb_children) {

    //First, we must determine the index where to search chars (the size of prefix).

    //Declare the index;
    uint8_t index = 0;

    //Increment while prefix is not ended (null terminated string).
    while (prefix[index++]);

    /*
     * As a node can contain at most 256 children, we will here declare a local array of 256 spaces,
     * to contain the found_chars characters.
     * This will not mess with the stack, as this function is only called at the program initialisation, before
     * the main loop.
     */
    char found_chars[256];

    //Initialise a variable counting lines validating the requirement.
    uint8_t children_counter = 0;

    //define a flag, showing that a line matching exactly the prefix was found_chars.
    bool matching_line = false;

    //Iterate while the last line hasn't been processed.
    while (line_number < nb_commands) {

        //Cache the name of the command;
        char *name = commands[line_number]->name;

        //If the prefix is present :
        if (check_prefix_presence(name, prefix)) {

            //Cache the char to analyse;
            char c = name[index];

            //If the char at index in string is the string end (the line matches exactly the prefix);
            if (!c) {

                //Set the matching flag;
                matching_line = true;

            } else {
                //If the name strictly contains the prefix, one potential new child has been found_chars.

                //We now must verify that the char at index was not already found_chars.

                //Declare a flag;
                bool duplicate = false;

                //For all found_chars children
                for (uint8_t found_child = 0; found_child < children_counter; found_child++) {

                    //If the char has already been encountered :
                    if (c == found_chars[found_child]) {

                        //Set the flag;
                        duplicate = true;

                        //Stop iterating, no need to search more;
                        break;

                    }
                }

                //If we found a new child :
                if (!duplicate) {

                    //Save the found char
                    found_chars[children_counter] = c;

                    //Increment the number of found children.
                    children_counter++;

                }

            }

            //Increment the current line
            line_number++;

        } else {
            //If the string doesn't contain the prefix :

            //Stop iterating.
            goto end;
        }

    }

    //A jump label for the end, to avoid duplicating the end code.
    end:

    //Update the number of children found_chars;
    *nb_children = children_counter;

    //Return the boolean flag;
    return matching_line;

}


GCodeNode *generate_tree(command_line **commands, uint8_t nb_commands, uint8_t *current_line, uint8_t index) {


    //TODO SORT AND REMOVE DUPLICATES

    //----------------------------- Prefix Computation -----------------------------

    //TODO STATIC
    char prefix[GCODE_MAX_DEPTH + 1];

    //Get the prefix;
    memcpy(prefix, commands[*current_line]->name, index * sizeof(char));

    //Null terminate the prefix;
    prefix[index] = '\0';

    //----------------------------- Prefix Computation -----------------------------

    //Local var for the number of children of the current tree.
    uint8_t nb_children = 0;

    //Count the number of children, and save the flag. It shows if the current line belongs to the current tree.
    bool matching_command = count_children(commands, nb_commands, *current_line, prefix, &nb_children);


    //Create a cache for the current line;
    command_line *line = commands[*current_line];

    //Declare the tree
    GCodeNode *tree;

    //Cache the tree's letter. Take the letter before index if it exists, and null char if not.
    char name = (index) ? prefix[index - 1] : (char) 0;

    //if the current line belongs to the current tree (processed.)
    if (matching_command) {

        //Increment the current line
        (*current_line)++;

        //Assign tree
        tree = new GCodeNode(name, nb_children, line->function);

    } else {
        //If it doesn't belong to the tree, no need to increment or register a particular function.

        //Assign tree
        tree = new GCodeNode(name, nb_children, 0);

    }

    //Increment the index, so that sub_trees focus on next chars.
    index++;

    //Fill the tree with all determined children.
    for (uint8_t child = 0; child < nb_children; child++) {

        //Build the sub_tree and add it to the current tree.
        tree->sub_nodes[child] = generate_tree(commands, nb_commands, current_line, index);

    }

    return tree;


}

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







