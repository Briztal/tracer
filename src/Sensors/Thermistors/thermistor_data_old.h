/*
  thermistor_data.h - Part of TRACER

  Copyright (c) 2017 Raphaël Outhier - reprap.org (all data in this file comes from the reprap website)

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
 * This file contains all data related to thermistors.
 *
 * I got this data from the reprap.org page dedicated to thermistors :
 *
 * http://reprap.org/wiki/Thermistor
 *
 * It comprises lookup table for each thermistor that the code can deal with, and the table size for each one.
 *
 * You may use macros provided here to populate the thermistor section of you configuration file.
 *
 * To do so, you may use the next abbreviation table
 *
 *
 *      Thermistor                                  |            Name
 *      EPCOS 100K Thermistor #1 (B57540G0104F000)  |       B57540G0104F000
 *      EPCOS 100K Thermistor #2 (B57560G1104F)     |       B57560G1104F
 *      EPCOS 100K Thermistor #3 (B57560G104F)      |       B57560G104F
 *      RRRF100K                                    |       RRRF100K
 *      RRRF10K                                     |       RRRF10K
 *      RS10K                                       |       RS10K
 *      Honeywell 100K Thermistor (135-104LAG-J01)  |       HW100K
 *      ATC Semitec  Thermistor (104GT-2)           |       ATC104GT2
 *      PT100                                       |       PT100
 *      KTY82-210 Thermistor (Philips)              |       KTY82210
 *
 *
 * You must write_data a line like the following in your config_files.h to enable a particular thermistor, named [NAME] :
 *
 * THEMRISTOR([indice], [NAME]_TABLE, [NAME]_SIZE)
 *      where indice is the thermistor indice, and [NAME] is the thermistor name
 *
 */

//----------------------------------------------------------------------------------------------------------------------


/*
 * EPCOS 100K Thermistor #1(B57540G0104F000)
 * Thermistor lookup table for RepRap Temperature Sensor boards (http://make.rrrf.org/ts)
 * Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
 * ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=4066 --ending-adc=1023
 * r0: 100000
 * t0: 25
 * r1: 0
 * r2: 4700
 * beta: 4066
 * ending adc: 1023
 */

#define B57540G0104F000_SIZE 20
#define B57540G0104F000_TABLE {\
         {1, 841},\
{54, 255},\
{107, 209},\
{160, 184},\
{213, 166},\
{266, 153},\
{319, 142},\
{372, 132},\
{425, 124},\
{478, 116},\
{531, 108},\
{584, 101},\
{637, 93},\
{690, 86},\
{743, 78},\
{796, 70},\
{849, 61},\
{902, 50},\
{955, 34},\
{1008, 3}\
}


//----------------------------------------------------------------------------------------------------------------------


/*
 * EPCOS 100K Thermistor #2 (B57560G1104F)
 * Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
 * ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=4092 --ending-adc=1023
 * r0: 100000
 * t0: 25
 * r1: 0
 * r2: 4700
 * beta: 4092
 * ending adc: 1023

 */

#define B57560G1104F_SIZE 20
#define B57560G1104F_TABLE {\
        {1, 821},\
        {54, 252},\
        {107, 207},\
        {160, 182},\
        {213, 165},\
        {266, 152},\
        {319, 141},\
        {372, 131},\
        {425, 123},\
        {478, 115},\
        {531, 107},\
        {584, 100},\
        {637, 93},\
        {690, 86},\
        {743, 78},\
        {796, 70},\
        {849, 60},\
        {902, 49},\
        {955, 34},\
        {1008, 3}\
}


//----------------------------------------------------------------------------------------------------------------------


/* EPCOS 100K Thermistor #3(B57560G104F)
 * Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
 * ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=4036 --ending-adc=1023
 * r0: 100000
 * t0: 25
 * r1: 0
 * r2: 4700
 * beta: 4036
 * ending adc: 1023
 */

#define B57560G104F_SIZE 20
#define B57560G104F_TABLE {\
        {54, 258},\
        {1, 864},\
        {107, 211},\
        {160, 185},\
        {213, 168},\
        {266, 154},\
        {319, 143},\
        {372, 133},\
        {425, 125},\
        {478, 116},\
        {531, 109},\
        {584, 101},\
        {637, 94},\
        {690, 87},\
        {743, 79},\
        {796, 70},\
        {849, 61},\
        {902, 50},\
        {955, 34},\
        {1008, 2}\
}


//----------------------------------------------------------------------------------------------------------------------


/* RRRF100K :
 * Thermistor lookup table for RepRap Temperature Sensor boards (http://make.rrrf.org/ts)
 * Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
 * ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=3960 --ending-adc=1023
 * r0: 100000
 * t0: 25
 * r1: 0
 * r2: 4700
 * beta: 3960
 * ending adc: 1023
 */

#define RRRF100K_SIZE 20
#define RRRF100K_TABLE {\
        {1, 929},\
        {54, 266},\
        {107, 217},\
        {160, 190},\
        {213, 172},\
        {266, 158},\
        {319, 146},\
        {372, 136},\
        {425, 127},\
        {478, 119},\
        {531, 111},\
        {584, 103},\
        {637, 96},\
        {690, 88},\
        {743, 80},\
        {796, 71},\
        {849, 62},\
        {902, 50},\
        {955, 34},\
        {1008, 2}\
}


//----------------------------------------------------------------------------------------------------------------------


/* RRRF10K :
 * Thermistor lookup table for RepRap Temperature Sensor boards (http://make.rrrf.org/ts)
 * Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
 * ./createTemperatureLookup.py --r0=10000 --t0=25 --r1=680 --r2=1600 --beta=3964 --ending-adc=305
 * r0: 10000
 * t0: 25
 * r1: 680
 * r2: 1600
 * beta: 3964
 * ending adc: 305
 */

#define RRRF10K_SIZE 19
#define RRRF10K_TABLE {\
        {1, 601},\
        {17, 260},\
        {33, 213},\
        {49, 187},\
        {65, 170},\
        {81, 156},\
        {97, 144},\
        {113, 134},\
        {129, 125},\
        {145, 117},\
        {161, 109},\
        {177, 101},\
        {193, 94},\
        {209, 86},\
        {225, 78},\
        {241, 69},\
        {257, 59},\
        {273, 46},\
        {289, 28}\
}


//----------------------------------------------------------------------------------------------------------------------


/* RS10K
 * Thermistor lookup table for RepRap Temperature Sensor boards (http://make.rrrf.org/ts)
 * Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
 * ./createTemperatureLookup.py --r0=10000 --t0=25 --r1=680 --r2=1600 --beta=3480 --ending-adc=315
 * r0: 10000
 * t0: 25
 * r1: 680
 * r2: 1600
 * beta: 3480
 * ending adc: 315
 */


#define RS10K_SIZE 20
#define RS10K_TABLE {\
        {1, 922},\
        {17, 327},\
        {33, 260},\
        {49, 225},\
        {65, 202},\
        {81, 184},\
        {97, 169},\
        {113, 156},\
        {129, 145},\
        {145, 134},\
        {161, 125},\
        {177, 115},\
        {193, 106},\
        {209, 96},\
        {225, 87},\
        {241, 76},\
        {257, 64},\
        {273, 50},\
        {289, 29},\
        {305, -45}\
}


//----------------------------------------------------------------------------------------------------------------------


/* Honeywell 100K Thermistor (135-104LAG-J01)
 * Made with createTemperatureLookup.py (http://svn.reprap.org/trunk/reprap/firmware/Arduino/utilities/createTemperatureLookup.py)
 * ./createTemperatureLookup.py --r0=100000 --t0=25 --r1=0 --r2=4700 --beta=3974 --ending-adc=1023
 * r0: 100000
 * t0: 25
 * r1: 0
 * r2: 4700
 * beta: 3974
 * ending adc: 1023
 */

#define HW100K_SIZE 20
#define HW100K_TABLE {\
        {1, 916},\
        {54, 265},\
        {107, 216},\
        {160, 189},\
        {213, 171},\
        {266, 157},\
        {319, 146},\
        {372, 136},\
        {425, 127},\
        {478, 118},\
        {531, 110},\
        {584, 103},\
        {637, 95},\
        {690, 88},\
        {743, 80},\
        {796, 71},\
        {849, 62},\
        {902, 50},\
        {955, 34},\
        {1008, 2}\
}


//----------------------------------------------------------------------------------------------------------------------


/* ATC Semitec Thermistor (104GT-2)
 * r0: 100000
 * t0: 25
 * r1: 0
 * r2: 4700
 * beta: 4267
 * max adc: 1023
 */

#define ATC104GT2_SIZE 20

#define ATC104GT2_TABLE {\
        {1, 713},\
        {54, 236},\
        {107, 195},\
        {160, 172},\
        {213, 157},\
        {266, 144},\
        {319, 134},\
        {372, 125},\
        {425, 117},\
        {478, 110},\
        {531, 103},\
        {584, 96},\
        {637, 89},\
        {690, 83},\
        {743, 75},\
        {796, 68},\
        {849, 59},\
        {902, 48},\
        {955, 34},\
        {1008, 3}\
}


//----------------------------------------------------------------------------------------------------------------------

/*
 * PT1000 Thermistor
 */
#define PT100_SIZE 15
#define PT100_TABLE {\
        {505, 0},\
        {547, 25},\
        {591, 50},\
        {632, 75},\
        {672, 100},\
        {711, 125},\
        {749, 150},\
        {785, 175},\
        {821, 200},\
        {856, 225},\
        {890, 250},\
        {923, 275},\
        {955, 300},\
        {987, 325},\
        {1018, 350},\
}


//----------------------------------------------------------------------------------------------------------------------

/*
 * KTY82-210 Thermistor (Philips)
 */

#define KTY82210_SIZE 15
#define KTY82210_TABLE {\
        {456, 0},\
        {491, 10},\
        {528, 20},\
        {566, 30},\
        {605, 40},\
        {645, 50},\
        {686, 60},\
        {728, 70},\
        {771, 80},\
        {814, 90},\
        {858, 100},\
        {901, 110},\
        {943, 120},\
        {980, 130},\
        {1011, 140},\
}

