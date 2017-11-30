
#include <cstdint>
#include "StringUtils.h"
#include <hardware_language_abstraction.h>
#include <interface.h>


/*
 * copy_until_char : this function copies the char sequence in in_buffer in out_buffer char by char,
 *  until it reads the limit char, or \0. This char is not copied to output_buffer.
 *
 *  When the copy is done, it null-terminates the copied sequence.
 *
 */

uint8_t
StringUtils::copy_until_char(char *in_buffer, char *out_buffer, uint8_t out_buffer_size, const char limit_char) {

    //If the out buffer has size 0, return (security);
    if (!out_buffer_size)
        return 0;

    //Initialise a counter for the number of bytes copied
    uint8_t copy_size = 0;

    //Initialise a cache for the current char
    char c = *(in_buffer++);

    //decrease the out_buffer size, as a null byte is mandatory at the end of the copied word.
    out_buffer_size--;

    //While the current char is not null, or the limit char
    while (c && (c != limit_char)) {

        //If no space is available anymore in the out buffer
        if (!out_buffer_size) {

            //Null the end of the word.
            *out_buffer = 0;

            //Display an error message
            CI::echo("Warning in StringUtil::copy_until_char : the word has been copied partially, "
                             "due to the unsufficien size of the output buffer.");

            //CompleteStringParser
            return copy_size;

        }

        //Copy the current char in the current output buffer case.
        *(out_buffer++) = c;

        //Update the current char
        c = *(in_buffer++);

        //Increment the size of the copied buffer.
        copy_size++;

        //Decrement the size of the out buffer
        out_buffer_size--;

    }

    //Null terminate the copied word.
    *out_buffer = 0;

    //Return the size of the copied word.
    return copy_size;
}


/*
 * count_until_char : this function copies the char sequence in in_buffer in out_buffer char by char,
 *  until it reads the limit char, or \0. This char is not copied to output_buffer.
 *
 *  When the copy is done, it null-terminates the copied sequence.
 *
 */

uint8_t StringUtils::count_until_char(char *in_buffer, const char limit_char) {

    uint8_t char_count = 0;
    //Initialise a cache for the current char
    char c = *(in_buffer++);

    //While the current char is not null, or the limit char
    while (c && (c != limit_char)) {

        //Update the current char
        c = *(in_buffer++);

        //Increment the size of the copied buffer.
        char_count++;
    }

    //Return the number of characters before the limit char (or the end of the string
    return char_count;

}


/*
 * lstrip : this function counts the number of time that 'verif_char' is present (consecutively) at the beginning
 *  of in_buffer. For example, with in_buffer containing   '0' '0' '0' '0' '5' '2' '\0', the result of the command
 *      lstrip(in_buffer, '0') will be 4.
 *
 */

uint8_t StringUtils::lstrip(char *in_buffer, const char verif_char) {

    //initialise a counter
    uint8_t counter = 0;

    //Initialise a cache var for the current char
    char c = *(in_buffer++);

    //While the current char is not null, and is equal to 'verif_char'
    while (c && (c == verif_char)) {

        //Increment the presence counter
        counter++;

        //Update the char
        c = *(in_buffer++);

    }

    return counter;

}


/*
 * get_nb_words : this function determines the number of words in a char sequence.
 *
 */

uint8_t StringUtils::get_nb_words(char *in_buffer) {

    //Initialise a word counter.
    uint8_t nb_words = 0;

    //initialise a size for the current word.
    uint8_t size;

    do {

        //Remove unnecessary spaces at the beginning of the char sequence.
        in_buffer += lstrip(in_buffer, ' ');

        //Get the size of the next word (will ne zero only if the sequence is finished).
        size = count_until_char(in_buffer, ' ');

        //increment the data pointer at the end of the word.
        in_buffer += size;

        //Stop working if the size is zero (sequence finished)
    } while (size);

    //Return the number of words.
    return nb_words;

}


/*
 * get_next_word : this function extracts the next word of the recieved command, and saves it in word_buffer.
 *
 */

uint8_t StringUtils::get_next_word(char *in_buffer, char *out_buffer, uint8_t out_buffer_size) {

    //Initialise a counter, that will count the number of chars evaluated in this function
    uint8_t increment_counter = lstrip(in_buffer, ' ');

    //Increment the input pointer at the end of extra spaces at its beginning.
    in_buffer += increment_counter;

    //Copy the next word (without extra spaces)in out_buffer, and update the increment counter.
    increment_counter += copy_until_char(in_buffer, out_buffer, out_buffer_size, ' ');

    //Return the number of chars evaluated.
    return increment_counter;

}

