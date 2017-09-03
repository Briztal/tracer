//
// Created by root on 03/09/17.
//

#ifndef TRACER_SUBMOVEMENTMANAGER_H
#define TRACER_SUBMOVEMENTMANAGER_H


#include <DataStructures/Queue.h>
#include "_kernels_data.h"

class SubMovementManager {

//-----------------------------------------------Sub_movement_queue-------------------------------------------------

private:

    //The sub_movements queue
    static Queue<k2_real_time_data> sub_movement_queue;

    //The arrays to store real and integer distances
    static float *sub_movement_real_distances;
    static uint8_t *sub_movement_int_distances;

public :

    //A simple method to return the number of available_sub_movements currently in the sub_movement queue.
    static uint8_t available_sub_movements() {
        return sub_movement_queue.available_elements();
    }


    //----------------------------------------Pre_Computed_Positions_storage--------------------------------------------

public :

    //movement_processed flag, true when the current movement's last position have been processed
    static bool movement_processed;

    /*
     * position pushing functions :
     *  As the pushing procedure is costly, it is divided in two procedures. They must be called one after the other
     *      for a new position to be effectively pushed.
     */

    //function to determine and push a candidate position.
    static void push_new_position();

    //function to determine a candidate position.
    static void push_new_position_1();

    //function to push (if the distances bounds are respected) the previously computed position in the queue.
    static void push_new_position_2();

    //function to push the maximum number of positions in the queue.
    static void fill_sub_movement_queue();

    //function to pop the next position in the queue.
    static void
    pop_next_position(uint8_t *elementary_dists, float *real_dists, sig_t *negative_signature, float *distance);


};


#endif //TRACER_SUBMOVEMENTMANAGER_H
