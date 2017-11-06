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
 * It eases your developpement by //TODO TUTORIAL / DOC
 *
 */
/* Scheduler :

 struct definition :
    type + " " + var + ";" + ...


 signature :

 type + " " + var + ", "+ ... + PAS DE VIRGULE A LA FIN


 struct fill :

    "data->"+ var + " = " + var + "; "


 struct extraction :

  'data->" + var + ", "+ ... + PAS DE VIRGULE A LA FIN

 */


//Simili recursive macros

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


//Concatenation macros

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

