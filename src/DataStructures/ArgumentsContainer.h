//
// Created by root on 22/10/17.
//

#ifndef TRACER_ARGUMENTSCONTAINER_H
#define TRACER_ARGUMENTSCONTAINER_H


#include <cstdint>

class ArgumentsContainer {

public:

    //Constructor.
    ArgumentsContainer(uint8_t args_size, uint8_t nb_args);

    //Returns the number of empty lines in the container.
    uint8_t available_spaces();

    //Save an argument inserted with the method behind, and returns its index in the container.
    uint8_t insert_argument(char **insertion_pointer);

    //Inserts and saves an argument, and returns its index in the container.
    bool insert_argument(char *args, uint8_t *index_p);

    //Returns a pointer to an argument's first case, and gives the size.
    char* get_argument(uint8_t index);

    //Removes an argument.
    void remove_argument(uint8_t index);

    //Deletes all arguments
    void clear();


private:

    //The maximal size of an argument.
    uint8_t args_size;

    //The maximal number of arguments stored in the class.
    uint8_t nb_args;

    //The actual arguments container.
    char *arguments;

    //The number of data_spaces available in the container.
    uint8_t nb_spaces;

    //A signature for available cases : i-th bool means : 1 = available, 0 = not available.
    bool *available_cases;

    

};


#endif //TRACER_ARGUMENTSCONTAINER_H
