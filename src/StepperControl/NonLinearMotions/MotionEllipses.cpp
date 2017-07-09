/*
  MotionEllipses.cpp - Part of TRACER

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


/*
#include "MotionEllipses.h"
#include "NonLinearMotionN.h"
#include "../MotionScheduler.h"


#ifdef ANGLE_FRACTION_1

float st[91] =
        {0.0000000, 0.0174524, 0.0348995, 0.0523360, 0.0697565, 0.0871557, 0.1045285, 0.1218693, 0.1391731, 0.1564345,
         0.1736482,
         0.1908090, 0.2079117, 0.2249510, 0.2419219, 0.2588191, 0.2756374, 0.2923717, 0.3090170, 0.3255681, 0.3420201,
         0.3583680,
         0.3746066, 0.3907311, 0.4067367, 0.4226183, 0.4383711, 0.4539905, 0.4694715, 0.4848096, 0.5000000, 0.5150381,
         0.5299193,
         0.5446391, 0.5591928, 0.5735764, 0.5877852, 0.6018150, 0.6156615, 0.6293204, 0.6427876, 0.6560590, 0.6691307,
         0.6819983,
         0.6946584, 0.7071068, 0.7193398, 0.7313537, 0.7431448, 0.7547096, 0.7660444, 0.7771459, 0.7880107, 0.7986355,
         0.8090169,
         0.8191520, 0.8290375, 0.8386706, 0.8480481, 0.8571673, 0.8660254, 0.8746197, 0.8829476, 0.8910065, 0.8987940,
         0.9063077,
         0.9135455, 0.9205048, 0.9271838, 0.9335804, 0.9396926, 0.9455185, 0.9510565, 0.9563047, 0.9612616, 0.9659257,
         0.9702957,
         0.9743700, 0.9781476, 0.9816271, 0.9848077, 0.9876884, 0.9902680, 0.9925461, 0.9945219, 0.9961947, 0.9975641,
         0.9986295,
         0.9993909, 0.9998476, 1.0000000};


#define T_90 90
#define T_180 180
#define T_270 270
#define T_360 360

#define RESOLUTION 1.0


#endif

#ifdef ANGLE_FRACTION_2


float st[181] =
        {0.0000000, 0.0087265, 0.0174524, 0.0261769, 0.0348995, 0.0436194, 0.0523360, 0.0610485, 0.0697565, 0.0784591,
         0.0871557, 0.0958458, 0.1045285, 0.1132032, 0.1218693, 0.1305262, 0.1391731, 0.1478094, 0.1564345, 0.1650476,
         0.1736482, 0.1822355, 0.1908090, 0.1993679, 0.2079117, 0.2164396, 0.2249511, 0.2334454, 0.2419219, 0.2503800,
         0.2588190, 0.2672384, 0.2756374, 0.2840153, 0.2923717, 0.3007058, 0.3090170, 0.3173047, 0.3255681, 0.3338069,
         0.3420202, 0.3502074, 0.3583679, 0.3665012, 0.3746066, 0.3826834, 0.3907311, 0.3987491, 0.4067366, 0.4146932,
         0.4226183, 0.4305111, 0.4383712, 0.4461978, 0.4539905, 0.4617486, 0.4694716, 0.4771588, 0.4848096, 0.4924235,
         0.5000000, 0.5075384, 0.5150381, 0.5224986, 0.5299193, 0.5372996, 0.5446391, 0.5519370, 0.5591929, 0.5664062,
         0.5735765, 0.5807030, 0.5877852, 0.5948228, 0.6018150, 0.6087614, 0.6156614, 0.6225146, 0.6293204, 0.6360782,
         0.6427876, 0.6494480, 0.6560590, 0.6626200, 0.6691306, 0.6755902, 0.6819983, 0.6883546, 0.6946584, 0.7009093,
         0.7071068, 0.7132505, 0.7193398, 0.7253743, 0.7313537, 0.7372774, 0.7431448, 0.7489557, 0.7547096, 0.7604060,
         0.7660444, 0.7716246, 0.7771460, 0.7826082, 0.7880108, 0.7933534, 0.7986355, 0.8038568, 0.8090170, 0.8141155,
         0.8191520, 0.8241262, 0.8290376, 0.8338858, 0.8386706, 0.8433915, 0.8480481, 0.8526402, 0.8571672, 0.8616292,
         0.8660254, 0.8703557, 0.8746197, 0.8788171, 0.8829476, 0.8870108, 0.8910065, 0.8949344, 0.8987940, 0.9025853,
         0.9063078, 0.9099612, 0.9135454, 0.9170601, 0.9205049, 0.9238796, 0.9271839, 0.9304175, 0.9335804, 0.9366722,
         0.9396927, 0.9426415, 0.9455185, 0.9483237, 0.9510565, 0.9537169, 0.9563047, 0.9588197, 0.9612617, 0.9636304,
         0.9659259, 0.9681476, 0.9702957, 0.9723700, 0.9743701, 0.9762960, 0.9781476, 0.9799247, 0.9816272, 0.9832549,
         0.9848077, 0.9862856, 0.9876884, 0.9890159, 0.9902680, 0.9914448, 0.9925461, 0.9935718, 0.9945219, 0.9953962,
         0.9961947, 0.9969174, 0.9975641, 0.9981348, 0.9986295, 0.9990482, 0.9993908, 0.9996573, 0.9998477, 0.9999619,
         1.0000000};


#define T_90 180
#define T_180 360
#define T_270 540
#define T_360 720
#define RESOLUTION 2.0

#endif


#ifdef ANGLE_FRACTION_4
float st[361] =
        {0.0000000, 0.0043633, 0.0087265, 0.0130896, 0.0174524, 0.0218149, 0.0261769, 0.0305385, 0.0348995, 0.0392598,
         0.0436194, 0.0479781, 0.0523360, 0.0566928, 0.0610485, 0.0654031, 0.0697565, 0.0741085, 0.0784591, 0.0828082,
         0.0871557, 0.0915016, 0.0958458, 0.1001881, 0.1045285, 0.1088669, 0.1132032, 0.1175374, 0.1218693, 0.1261990,
         0.1305262, 0.1348509, 0.1391731, 0.1434926, 0.1478094, 0.1521234, 0.1564345, 0.1607426, 0.1650476, 0.1693495,
         0.1736482, 0.1779435, 0.1822355, 0.1865240, 0.1908090, 0.1950903, 0.1993679, 0.2036418, 0.2079117, 0.2121777,
         0.2164396, 0.2206974, 0.2249511, 0.2292004, 0.2334454, 0.2376859, 0.2419219, 0.2461533, 0.2503800, 0.2546020,
         0.2588190, 0.2630312, 0.2672384, 0.2714405, 0.2756374, 0.2798290, 0.2840153, 0.2881963, 0.2923717, 0.2965415,
         0.3007058, 0.3048643, 0.3090170, 0.3131638, 0.3173047, 0.3214395, 0.3255681, 0.3296906, 0.3338069, 0.3379167,
         0.3420202, 0.3461170, 0.3502074, 0.3542910, 0.3583679, 0.3624380, 0.3665012, 0.3705574, 0.3746066, 0.3786486,
         0.3826834, 0.3867110, 0.3907311, 0.3947439, 0.3987491, 0.4027467, 0.4067366, 0.4107189, 0.4146932, 0.4186597,
         0.4226183, 0.4265687, 0.4305111, 0.4344453, 0.4383712, 0.4422887, 0.4461978, 0.4500985, 0.4539905, 0.4578739,
         0.4617486, 0.4656145, 0.4694716, 0.4733196, 0.4771588, 0.4809888, 0.4848096, 0.4886213, 0.4924235, 0.4962165,
         0.5000000, 0.5037739, 0.5075384, 0.5112931, 0.5150381, 0.5187733, 0.5224986, 0.5262139, 0.5299193, 0.5336145,
         0.5372996, 0.5409744, 0.5446391, 0.5482932, 0.5519370, 0.5555702, 0.5591929, 0.5628049, 0.5664062, 0.5699968,
         0.5735765, 0.5771452, 0.5807030, 0.5842497, 0.5877852, 0.5913096, 0.5948228, 0.5983247, 0.6018150, 0.6052940,
         0.6087614, 0.6122172, 0.6156614, 0.6190940, 0.6225146, 0.6259235, 0.6293204, 0.6327053, 0.6360782, 0.6394390,
         0.6427876, 0.6461240, 0.6494480, 0.6527597, 0.6560590, 0.6593458, 0.6626200, 0.6658817, 0.6691306, 0.6723668,
         0.6755902, 0.6788007, 0.6819983, 0.6851830, 0.6883546, 0.6915131, 0.6946584, 0.6977904, 0.7009093, 0.7040147,
         0.7071068, 0.7101854, 0.7132505, 0.7163019, 0.7193398, 0.7223639, 0.7253743, 0.7283710, 0.7313537, 0.7343225,
         0.7372774, 0.7402181, 0.7431448, 0.7460574, 0.7489557, 0.7518398, 0.7547096, 0.7575650, 0.7604060, 0.7632325,
         0.7660444, 0.7688419, 0.7716246, 0.7743927, 0.7771460, 0.7798845, 0.7826082, 0.7853169, 0.7880108, 0.7906896,
         0.7933534, 0.7960020, 0.7986355, 0.8012537, 0.8038568, 0.8064446, 0.8090170, 0.8115740, 0.8141155, 0.8166416,
         0.8191520, 0.8216469, 0.8241262, 0.8265898, 0.8290376, 0.8314696, 0.8338858, 0.8362861, 0.8386706, 0.8410391,
         0.8433915, 0.8457278, 0.8480481, 0.8503522, 0.8526402, 0.8549119, 0.8571672, 0.8594065, 0.8616292, 0.8638355,
         0.8660254, 0.8681988, 0.8703557, 0.8724960, 0.8746197, 0.8767267, 0.8788171, 0.8808907, 0.8829476, 0.8849877,
         0.8870108, 0.8890171, 0.8910065, 0.8929789, 0.8949344, 0.8968727, 0.8987940, 0.9006982, 0.9025853, 0.9044551,
         0.9063078, 0.9081431, 0.9099612, 0.9117621, 0.9135454, 0.9153115, 0.9170601, 0.9187912, 0.9205049, 0.9222010,
         0.9238796, 0.9255405, 0.9271839, 0.9288096, 0.9304175, 0.9320078, 0.9335804, 0.9351352, 0.9366722, 0.9381914,
         0.9396927, 0.9411759, 0.9426415, 0.9440891, 0.9455185, 0.9469301, 0.9483237, 0.9496991, 0.9510565, 0.9523958,
         0.9537169, 0.9550200, 0.9563047, 0.9575714, 0.9588197, 0.9600498, 0.9612617, 0.9624552, 0.9636304, 0.9647872,
         0.9659259, 0.9670459, 0.9681476, 0.9692309, 0.9702957, 0.9713420, 0.9723700, 0.9733793, 0.9743701, 0.9753423,
         0.9762960, 0.9772310, 0.9781476, 0.9790455, 0.9799247, 0.9807853, 0.9816272, 0.9824504, 0.9832549, 0.9840407,
         0.9848077, 0.9855561, 0.9862856, 0.9869964, 0.9876884, 0.9883615, 0.9890159, 0.9896513, 0.9902680, 0.9908659,
         0.9914448, 0.9920050, 0.9925461, 0.9930685, 0.9935718, 0.9940563, 0.9945219, 0.9949685, 0.9953962, 0.9958049,
         0.9961947, 0.9965655, 0.9969174, 0.9972501, 0.9975641, 0.9978589, 0.9981348, 0.9983916, 0.9986295, 0.9988484,
         0.9990482, 0.9992290, 0.9993908, 0.9995336, 0.9996573, 0.9997621, 0.9998477, 0.9999143, 0.9999619, 0.9999905,
         1.0000000};

#define T_90 360
#define T_180 720
#define T_270 1080
#define T_360 1440
#define RESOLUTION 4.

#endif


#ifdef ANGLE_FRACTION_8
//NOT IMPLEMENTED YET
#endif


float *const MotionEllipses::sinTable = st;


void MotionEllipses::move() {
    NonLinearMotionN::move();
}

bool MotionEllipses::prepare_motion() {

    motion_size = 2;

    NonLinearMotionN::initialise_motion();

    if (!v_parameters_t[Cid] || !v_parameters_t[Aid] || !v_parameters_t[Bid])
        return false;
    if (!v_parameters_t[Uid])
        parameters_t[Uid] = 0;
    if (!v_parameters_t[Rid])
        parameters_t[Rid] = 0;

    rotation_angle_r = parameters_t[Rid];

    begin_angle_d = (int) (parameters_t[Uid] * 180.0 / M_PI * RESOLUTION);
    count_angle_d = (int) (parameters_t[Cid] * 180.0 / M_PI * RESOLUTION);

    end_angle_r = (float) (begin_angle_d + count_angle_d) * M_PI / (180.0 * RESOLUTION);

    float begin_angle_r = (float) (begin_angle_d) * M_PI / (180.0 * RESOLUTION);


    this->A = parameters_t[Aid];//mm
    this->B = parameters_t[Bid];//mm

    //cosines and sines
    float cTheta = cos(rotation_angle_r);
    float sTheta = sin(rotation_angle_r);

    center_0 = destinations[0];
    center_1 = destinations[1];

    //Unrotated first positions
    float temp0 = cos(begin_angle_r) * A, temp1 = sin(begin_angle_r) * B;//mm

    float begin0mm = cTheta * temp0 + sTheta * temp1, begin1mm = -sTheta * temp0 + cTheta * temp1;

    first_positions[0] = destinations[0] + (first_pos_t[0] = (long) (steps[0] * begin0mm));
    first_positions[1] = destinations[1] + (first_pos_t[1] = (long) (steps[1] * begin1mm));

    setup_engagement_move();

    return true;

}

void MotionEllipses::init_position_parameters() {

    //----------------------------------------Switching to tics system--------------------------------------------------

    //rotation_angle_r cos and sines
    float cTheta = cos(rotation_angle_r), sTheta = sin(rotation_angle_r);

    min = 0;

    //Angles and end_distances
    if (count_angle_d < 0)
        provide_increment(-1);
    else
        provide_increment(1);

    max = begin_angle_d + count_angle_d;
    min = begin_angle_d;


    float steps0 = steps[0], steps1 = steps[1];

    //SETTINGS
    //Rotation parameters
    ca = steps0 * cTheta * A;
    sa = steps1 * -sTheta * A;
    cb = steps1 * cTheta * B;
    sb = steps0 * sTheta * B;

    //Last Position
    float temp0 = cos(end_angle_r), temp1 = sin(end_angle_r);
    last_pos_t[0] = (long) (ca * temp0 + sb * temp1), last_pos_t[1] = (long) (sa * temp0 + cb * temp1);

    last_positions[0] = center_0 + last_pos_t[0];
    last_positions[1] = center_1 + last_pos_t[1];

    set_last_position();

}

void MotionEllipses::get_position(float alpha, long *pos) {
    float tmpCos, tmpSin;
    trigProcess((int) alpha, tmpCos, tmpSin);
    pos[0] = (long) (ca * tmpCos + sb * tmpSin);
    pos[1] = (long) (sa * tmpCos + cb * tmpSin);
}

float tmpCos, tmpSin;
long t0;

//TODO TRIER LES END_DISTANCES ICI AUSSI

bool MotionEllipses::process_position(pos_data *p) {
    if (p->started) {
        goto
        *(*p).step;
    }
    p->started = true;
    c1 :
    trigProcess((int) alpha, tmpCos, tmpSin);
    (*p).step = &&c2;

    return false;
    c2 :
    t0 = (long) (ca * tmpCos);
    (*p).step = &&c3;
    return false;
    c3 :
    t0 += (long) (sb * tmpSin);
    (*p).step = &&c4;
    return false;
    c4 :

    int i = (int) (t0 - *pos_t);
    bool b = (i > 0);
    elementary_dists[0] = b ? (unsigned char) (i) : (unsigned char) (-i);
    if (b ^ dirs[0]) {
        (**dir_set_functions)(b);
        *dirs = b;
    }
    *pos_t = t0;
    (*p).step = &&c5;

    return false;
    c5 :
    t0 = (long) (sa * tmpCos);
    (*p).step = &&c6;
    return false;
    c6:
    t0 += (long) (cb * tmpSin);
    (*p).step = &&c7;
    return false;
    c7 :
    i = (int) (t0 - pos_t[1]);
    bool b1 = (i > 0);

    elementary_dists[1] = b1 ? (unsigned char) (i) : (unsigned char) (-i);
    if (b1 ^ dirs[1]) {
        (*(dir_set_functions[1]))(b1);
        dirs[1] = b1;
    }
    pos_t[1] = t0;
    (*p).step = &&c7;

    return true;

}

void MotionEllipses::trigProcess(int x, float &cos, float &sin) {
    if (x >= T_360) {
        x -= T_360;
        alpha -= T_360;
    } else if (x < 0) {
        x += T_360;
        alpha += T_360;
    }

    if (x <= T_90) {
        sin = sinTable[x];
        cos = sinTable[T_90 - x];
    } else if (x <= T_180) {
        sin = sinTable[T_180 - x];
        cos = -sinTable[x - T_90];
    } else if (x <= T_270) {
        sin = -sinTable[x - T_180];
        cos = -sinTable[T_270 - x];
    } else {
        sin = -sinTable[T_360 - x];
        cos = sinTable[x - T_270];
    }

}


 */







