/*
  ArgumentsContainer.h - Part of TRACER

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


#ifndef TRACER_ARGUMENTSCONTAINER_H
#define TRACER_ARGUMENTSCONTAINER_H

#include <cstdint>

class ArgumentsContainer {

public:

    //Constructor;
    ArgumentsContainer(uint8_t args_size, uint8_t nb_args);

    //Returns the number of empty lines in the container;
    uint8_t available_spaces();

    //Save an argument inserted with the method behind, and returns its index in the container;
    uint8_t insert_argument(char **insertion_pointer);

    //Inserts and saves an argument, and returns its index in the container;
    bool insert_argument(const char *args, uint8_t *index_p);

    //Returns a pointer to an argument's first case, and gives the size;
    char* get_argument(uint8_t index);

    //Removes an argument;
    void remove_argument(uint8_t index);

    //Deletes all arguments;
    void clear();


private:

    //The maximal size of an argument;
    uint8_t args_size;

    //The maximal number of arguments stored in the class;
    uint8_t nb_args;

    //The actual arguments container;
    char *arguments;

    //The number of data_spaces available in the container;
    uint8_t nb_spaces;

    //A signature for available cases : i-th bool means : 1 = available, 0 = not available;
    bool *available_cases;

    

};


#endif //TRACER_ARGUMENTSCONTAINER_H
