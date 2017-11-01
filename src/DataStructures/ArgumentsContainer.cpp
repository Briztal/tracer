#include <hardware_language_abstraction.h>
#include "ArgumentsContainer.h"
#include <interface.h>


ArgumentsContainer::ArgumentsContainer(uint8_t args_size, uint8_t nb_args) :
        args_size(args_size + (uint8_t) 1), nb_args(nb_args), available_cases(new bool[nb_args]), nb_spaces(nb_args),
        arguments(new char[(args_size + (uint8_t) 1) * nb_args]) {


}

uint8_t ArgumentsContainer::available_spaces() {
    return nb_spaces;
}

uint8_t ArgumentsContainer::insert_argument(char *args, uint8_t size) {

    if (!nb_spaces)
        return 0;

    for (uint8_t index = 0; index < nb_args; index++) {
        if (available_cases[index]) {

            //Memorise the size
            arguments[args_size * index] = (char) size;

            //Copy arguments
            memcpy(arguments + args_size * index + 1, args, size * sizeof(char));

            //Mark the line as filled
            available_cases[index] = false;
            
            //Return the index.
            return index;

        }
    }

    return 0;
}

char *ArgumentsContainer::get_argument(uint8_t index, uint8_t *size) {
    
    //Update the size
    *size = (uint8_t)arguments[args_size * index];

    //Return the address of the arg's first case.
    return arguments + args_size * index + 1;
}

void ArgumentsContainer::remove_argument(uint8_t index) {
    
    //Mark the line as available.
    available_cases[index] = true;

}
