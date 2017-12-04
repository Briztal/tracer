/*
  Queue.h - Part of TRACER

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
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef TRACER_QUEUE_H
#define TRACER_QUEUE_H

#include <stdint.h>

template<typename T>
class Queue {


    /*
     * A macro to safely decrement an index :
     *  - if the index is zero, it takes the maximal value;
     *  - f not, it is decremented;
     */

#define SAFE_DECR(i)\
    if (i) {\
        i--;\
    } else {\
        i = max_indice;\
    }\


public:

    /*
     * Constructor : it takes the buffer size in only argument_t.
     */

    Queue(uint8_t size) :
            size(size), content(new T[size]), max_indice((const uint8_t) (size - 1)),
            spaces(size) {
    }


    //---------------------------------------------------Dequeue--------------------------------------------------------

    /*
     * dequeue : this method reads the output element, discards and returns it.
     *
     */

    T dequeue() {


        T element = content[output_index];

        discard();

        return element;

    }


    /*
     * read_output : this method returns a pointer to the output element.
     *
     */

    T *read_output() {

        return content + output_index;
    }


    /*
     * discard : this method discards the output element
     *
     */

    void discard() {

        if (elements) {

            SAFE_DECR(output_index);

            elements--;
            spaces++;

        }
    }

    //---------------------------------------------------Enqueue--------------------------------------------------------

    /*
     * enqueue_object : this method copies the element provided object to the current input case,
     *      and updates the input index.
     *
     */

    void enqueue_object(T element) {

        if (!spaces)
            return;

        content[input_index] = element;

        SAFE_DECR(input_index);

        elements++;
        spaces--;

    }


    /*
     * process : this method updates the input index. The input case must have been filled before.
     *
     */

    void enqueue() {

        if (!spaces)
            return;

        SAFE_DECR(input_index);

        elements++;
        spaces--;

    }


    /*
     * get_input_ptr : this method returns a pointer to the current input case
     *
     */

    T *get_input_ptr() {

        return content + input_index;
    }


    /*
     * read_previous_input_ptr : this method returns a pointer to the previous input case
     *
     */

    T *read_previous_input_ptr() {
        if (input_index != max_indice) {
            return content + input_index + 1;
        } else {
            return content;
        }
    }


    /*
    * read_input_ptr_offset : this method returns a pointer to the case that was the input case [i] inputs ago.
    *
    */

    T *read_input_ptr_offset(uint8_t i) {
        if (input_index + i <=  max_indice) {
            return content + input_index + i;
        } else {
            return content + i - (max_indice - input_index) -1;
        }
    }


    //-------------------------------------------------Queue state------------------------------------------------------


    uint8_t available_spaces() {
        return spaces;
    }

    uint8_t available_elements() {
        return elements;
    }

    const uint8_t pull_indice() {
        return output_index;
    }

    const uint8_t push_indice() {
        return input_index;
    }


    //------------------------------------------------Queue fields------------------------------------------------------


private:

    const uint8_t size;
    const uint8_t max_indice;

    T *const content;

    uint8_t output_index = 0;
    uint8_t input_index = 0;

    uint8_t elements = 0;
    uint8_t spaces;

};


#endif //TRACER_QUEUE_H
