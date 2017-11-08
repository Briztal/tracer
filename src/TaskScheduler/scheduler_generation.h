/*
  scheduler_generation.h - Part of TRACER

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

/*
 * This file contains macro used in generation of scheduler functions / structure for a precise function.
 *
 * It eases your development the following way :
 *
 *  Say that we have a function like below, that you may want to schedule  :
 *
 *  task_state_t function(uint8_t var_0, float var_1, long var_2) {
 *      //SOME OPERATIONS
 *  }
 *
 *  Normally, to schedule a this function, you would need :
 *      - A structure to contain data (var_0, 1 and 2), from the moment you schedule the function
 *          to the moment it is effectively executed;
 *      - A scheduler, taking the same arguments than your function, that would put those into
 *          an instance of the struct, and schedule the unpacker (below);
 *      - An unpacker, that would unpack arguments, and execute the desired function, with extracted args.
 *
 *  As this can be a very heavy and repetitive process, macros present in this file do the implementation job for you.
 *
 *  The only thing you have to do is to call the macro GENERATE_SCHEDULERn with the arguments like below :
 *
 *      GENERATE_SCHEDULER(function_name, nb_args, type_0, variable_0, ..., type_[nb_args], variable_[nb_args])
 *
 *  To keep up with our example, to generate the scheduling structure for "function", you would write :
 *
 *      GENERATE_SCHEDULER(function, 3, uint8_t, var_0, float, var_1, long, var_2)
 *
 *
 */

//----------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------WARNING--------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------MACRO_CODE_BELOW---------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//------------------------------------------------MODIFY_AT_YOUR_OWN_RISKS----------------------------------------------
//----------------------------------------------------------------------------------------------------------------------



/*
 * Four text processors are required to generate properly the structure :

 signature : this will be the signature of the scheduler (the unpacker takes a void *,  as it extracts data.

    we need to convert (type_0, var_0, ..., type_n, var_n) to :

        (type_0 var_0, ... , type_n var_n)  //<-- no comma at the end



 struct declaration : this text will declare the content of the structure, according to provided types and variables.

    we need to convert (type_0, var_0, ..., type_n, var_n) to :

        type_0 var_0; ... ; type_n var_n;



 struct fill : this text effectively inserts text to a container struct, using a pointer to the struct named "data".

    we need to convert (type_0, var_0, ..., type_n, var_n) to :

        data->var_0 = var_0; ... ; data->var_n = var_n;



 struct extraction : this text provides arguments to the scheduled function, extracting it of the struct

    we need to convert (type_0, var_0, ..., type_n, var_n) to :

        (data->var_0, ... , data->var_n)  //<-- no comma at the end


 */


/*
 *
 * The real difficulty in this generation, is that it must adapt to "any" number of arguments.
 *
 * As macro cannot be recursive, we won't be able to adapt to any number, but we can simulate this adaptation :
 *
 *  We will define two macros REC_N and RECP_N, for each number of argument we will support
 *      (N will be in [|0; MAX_NB_ARGS|])
 *
 *  These macros are a simulation of a N degree recursion of a provided macro MACRO.
 *      As macros will process data based on couples (type, variable), it accepts another macro, CAT,
 *      that will be used to extract data from couples (type, variable).
 *
 *      //TODO COMMENT REC AND RECP
 *
 */

#define REC_0(MACRO, CAT, ...)
#define REC_1(MACRO, CAT, t0, v0) MACRO(CAT(t0, v0) , )
#define REC_2(MACRO, CAT, t0, v0, t1, v1) MACRO(CAT(t0, v0) , MACRO(CAT(t1, v1), ))
#define REC_3(MACRO, CAT, t0, v0, t1, v1, t2, v2) MACRO(CAT(t0, v0) , MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), )))
#define REC_4(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3) MACRO(CAT(t0, v0) , MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), MACRO(CAT(t3, v3), ))))
#define REC_5(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4) MACRO(CAT(t0, v0) , MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), MACRO(CAT(t3, v3), MACRO(CAT(t4, v4), )))))
#define REC_6(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5) MACRO(CAT(t0, v0) , MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), MACRO(CAT(t3, v3), MACRO(CAT(t4, v4), MACRO(CAT(t5, v5), ))))))
#define REC_7(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6) MACRO(CAT(t0, v0) , MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), MACRO(CAT(t3, v3), MACRO(CAT(t4, v4), MACRO(CAT(t5, v5), MACRO(CAT(t6, v6), )))))))
#define REC_8(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6, t7, v7) MACRO(CAT(t0, v0) , MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), MACRO(CAT(t3, v3), MACRO(CAT(t4, v4), MACRO(CAT(t5, v5), MACRO(CAT(t6, v6), MACRO(CAT(t7, v7), ))))))))
#define REC_9(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6, t7, v7, t8, v8) MACRO(CAT(t0, v0) , MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), MACRO(CAT(t3, v3), MACRO(CAT(t4, v4), MACRO(CAT(t5, v5), MACRO(CAT(t6, v6), MACRO(CAT(t7, v7), MACRO(CAT(t8, v8), )))))))))
#define REC_10(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6, t7, v7, t8, v8, t9, v9) MACRO(CAT(t0, v0) , MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), MACRO(CAT(t3, v3), MACRO(CAT(t4, v4), MACRO(CAT(t5, v5), MACRO(CAT(t6, v6), MACRO(CAT(t7, v7), MACRO(CAT(t8, v8), MACRO(CAT(t9, v9), ))))))))))

#define RECP_0(MACRO, CAT, ...)
#define RECP_1(MACRO, CAT, t0, v0) CAT(t0, v0)
#define RECP_2(MACRO, CAT, t0, v0, t1, v1) MACRO(CAT(t0, v0), CAT(t1, v1))
#define RECP_3(MACRO, CAT, t0, v0, t1, v1, t2, v2) MACRO(CAT(t0, v0), MACRO(CAT(t1, v1), CAT(t2, v2)))
#define RECP_4(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3) MACRO(CAT(t0, v0), MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), CAT(t3, v3))))
#define RECP_5(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4) MACRO(CAT(t0, v0), MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), MACRO(CAT(t3, v3), CAT(t4, v4)))))
#define RECP_6(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5) MACRO(CAT(t0, v0), MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), MACRO(CAT(t3, v3), MACRO(CAT(t4, v4), CAT(t5, v5))))))
#define RECP_7(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6) MACRO(CAT(t0, v0), MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), MACRO(CAT(t3, v3), MACRO(CAT(t4, v4), MACRO(CAT(t5, v5), CAT(t6, v6)))))))
#define RECP_8(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6, t7, v7) MACRO(CAT(t0, v0), MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), MACRO(CAT(t3, v3), MACRO(CAT(t4, v4), MACRO(CAT(t5, v5), MACRO(CAT(t6, v6), CAT(t7, v7))))))))
#define RECP_9(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6, t7, v7, t8, v8) MACRO(CAT(t0, v0), MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), MACRO(CAT(t3, v3), MACRO(CAT(t4, v4), MACRO(CAT(t5, v5), MACRO(CAT(t6, v6), MACRO(CAT(t7, v7), CAT(t8, v8)))))))))
#define RECP_10(MACRO, CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6, t7, v7, t8, v8, t9, v9) MACRO(CAT(t0, v0), MACRO(CAT(t1, v1), MACRO(CAT(t2, v2), MACRO(CAT(t3, v3), MACRO(CAT(t4, v4), MACRO(CAT(t5, v5), MACRO(CAT(t6, v6), MACRO(CAT(t7, v7), MACRO(CAT(t8, v8), CAT(t9, v9))))))))))



/*
 * Arguments counters macros
 *
 */

#define NB_ARGS(...) NB_ARGS_(__VA_ARGS__,FIRST_PART())

//TODO VIRER ????

#define NB_ARGS_(...) SELECT_NUMBER(__VA_ARGS__)

#define SELECT_NUMBER(_1, _2, _3, _4, _5, _6, _7, _8, _9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,N,...) N

#define FIRST_PART() 10, 10, 9, 9, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0


/*
 * Concatenation macros
 *
 *  Below are defined macro that will be used as the CAT macro used in REC_ and RECP_
 *  //TODO DOC
 *
 */

#define SPACE_CAT(t, v) t v
#define STRUCT_FILL_CAT(t, v) data->v = v
#define STRUCT_EXTRACT_CAT(t, v) data->v

#define TO_COMMA(x, y) x , y
#define TO_SEMICOLON(x, y) x ; y


//Utility macros

#define GET_SIGNATURE(nb_args, ...) (RECP_ ## nb_args(TO_COMMA, SPACE_CAT, __VA_ARGS__))

#define DEFINE_STRUCT(nb_args, ...) REC_ ## nb_args(TO_SEMICOLON, SPACE_CAT, __VA_ARGS__)

#define FILL_STRUCT(nb_args, ...) REC_ ## nb_args(TO_SEMICOLON, STRUCT_FILL_CAT, __VA_ARGS__)

#define EXTRACT_STRUCT(nb_args, ...) (RECP_ ## nb_args(TO_COMMA, STRUCT_EXTRACT_CAT, __VA_ARGS__))


//Implementations macros

#define GENERATE_STRUCT(name, struct_data) typedef struct name##_struct_t { struct_data } name##struct_t ;

#define GENERATE_PACKER(name, signature, struct_fill)\
static task_state_t name##_scheduled signature { \
    \
    /*Create the data in the heap*/\
    name##_struct_t *data = new name##_struct_t();\
    \
    /*Fill the data*/\
    struct_fill\
    \
    /*Schedule the task*/\
    TaskScheduler::schedule_task(_##name, (void *) data, 0);\
    \
    /*Complete*/\
    return complete;\
    \
}\

#define GENERATE_UNPACKER(name, struct_extraction)\
static task_state_t _##name(void *d) {\
    \
    /*Cast the data*/\
    name##_struct_t *data = (name##_struct_t *) d;\
    \
    /*Extract the data and execute the function*/\
    task_state_t state = name struct_extraction;\
    \
    /*free the data if the task musn't be reprogrammed*/\
    if (state != reprogram) {\
        free(data);\
    }\
    \
    /*Complete with the state of the function.*/\
    return state;\
}


//Implementation macro

#define _GENERATE_SCHEDULER(name, signature, struct_data, struct_fill, struct_extraction)\
    private:\
    GENERATE_STRUCT(name, struct_data);\
    GENERATE_UNPACKER(name, struct_extraction);\
    public:\
    GENERATE_PACKER(name, signature, struct_fill);


//Scheduler generation macro

#define GENERATE_SCHEDULER(name, nb_args, ...)\
    _GENERATE_SCHEDULER(name, GET_SIGNATURE(nb_args, __VA_ARGS__), DEFINE_STRUCT(nb_args, __VA_ARGS__), FILL_STRUCT(nb_args, __VA_ARGS__), EXTRACT_STRUCT(nb_args, __VA_ARGS__))

