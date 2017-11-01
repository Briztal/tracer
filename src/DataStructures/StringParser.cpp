
#include <cstdint>
#include "StringParser.h"
#include <hardware_language_abstraction.h>
#include <interface.h>

/*
 * get_words_nb : this function will count the number of words in the string provided in arguments
 *
 */

unsigned char StringParser::get_words_nb(char *data_in, uint8_t string_size) {

    //If the string is empty, return zero.
    if (!string_size)
        return 0;

    //If not, initialise a counter and a boolean to determine if the last char evaluated was a space.
    uint8_t words_counter = 0;
    bool last_was_space = *(data_in++) == ' ';

    //If the first letter is not a space, increase the word counter
    if (!last_was_space) {
        words_counter++;
    }

    //As the first letter was evaluated, decrease the command size
    string_size--;

    //Evaluate every letter
    for (;string_size--;) {

        if (*(data_in++) == ' ') {
            //If the current letter is a space, then set the space flag
            last_was_space = true;
        } else {

            //If the current letter is not a space :
            if (last_was_space) {

                //if the last letter was a space, then we currently evaluate the beginning of a word.
                words_counter++;

                //clear the space flag
                last_was_space = false;
            }
        }
    }

    return words_counter;

}


/*
 * get_next_word : this function extracts the next word of the recieved command, and saves it in word_buffer.
 *
 */

unsigned char StringParser::get_next_word(char **data_in_p, uint8_t *command_size_p) {

    char *data_in = *data_in_p;

    uint8_t command_size = *command_size_p;

    word_buffer = word_buffer_0;

    if (!command_size) {
        *word_buffer = 0;
        *data_in_p = data_in;
        *command_size_p = 0;
        return 0;
    }

    while ((*word_buffer = *(data_in++)) == ' ') {

        //Stop the iteration if no more chars are available
        if (!command_size) {
            *word_buffer = 0;
            *data_in_p = data_in;
            *command_size_p = 0;
            return 0;
        }

        command_size--;
        //copy the current char and re-iterate if it is a space.

    }

    //One non-space char has been added, so increment the size and the word buffer
    unsigned char size = 1;
    word_buffer++;
    command_size--;

    //local for the current char
    char t;

    //While there still are chars to process, and that the current char is not a space
    while ((command_size) && ((t = *(command_size--, data_in++)) != ' ')) {

        //Append the char to the current word
        *(word_buffer++) = t;

        //Increment the size
        size++;

    }

    //Mark the end of the word, for strcmp.
    *word_buffer = 0;

    *data_in_p = data_in;
    *command_size_p = command_size;
    return size;

}



char twrd[MAX_WORD_SIZE];
char *StringParser::word_buffer = twrd;
char *const StringParser::word_buffer_0 = twrd;
