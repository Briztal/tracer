/*
  StringUtils.h - Part of TRACER

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


#ifndef TRACER_STRINGPARSER_H
#define TRACER_STRINGPARSER_H

#include "stdint.h"

class StringUtils {


    //--------------------------------------Words extraction----------------------------------

public :

    //Get the number of times that 'verif_char' is consecutively present, at the beginning of the char sequence.
    static uint8_t lstrip(char *in_buffer, const char verif_char);

    //Get the length of a string;
    static uint8_t length(char *in_buffer);

    //Count thenumber of chars before the limit_char, or the end of the string;
    static uint8_t count_until_char(char *in_buffer, const char limit_char);

    //Get the number of words in the string;
    static uint8_t get_nb_words(char *in_buffer);

    //Compare the two strings, and return true if they are the same;
    static bool strcmp(const char *string0, const char *string1);

};


#endif //TRACER_STRINGPARSER_H
