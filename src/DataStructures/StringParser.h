//
// Created by root on 23/10/17.
//

#ifndef TRACER_STRINGPARSER_H
#define TRACER_STRINGPARSER_H

class StringParser {


    //--------------------------------------Words extraction----------------------------------

public :

    //Function to extract next word of the command.
    static uint8_t get_next_word(char **data_in, uint8_t *command_size_p);

    //Function to extract next word of the command.
    static uint8_t get_words_nb(char *data_in, uint8_t string_size);

    //The word buffer.
    static char *word_buffer;
    static char *const word_buffer_0;



};


#endif //TRACER_STRINGPARSER_H
