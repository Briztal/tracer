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

#include <DataStructures/string/tstring.h>

template<typename T>
class Queue {

    /*
     * A macro to safely decrement an index :
     *  - if the index is zero, it takes the maximal value;
     *  - f not, it is decremented;
     */

#define SAFE_DECR(i)\
    if (i) {\
        (i)--;\
    } else {\
        (i) = max_index;\
    }\


private:

/*
 * This struct will be the basic object we will store in the queue.
 */
    struct queue_object_t {

        //The effective object we want to store.
        T object;

        //A flag, to notify whether the object has been allocated.
        bool allocated = false;

    };

    //----------------------------------------------- Queue fields -----------------------------------------------------

private:

    //The size of the queue;
    const uint8_t size;

    //The maximum index (size - 1);
    const uint8_t max_index;

    //The array of queue objects;
    queue_object_t *const content;

    //The reading and insertion indices, initialised at zero;
    uint8_t reading_index = 0;
    uint8_t insertion_index = 0;

    //The number of objects and spaces in the queue;
    uint8_t nb_objects = 0;
    uint8_t nb_spaces;


    //----------------------------------------- Constructor, destructor -----------------------------------------

public:
    /*
     * Constructor : initialises all data
     */
    explicit Queue(uint8_t size) :
            size(size), max_index((const uint8_t) (size - 1)), content(new queue_object_t[size]), nb_spaces(size) {}


    /*
     * Destructor : deletes the data array;
     */
    ~Queue() {
        delete[] content;
    }
    //--------------------------------------------- Vars display ---------------------------------------------


    tstring display() {

        return tstring("\ninput ") + insertion_index + " output " + reading_index + " nb_objects " +
               nb_objects + " nb_spaces " + nb_spaces;

    }

    //---------------------------------------------Read Object Manipulation---------------------------------------------


    /*
     * get_read_adress : this method returns a pointer to the output element.
     *
     */

    T *get_reading_address(bool *success) {

        //First, cache the address of the queue object at the reading index;
        queue_object_t *reading_object = content + reading_index;

        //Update the success flag;
        *success = reading_object->allocated;

        //Return an actual object only if the object is allocated:
        if (*success) {
            return &reading_object->object;
        }

        return nullptr;
    }


    /*
    * read_inserted_object : this method returns a pointer to the case that was the input case [i] inputs ago.
    *
    */

    T *read_inserted_object(uint8_t i, bool *success) {

        //First, cache the new index.
        uint8_t new_index = insertion_index + i;

        //As the offset is unknown, the first part is gonna be to convert the index into one that fits in the buffer.

        //As the index must necessarily be decreased, while it is superior to the size, decrease it of the size.
        while (new_index >= size) {

            //Decrease;
            new_index -= size;

        }

        //Now, cache the address of the queue object at the computed index;
        queue_object_t *reading_object = content + new_index;

        //Update the success flag;
        *success = reading_object->allocated;

        //Return an actual object only if the object is allocated:
        if (*success) {
            return &reading_object->object;
        }

        //If the object was not allocated, fail.
        return nullptr;

    }


    /*
     * discard_sub_movement : this method discards the current reading element
     */

    void discard(bool *success) {

        //First, cache the address of the queue object at the reading index.
        queue_object_t *reading_object = content + reading_index;

        //Update the success flag;
        *success = reading_object->allocated;

        //Only modify the state variables if the object is allocated
        if (*success) {

            reading_object->allocated = false;

            //update the reading index
            SAFE_DECR(reading_index);

            //Modify the state variables
            nb_objects--;
            nb_spaces++;


        }

    }


    //------------------------------------------Insertion Object Manipulation-------------------------------------------

    /*
     * get_input_ptr : this method returns a pointer to the current input case
     *
     */

    T *get_insertion_address(bool *success) {

        //First, cache the address of the queue object at the reading index;
        queue_object_t *insertion_object = content + insertion_index;

        //Update the success flag.
        *success = !insertion_object->allocated;

        //Return an actual object only if the object is not:
        if (*success) {
            return &insertion_object->object;
        }

        return nullptr;
    }


    /*
     * insert_object : this method copies the element provided object to the current input case,
     *      and updates the input index.
     *
     */

    void insert_object(T *object, bool *success) {

        //First, cache the address of the queue object at the reading index;
        queue_object_t *insertion_object = content + insertion_index;

        //Update the success flag;
        *success = !insertion_object->allocated;

        //Return an actual object only if the object is not allocated:
        if (*success) {

            //Copy data from the object to the insertion object;
            insertion_object->object = *object;

            //Save the copied object, and eventually reset *success in case of error.
            insert(success);

        }

    }


    /*
     * process_t : this method updates the input index. The input case must have been filled before.
     *
     */

    void insert(bool *success) {

        //First, cache the address of the queue object at the reading index.
        queue_object_t *insertion_object = content + insertion_index;

        //Update the success flag;
        *success = !insertion_object->allocated;


        //Only modify the state variables if the object is not allocated
        if (*success) {


            //update the reading index
            SAFE_DECR(insertion_index);

            //Modify the state variables
            nb_objects++;
            nb_spaces--;

            insertion_object->allocated = true;

        }

    }

    /*
     * read_previous_input_ptr : this method returns a pointer to the previous input case
     *
     */

    /*
    T *read_previous_input_ptr() {
        if (insertion_index != max_index) {
            return data + insertion_index + 1;
        } else {
            return data;
        }
    }

    */


    //-------------------------------------------------Queue state------------------------------------------------------


    uint8_t available_spaces() {
        return nb_spaces;
    }

    uint8_t available_objects() {
        return nb_objects;
    }


    const uint8_t get_reading_index(bool *success) {

        //Update the success flag
        *success = content[reading_index].allocated;

        //Return a valid index only is the element is allocated.
        if (*success) {
            return reading_index;
        }

        //If the element is not allocated, return 0
        return 0;

    }

    const uint8_t get_insertion_index(bool *success) {

        //Update the success flag
        *success = !content[insertion_index].allocated;

        //Return a valid index only is the element is not allocated.
        if (*success) {
            return insertion_index;
        }

        //If the element is allocated, return 0
        return 0;
    }



    //-------------------------------------------------- Clear ---------------------------------------------------------

    void clear() {

        //Reset indices;
        reading_index = insertion_index = 0;

        //Reset the number of objects contained in the queue to zero;
        nb_objects = 0;

        //Reset the number of available spaces to the maximum number;
        nb_spaces = size;

        //Deallocate all objects :

        //For each element :
        for (uint8_t i = 0; i < size; i++) {

            //Deallocate the element;
            content[i].allocated = false;

        }


    }

#undef SAFE_DECR

};


#endif //TRACER_QUEUE_H
