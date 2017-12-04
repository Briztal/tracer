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
 *      - An unpacker, that would unpack arguments, and analyse_command the desired function, with extracted dynamic_args.
 *
 *  As this can be a very heavy and repetitive process, macros present in this file do the implementation job for you.
 *
 *  The only thing you have to do is to call the macro GENERATE_SCHEDULERn with the arguments like below :
 *
 *      GENERATE_SCHEDULER(function_name, task_type, type_0, variable_0, ..., type_[nb_args], variable_[nb_args])
 *
 *  To keep up with our example, to generate the scheduling structure for "function" with the type 1, you would write :
 *
 *      GENERATE_SCHEDULER(function, 1, uint8_t, var_0, float, var_1, long, var_2)
 *
 *
 * the resulting scheduler will be a function, taking exactly the same argument than your primary function,
 *
 *  and named :
 *
 *  task_state_t function_scheduled_i(uint8_t var_0, float var_1, long var_2) {
 *      //SOME SCHEDULED OPERATIONS
 *  }
 *
 *          where i is the task type you required previously.
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
 *  We will define two macros SEMICOLON_N and COMMA_N, for each number of argument we will support
 *      (N will be in [|0; MAX_NB_ARGS|])
 *
 *  These macros are a simulation of a N degree recursion of a provided macro MACRO.
 *      As macros will process data based on couples (type, variable), it accepts another macro, CAT,
 *      that will be used to extract data from couples (type, variable).
 *
 */


/*
 * Macros SEMICOLON_F and COMMA_F, defined below are meant to write the name  the function corresponding 
 *  to the number of arguments provided;
 *
 *  They will write one of the SEMICOLON_[i] or COMMA_[i] functions.
 *
 */

#define SEMICOLON_F(...) REC_FUNCTION(__VA_ARGS__, SEMICOLON_FIRST_PART())
#define COMMA_F(...) REC_FUNCTION(__VA_ARGS__, COMMA_FIRST_PART())

#define REC_FUNCTION(...) SELECT_NUMBER(__VA_ARGS__)

#define SELECT_NUMBER(_1, _2, _3, _4, _5, _6, _7, _8, _9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,N,...) N

#define COMMA_FIRST_PART() COMMA_10, COMMA_10, COMMA_9, COMMA_9, COMMA_8, COMMA_8, COMMA_7, COMMA_7, COMMA_6, COMMA_6, COMMA_5, COMMA_5, COMMA_4, COMMA_4, COMMA_3, COMMA_3, COMMA_2, COMMA_2, COMMA_1, COMMA_1, COMMA_0, COMMA_0
#define SEMICOLON_FIRST_PART() SEMICOLON_10, SEMICOLON_10, SEMICOLON_9, SEMICOLON_9, SEMICOLON_8, SEMICOLON_8, SEMICOLON_7, SEMICOLON_7, SEMICOLON_6, SEMICOLON_6, SEMICOLON_5, SEMICOLON_5, SEMICOLON_4, SEMICOLON_4, SEMICOLON_3, SEMICOLON_3, SEMICOLON_2, SEMICOLON_2, SEMICOLON_1, SEMICOLON_1, SEMICOLON_0, SEMICOLON_0


/*
 * Function_like macros defined below do the following :
 *  - SEMICOLON_I : concatenate the i couples of (type_i, value_i) to CAT(type_0, value_0); ...; CAT(type_I, value_I);
 *  - COMMA_I : concatenate the i couples of (type_i, value_i) to CAT(type_0, value_0), ..., CAT(type_I, value_I)
 *
 * CAT is a macro provided in arguments, meant to process (type_n, variable_n) to a particular text.
 *
 */

#define SEMICOLON_0(CAT, ...)
#define SEMICOLON_1(CAT, t0, v0) TO_S(CAT(t0, v0) , )
#define SEMICOLON_2(CAT, t0, v0, t1, v1) TO_S(CAT(t0, v0) , TO_S(CAT(t1, v1), ))
#define SEMICOLON_3(CAT, t0, v0, t1, v1, t2, v2) TO_S(CAT(t0, v0) , TO_S(CAT(t1, v1), TO_S(CAT(t2, v2), )))
#define SEMICOLON_4(CAT, t0, v0, t1, v1, t2, v2, t3, v3) TO_S(CAT(t0, v0) , TO_S(CAT(t1, v1), TO_S(CAT(t2, v2), TO_S(CAT(t3, v3), ))))
#define SEMICOLON_5(CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4) TO_S(CAT(t0, v0) , TO_S(CAT(t1, v1), TO_S(CAT(t2, v2), TO_S(CAT(t3, v3), TO_S(CAT(t4, v4), )))))
#define SEMICOLON_6(CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5) TO_S(CAT(t0, v0) , TO_S(CAT(t1, v1), TO_S(CAT(t2, v2), TO_S(CAT(t3, v3), TO_S(CAT(t4, v4), TO_S(CAT(t5, v5), ))))))
#define SEMICOLON_7(CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6) TO_S(CAT(t0, v0) , TO_S(CAT(t1, v1), TO_S(CAT(t2, v2), TO_S(CAT(t3, v3), TO_S(CAT(t4, v4), TO_S(CAT(t5, v5), TO_S(CAT(t6, v6), )))))))
#define SEMICOLON_8(CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6, t7, v7) TO_S(CAT(t0, v0) , TO_S(CAT(t1, v1), TO_S(CAT(t2, v2), TO_S(CAT(t3, v3), TO_S(CAT(t4, v4), TO_S(CAT(t5, v5), TO_S(CAT(t6, v6), TO_S(CAT(t7, v7), ))))))))
#define SEMICOLON_9(CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6, t7, v7, t8, v8) TO_S(CAT(t0, v0) , TO_S(CAT(t1, v1), TO_S(CAT(t2, v2), TO_S(CAT(t3, v3), TO_S(CAT(t4, v4), TO_S(CAT(t5, v5), TO_S(CAT(t6, v6), TO_S(CAT(t7, v7), TO_S(CAT(t8, v8), )))))))))
#define SEMICOLON_10(CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6, t7, v7, t8, v8, t9, v9) TO_S(CAT(t0, v0) , TO_S(CAT(t1, v1), TO_S(CAT(t2, v2), TO_S(CAT(t3, v3), TO_S(CAT(t4, v4), TO_S(CAT(t5, v5), TO_S(CAT(t6, v6), TO_S(CAT(t7, v7), TO_S(CAT(t8, v8), TO_S(CAT(t9, v9), ))))))))))


#define COMMA_0(CAT, ...)
#define COMMA_1(CAT, t0, v0) CAT(t0, v0)
#define COMMA_2(CAT, t0, v0, t1, v1) TO_C(CAT(t0, v0), CAT(t1, v1))
#define COMMA_3(CAT, t0, v0, t1, v1, t2, v2) TO_C(CAT(t0, v0), TO_C(CAT(t1, v1), CAT(t2, v2)))
#define COMMA_4(CAT, t0, v0, t1, v1, t2, v2, t3, v3) TO_C(CAT(t0, v0), TO_C(CAT(t1, v1), TO_C(CAT(t2, v2), CAT(t3, v3))))
#define COMMA_5(CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4) TO_C(CAT(t0, v0), TO_C(CAT(t1, v1), TO_C(CAT(t2, v2), TO_C(CAT(t3, v3), CAT(t4, v4)))))
#define COMMA_6(CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5) TO_C(CAT(t0, v0), TO_C(CAT(t1, v1), TO_C(CAT(t2, v2), TO_C(CAT(t3, v3), TO_C(CAT(t4, v4), CAT(t5, v5))))))
#define COMMA_7(CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6) TO_C(CAT(t0, v0), TO_C(CAT(t1, v1), TO_C(CAT(t2, v2), TO_C(CAT(t3, v3), TO_C(CAT(t4, v4), TO_C(CAT(t5, v5), CAT(t6, v6)))))))
#define COMMA_8(CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6, t7, v7) TO_C(CAT(t0, v0), TO_C(CAT(t1, v1), TO_C(CAT(t2, v2), TO_C(CAT(t3, v3), TO_C(CAT(t4, v4), TO_C(CAT(t5, v5), TO_C(CAT(t6, v6), CAT(t7, v7))))))))
#define COMMA_9(CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6, t7, v7, t8, v8) TO_C(CAT(t0, v0), TO_C(CAT(t1, v1), TO_C(CAT(t2, v2), TO_C(CAT(t3, v3), TO_C(CAT(t4, v4), TO_C(CAT(t5, v5), TO_C(CAT(t6, v6), TO_C(CAT(t7, v7), CAT(t8, v8)))))))))
#define COMMA_10(CAT, t0, v0, t1, v1, t2, v2, t3, v3, t4, v4, t5, v5, t6, v6, t7, v7, t8, v8, t9, v9) TO_C(CAT(t0, v0), TO_C(CAT(t1, v1), TO_C(CAT(t2, v2), TO_C(CAT(t3, v3), TO_C(CAT(t4, v4), TO_C(CAT(t5, v5), TO_C(CAT(t6, v6), TO_C(CAT(t7, v7), TO_C(CAT(t8, v8), CAT(t9, v9))))))))))


/*
 * Concatenation macros
 *
 *  Below are defined macro that will be used as the CAT macro used in COMMA_I and SEMICOLON_I
 *
 */

//SPACE_CAT concatenates t, a space and v.
#define SPACE_CAT(t, v) t v

//STRUCT_FILL_CAT uses v for a struct assignment.
#define STRUCT_FILL_CAT(t, v) _unpacker_data_->v = v

//STRUCT_EXTRACT_CAT uses v for a struct extraction.
#define STRUCT_EXTRACT_CAT(t, v) _unpacker_data_->v



/*
 * Comma and semicolon concatenation :
 */
#define TO_C(x, y) x , y
#define TO_S(x, y) x ; y


/*
 * Implementations macros
 *
 * The macros defined below will :
 *
 *  - generate the structure
 *
 *  - generate the packer (will put data to the struct)
 *
 *  - generate the unpacker (will extract data from the struct and call the function with it)
 *
 */

#include <interface.h>
#define GENERATE_STRUCT(name, struct_data) struct name##_struct_t { struct_data };

#define GENERATE_PACKER(name, type,  signature, struct_fill)\
static task_state_t name##_scheduled_##type signature { \
    \
    /*Create the data in the heap*/\
    struct name##_struct_t *_unpacker_data_ = new name##_struct_t();\
    CI::echo("ADDRESS : "+String((uint32_t)_unpacker_data_));\
    \
    /*Fill the data*/\
    struct_fill\
    \
    /*Schedule a task, that will schedule_command the required function, passing the previously created data.*/\
    TaskScheduler::schedule_task(type, _##name, (void *) _unpacker_data_);\
    \
    /*Complete*/\
    return complete;\
    \
}\


#define GENERATE_UNPACKER(name, struct_extraction)\
static task_state_t _##name(void *_unpacker_pointer_) {\
    /*Cast the data*/\
    struct name##_struct_t *_unpacker_data_ = (name##_struct_t *) _unpacker_pointer_;\
    \
    /*Extract the data and execute the function*/\
    task_state_t _unpacker_return_state_ = name struct_extraction;\
    \
    /*Complete with the state of the function.*/\
    return _unpacker_return_state_;\
}



/*
 * Generator macro :
 *
 *  This macro used the three macros defines upper.
 *
 */
#define GENERATE_SCHEDULER_(name, type, signature, struct_data, struct_fill, struct_extraction)\
    private:\
    GENERATE_STRUCT(name, struct_data);\
    GENERATE_UNPACKER(name, struct_extraction);\
    public:\
    GENERATE_PACKER(name, type, signature, struct_fill);


/*
 * String processing macros :
 *
 *  Macros defined below generate the code required to
 *      - get the signature;
 *      - define the structure;
 *      - fill the structure;
 *      - read data from the structure;
 *
 *  from arguments.
 *
 */

#define GET_SIGNATURE_(...) (COMMA_F(__VA_ARGS__) (SPACE_CAT, __VA_ARGS__))

#define DEFINE_STRUCT_(...)  SEMICOLON_F(__VA_ARGS__) (SPACE_CAT, __VA_ARGS__)

#define FILL_STRUCT_(...)  SEMICOLON_F(__VA_ARGS__) (STRUCT_FILL_CAT, __VA_ARGS__)

#define EXTRACT_STRUCT_(...) (COMMA_F(__VA_ARGS__) (STRUCT_EXTRACT_CAT, __VA_ARGS__))


/*
 * Scheduler generation macro :
 *
 * This macro is the one that is used by the programmer, to schedule a function, taking some dynamic_args.
 *
 *  It calls the pre-generation macro, passing the code generated using the four macros upper.
 *
 */

#define GENERATE_SCHEDULER(name, type, ...)\
    GENERATE_SCHEDULER_(name, type, GET_SIGNATURE_(__VA_ARGS__), DEFINE_STRUCT_(__VA_ARGS__), FILL_STRUCT_(__VA_ARGS__), EXTRACT_STRUCT_(__VA_ARGS__))


