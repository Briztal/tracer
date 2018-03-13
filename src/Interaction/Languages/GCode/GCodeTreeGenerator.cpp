/*
  GCodeTreeGenerator.cpp - Part of TRACER

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

#ifdef ENABLE_GCODE_INTERFACE

#include "GCodeTreeGenerator.h"

#include <Project/InterfaceCommands/GCodeCommands.h>
#include <Interaction/Interaction.h>

/*
 * generate_tree : this function generates the TerminalTree that will be used to parse commands.
 */

GCodeTree *GCodeTreeGenerator::generate_tree() {

    //Declare the command array size;
    uint16_t nb_commands;

    //Get the command array;
    command_line_t **commands = build_commands(&nb_commands);

    //Sort the commands array, and get the effective number of commands;
    sort_commands(commands, nb_commands);

    //Initialise the current line to zero;
    uint16_t current_line = 0;

    //Increment the current line while it contains a empty-string command (duplicates)
    while (!commands[current_line]->name[0]) {
        current_line++;
    }

    //Build the command tree, with the command array and its size, starting from the first line at index zero.
    GCodeTree *tree = generate_tree(commands, nb_commands, &current_line, 0);


    //Free all temporary commands, passing the original size;
    free_commands(commands, nb_commands);

    return tree;

}

/*
 * build_commands : this function creates the command_line_t array in the heap, according to the configuration file.
 */

command_line_t **GCodeTreeGenerator::build_commands(uint16_t *nb_commands_p) {

    //Get the number of commands;
    uint16_t nb_commands = get_command_nb();

    //The command array;
    command_line_t **commands = new command_line_t *[nb_commands];

    //A local command line;
    command_line_t *command_line;

    //The current command index;
    uint16_t i = 0;

    //Define a macro that will create a new command line, and fill its data with passed parameters.
#define GCODE_COMMAND(name_, function_)\
        commands[i++] = command_line = new command_line_t();\
        command_line->function = GCodeCommands::function_;\
        save_command_name(command_line, string(#name_).data());\

//Create all commands
#include <Config/controller_gcode_config.h>

    //Un-define the macro for safety;
#undef GCODE_COMMAND

    //update the number of commands;
    *nb_commands_p = nb_commands;

    return commands;

}

void GCodeTreeGenerator::save_command_name(command_line_t *command, const char *name) {

    //Cache the command name pointer;
    char *n = command->name;

    //Cache for the current char;
    char c;

    for (uint8_t index = 0; index < GCODE_MAX_DEPTH; index++) {

        //Copy the current char and get its value;
        c = *(n++) = *(name++);

        //If we copied the null terminating the string, terminate.
        if (!c) {
            break;
        }
    }
}


/*
 * free_commands : this function frees each line the command_line_t array, and the array.
 */

void GCodeTreeGenerator::free_commands(command_line_t **commands, const uint16_t nb_commands) {

    //Iterate over every command :
    for (uint16_t command = 0; command < nb_commands; command++) {

        //Free the command;
        delete commands[command];

    }

    //Free the array;
    delete[] commands;

}


/*
 * get_command_nb : this function returns the number of GCode commands required by the config file.
 */

uint16_t GCodeTreeGenerator::get_command_nb() {

    //Initialise the counter
    uint16_t i = 0;

//First, define  macro counting the number of macro call
#define GCODE_COMMAND(name, function) i++;

    //Include the configuration file;
#include <Config/controller_gcode_config.h>

    //Un-define the macro for safety
#undef GCODE_COMMAND

    //Return the number of GCode commands;
    return i;

}

void GCodeTreeGenerator::print_tree(const GCodeTree *tree) {

    GCode::log(string(tree->name));

    for (uint8_t child_id = 0; child_id < tree->nb_children; child_id++) {

        bool b = false;

        const GCodeTree *child = tree->get_child(child_id, &b);

        if (b) {
            print_tree(child);
        } else {
            std_out("Error in GCodeTreeGenerator::print_tree : the child "+string(child_id)+" is not assigned");
        }
    }

}
//-------------------------------------------------------- Sort --------------------------------------------------------

/*
 * sort_commands : this command accomplishes the two following purposes :
 *  - sorting the command array;
 *  - in case of commands duplicates (error from the coder), it nullifies the command's name,
 *      and puts it at the beginning of the array
 */

void GCodeTreeGenerator::sort_commands(command_line_t **commands, uint16_t nb_commands) {

    /*
     * To accomplish our two goals, we will use a reversed bubble sort (make elements prepare_movement to the end of the array).
     *
     * This sorting algorithm is adapted to the process.
     */

    //The minimum index we must check. Below this index are stored null commands.
    uint16_t minimum_index = 0;

    const uint16_t maximum_index = nb_commands - (uint16_t) 1;

    //The initial index of the sort limit;
    uint16_t sorted_index = nb_commands - (uint16_t) 2;

    //Iterate from the end of the array to the limit of the null-zone (the comparison is made at the end.
    do {

        //Local variable for the moving command index;
        uint16_t moving_index = sorted_index;

        //Cache the current name;
        char *const current_name = commands[moving_index]->name;

        //Make the current command fall until it reaches its place.

        //For each command after the current index :
        while (moving_index < maximum_index) {


            //Cache the next index;
            uint16_t next_index = moving_index + (uint16_t) 1;

            //Compare the two commands
            comp_result_t res = command_comparison(current_name, commands[next_index]->name);

            switch (res) {

                //If the moving command is still greater than the switch command:
                case greater:


                    //We still have to make the moving command fall of on index
                    switch_commands(commands, moving_index, next_index);

                    //Increment the moving index
                    moving_index++;

                    //Continue;
                    break;

                    //If the moving command arrives at its final position:
                case smaller:

                    //This command's fall is finished. Jump to the fall end label.
                    goto iteration_limit;

                    //If both command have the same name :
                case equal:

                    /*
                     * THIS CASE CORRESPONDS TO A PROGRAMMER'S ERROR.
                     *
                     * The moving command will be nullified, and moved to the beginning of the array.
                     * //TODO CATCH THIS ERROR !
                     */

                    //First, nullify the command;
                    *current_name = '\0';

                    //Next, we must prepare_movement the moving command back to its initial position.

                    //While we are not at the initial position
                    while (moving_index != sorted_index) {

                        //Make the moving command go upper
                        switch_commands(commands, moving_index, moving_index - (uint16_t) 1);

                        //Decrease moving_index;
                        moving_index--;

                    }

                    //If the sorted_index was already the last one :
                    if (minimum_index == sorted_index) {

                        //The program can end, the nullified command is already in the null-zone
                        return;
                    }

                    //Now, to avoid unnecessary switches, we switch the moving command with the first non-nul command.
                    switch_commands(commands, minimum_index, sorted_index);

                    //As another command is now present in the null-zone, increase its size;
                    minimum_index++;

                    //We still need to sort (at least) the command that was switched at the sorting index.

                    //Restart this iteration of the sort with the new command : increase sorted index and reiterate;
                    sorted_index++;
                    goto iteration_limit;


            }

        }

        //A jump label, to go to the end of a fall directly from the switch statement.
        iteration_limit:
        asm("nop");

    } while (sorted_index-- != minimum_index);

}

/*
 * command_comparison : this command will compare commands names using the alphanumeric order, and return :
 *
 *  - smaller if command_0's name is smaller than command_1;
 *  - greater if command_1's name is greater than command_1;
 *  - equal if both command have the same name,
 */

comp_result_t GCodeTreeGenerator::command_comparison(char *command_0, char *command_1) {

    //Caches for name pointers

    while (true) {

        //Cache for values
        char v0 = *(command_0++), v1 = *(command_1++);

        if (v0 < v1) {
            //If the command 0 contains a char smaller than command_0 :

            //return the smaller state;
            return smaller;

        } else if (v0 > v1) {
            //If the command 0 contains a char greater than command_0 :

            //Return the greater state;
            return greater;

        } else {
            //If both commands have the same char:

            //If both chars are null (end)
            if (!v0) {

                //Both are equal.
                return equal;

            }

            //If not, we must compare the next character.

        }

    }

}


/*
 * switch_commands : this simple function switches two commands.
 */

void GCodeTreeGenerator::switch_commands(command_line_t **commands, uint16_t index_0, uint16_t index_1) {

    //Cache the element at index_0
    command_line_t *temp = commands[index_0];

    //Copy the element at index_1 to index_0
    commands[index_0] = commands[index_1];

    //Copy the saved element at index_1;
    commands[index_1] = temp;

}

//--------------------------------------------------- Tree creation ----------------------------------------------------



GCodeTree *GCodeTreeGenerator::generate_tree(command_line_t **commands, uint16_t nb_commands, uint16_t *current_line,
                                             uint8_t index) {


    //----------------------------- Prefix Computation -----------------------------

    //A char array that will contain the prefix. It will be static, and declared only once.
    static char prefix[GCODE_MAX_DEPTH + 1]{0};

    //Get the prefix;
    memcpy(prefix, commands[*current_line]->name, index * sizeof(char));

    //Null terminate the prefix;
    prefix[index] = '\0';

    //----------------------------- Prefix Computation -----------------------------

    //Local var for the number of nb_children of the current tree.
    uint8_t nb_children = 0;

    //Count the number of nb_children, and save the flag. It shows if the current line belongs to the current tree.
    bool matching_command = count_children(commands, nb_commands, *current_line, prefix, &nb_children);


    //Create a cache for the current line;
    command_line_t *line = commands[*current_line];

    //Declare the tree
    GCodeTree *tree;

    //Cache the tree's letter. Take the letter before index if it exists, and null char if not.
    char name = (index) ? prefix[index - 1] : (char) 0;

    //if the current line belongs to the current tree (processed.)
    if (matching_command) {

        //Increment the current line
        (*current_line)++;

        //Assign tree
        tree = new GCodeTree(name, nb_children, line->function);

    } else {
        //If it doesn't belong to the tree, no need to increment or register a particular function.

        //Assign tree
        tree = new GCodeTree(name, nb_children, 0);

    }

    //Increment the index, so that nb_children focus on next chars.
    index++;

    //Fill the tree with all determined nb_children.
    for (uint8_t child_id = 0; child_id < nb_children; child_id++) {

        //Build the sub_tree and add it to the current tree.
        GCodeTree *child_tree = generate_tree(commands, nb_commands, current_line, index);

        //Initialise a flag to check the child set execution.
        bool tree_flag = false;

        //Set the child of the tree;
        tree->set_child(child_id, child_tree, &tree_flag);

        //If the child set encountered an error
        if (!tree_flag) {

            //Log
            std_out("Error in GCodeTreeGenerator::generate_tree : the child was already assigned, or un-assignable. ");

        }


    }

    return tree;


}


/*
 * count_children : this is used to count the number of direct nb_children of a tree represented by its prefix.
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
GCodeTreeGenerator::count_children(command_line_t **commands, uint16_t nb_commands, uint16_t line_number, char *prefix,
                                   uint8_t *nb_children) {

    //First, we must determine the index where to search chars (the size of prefix).

    //Declare the index;
    uint8_t index = 0;

    //Increment while prefix is not ended (null terminated string).
    while (prefix[index]) {
        index++;
    }

    /*
     * As a node can contain at most 256 nb_children, we will here declare a local array of 256 spaces,
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

                //For all found_chars nb_children
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

                    //Increment the number of found nb_children.
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

    //Update the number of nb_children found_chars;
    *nb_children = children_counter;

    //Return the boolean flag;
    return matching_line;

}


/*
 * check_prefix_presence : this function returns true if both strings have exactly the same content.
 */

bool GCodeTreeGenerator::check_prefix_presence(const char *string, const char *prefix) {

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

#endif