/*
  StringUtils.cpp - Part of TRACER

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

#include <HardwareSerial.h>
#include <Interaction/Interaction.h>
#include "StringUtils.h"

/*
 * length : this function determines the length of the string (without the \0 comprised);
 */

uint8_t StringUtils::length(const char *in_buffer) {

    uint8_t char_count = 0;

    //While the current char is not null;
    while (*(in_buffer++)) {

        //Increment the size of the copied buffer.
        char_count++;
    }

    //Return the number of characters before the end of the string
    return char_count;

}


//TODO RE-COMMENT, THIS IS THE OLD FUNCTION

/*
 * count_until_char : this function copies the char sequence in in_buffer in out_buffer char by char,
 *  until it reads the limit char, or \0. This char is not copied to output_buffer.
 *
 *  When the copy is done, it null-terminates the copied sequence.
 *
 */

uint8_t StringUtils::count_until_char(const char *in_buffer, const char limit_char) {

    uint8_t char_count = 0;
    //Initialise a cache for the current char
    char c = *(in_buffer++);

    //While the current char is not null, or the limit char
    while (c && (c != limit_char)) {

        std_out("SUUS");

        return 0;

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

uint8_t StringUtils::lstrip(const char *in_buffer, const char verif_char) {

    //initialise_hardware a counter
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

uint8_t StringUtils::get_nb_words(const char *in_buffer) {

    //Initialise a word counter.
    uint8_t nb_words = 0;

    //initialise_hardware a size for the current word.
    uint8_t size;

    do {

        //Remove unnecessary nb_spaces at the beginning of the char sequence.
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
 * strcmp : compares the two provided strings, and return true if they are equal;
 */

bool StringUtils::strcmp(const char *string0, const char *string1) {

    //Comparison loop;
    while(true) {

        //Cache the current char of string0 and increment string0;
        char c = *(string0++);

        //Compare with the current char of string1 and increment string1. If chars are different :
        if (c != *(string1++)) {

            //Found two different chars at the same index -> not equal;
            return false;

        }

        //Now, we are shure that both chars are the same;

        //If the current char is a null termination :
        if (!c) {

            //End of string found for both strings : both strings are the same;
            return true;

        }

        //If not, re-iterate;

    }

}

