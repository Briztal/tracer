#include <hardware_language_abstraction.h>
#include "ArgumentsContainer.h"
#include <interface.h>


//TODO COMMENT
ArgumentsContainer::ArgumentsContainer(uint8_t args_size, uint8_t nb_args) :
        args_size(args_size), nb_args(nb_args), available_cases(new bool[nb_args]), nb_spaces(nb_args),
        arguments(new char[args_size * nb_args]) {


}

uint8_t ArgumentsContainer::available_spaces() {
    return nb_spaces;
}


uint8_t ArgumentsContainer::insert_argument(char ** insertion_pointer) {

    if (!nb_spaces)
        return 0;

    //For every line in the storage
    for (uint8_t index = 0; index < nb_args; index++) {

        //If the line is available
        if (available_cases[index]) {

            //Update the insertion pointer.
            *insertion_pointer = arguments + args_size * index;

            //Mark the line as filled.
            available_cases[index] = false;

            //Return the index.
            return index;

        }
    }
}


bool ArgumentsContainer::insert_argument(char *args, uint8_t *index_p) {

    if (!nb_spaces)
        return false;

    //For every line in the storage
    for (uint8_t index = 0; index < nb_args; index++) {

        //If the line is available
        if (available_cases[index]) {

            //------------------------Argument Copy----------------------------

            //Declare a variable to control that we never exceed the line's capacity.
            uint8_t free_size = args_size;

            //Cache for the current char.
            char c = *(args++);

            //Cache for the destination.
            char *dest = arguments + args_size * index;

            //While the char isn't null.
            while (c) {

                //if no more space is available on the line
                if (!free_size) {

                    //Display an error message
                    CI::echo("WARNING IN ArgumentsContainer::insert_argument : the argument was too big"
                                     " for the container.");

                    //Fail
                    return false;

                }

                //Copy the current char.
                *dest++=c;

                //update the current char.
                c = *(args++);

                //Decrement the available size in the line.
                free_size--;

            }

            //Nullify the end.
            *dest = 0;

            //------------------------/Argument Copy----------------------------


            //Mark the line as filled.
            available_cases[index] = false;

            //Update the index
            *index_p = index;

            //Complete.
            return true;

        }
    }

    return false;
}

char *ArgumentsContainer::get_argument(uint8_t index) {

    //Return the address of the arg's first case.
    return arguments + args_size * index;
}


void ArgumentsContainer::remove_argument(uint8_t index) {

    //Mark the line as available.
    available_cases[index] = true;

}

void ArgumentsContainer::clear() {

    //Mark all cases available.
    memset(available_cases, true, sizeof(bool));
}

