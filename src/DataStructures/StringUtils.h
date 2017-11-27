//
// Created by root on 23/10/17.
//

#ifndef TRACER_STRINGPARSER_H
#define TRACER_STRINGPARSER_H

class StringUtils {


    //--------------------------------------Words extraction----------------------------------

public :

    //Copy a char sequence, until a limit char (or \0) is detected, and return the size of the copied word.
    static uint8_t copy_until_char(char *in_buffer, char *out_buffer, uint8_t out_buffer_size, const char limit_char);

    //Get the number of times that 'verif_char' is consecutively present, at the beginning of the char sequence.
    static uint8_t lstrip(char *in_buffer, const char verif_char);


    static uint8_t count_until_char(char *in_buffer, const char limit_char);

    static uint8_t get_nb_words(char *in_buffer);

    static uint8_t get_next_word(char *in_buffer, char *out_buffer, uint8_t out_buffer_size);



    /*
    //Function to extract next word of the command.
    static uint8_t get_next_word(char **data_in, uint8_t *command_size_p);

    //Function to extract next word of the command.
    static uint8_t get_words_nb(char *data_in, uint8_t string_size);

    //The word buffer.
    static char *word_buffer;
    static char *const word_buffer_0;

     */


};


#endif //TRACER_STRINGPARSER_H
