/*
  MathProcess.cpp - Part of TRACER

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

#include "mathProcess.hpp"

/*const float invTable[400] = {1,1.00000, 0.5000000, 0.3333333, 0.2500000, 0.2000000, 0.1666667, 0.1428571, 0.1250000, 0.1111111, 0.1000000, 0.0909091,
                             0.0833333, 0.0769231, 0.0714286, 0.0666667, 0.0625000, 0.0588235, 0.0555556, 0.0526316, 0.0500000, 0.0476190, 0.0454545,
                             0.0434783, 0.0416667, 0.0400000, 0.0384615, 0.0370370, 0.0357143, 0.0344828, 0.0333333, 0.0322581, 0.0312500, 0.0303030,
                             0.0294118, 0.0285714, 0.0277778, 0.0270270, 0.0263158, 0.0256410, 0.0250000, 0.0243902, 0.0238095, 0.0232558, 0.0227273,
                             0.0222222, 0.0217391, 0.0212766, 0.0208333, 0.0204082, 0.0200000, 0.0196078, 0.0192308, 0.0188679, 0.0185185, 0.0181818,
                             0.0178571, 0.0175439, 0.0172414, 0.0169492, 0.0166667, 0.0163934, 0.0161290, 0.0158730, 0.0156250, 0.0153846, 0.0151515,
                             0.0149254, 0.0147059, 0.0144928, 0.0142857, 0.0140845, 0.0138889, 0.0136986, 0.0135135, 0.0133333, 0.0131579, 0.0129870,
                             0.0128205, 0.0126582, 0.0125000, 0.0123457, 0.0121951, 0.0120482, 0.0119048, 0.0117647, 0.0116279, 0.0114943, 0.0113636,
                             0.0112360, 0.0111111, 0.0109890, 0.0108696, 0.0107527, 0.0106383, 0.0105263, 0.0104167, 0.0103093, 0.0102041, 0.0101010,
                             0.0100000, 0.0099010, 0.0098039, 0.0097087, 0.0096154, 0.0095238, 0.0094340, 0.0093458, 0.0092593, 0.0091743, 0.0090909,
                             0.0090090, 0.0089286, 0.0088496, 0.0087719, 0.0086957, 0.0086207, 0.0085470, 0.0084746, 0.0084034, 0.0083333, 0.0082645,
                             0.0081967, 0.0081301, 0.0080645, 0.0080000, 0.0079365, 0.0078740, 0.0078125, 0.0077519, 0.0076923, 0.0076336, 0.0075758,
                             0.0075188, 0.0074627, 0.0074074, 0.0073529, 0.0072993, 0.0072464, 0.0071942, 0.0071429, 0.0070922, 0.0070423, 0.0069930,
                             0.0069444, 0.0068966, 0.0068493, 0.0068027, 0.0067568, 0.0067114, 0.0066667, 0.0066225, 0.0065789, 0.0065359, 0.0064935,
                             0.0064516, 0.0064103, 0.0063694, 0.0063291, 0.0062893, 0.0062500, 0.0062112, 0.0061728, 0.0061350, 0.0060976, 0.0060606,
                             0.0060241, 0.0059880, 0.0059524, 0.0059172, 0.0058824, 0.0058480, 0.0058140, 0.0057803, 0.0057471, 0.0057143, 0.0056818,
                             0.0056497, 0.0056180, 0.0055866, 0.0055556, 0.0055249, 0.0054945, 0.0054645, 0.0054348, 0.0054054, 0.0053763, 0.0053476,
                             0.0053191, 0.0052910, 0.0052632, 0.0052356, 0.0052083, 0.0051813, 0.0051546, 0.0051282, 0.0051020, 0.0050761, 0.0050505,
                             0.0050251, 0.0050000, 0.0049751, 0.0049505, 0.0049261, 0.0049020, 0.0048780, 0.0048544, 0.0048309, 0.0048077, 0.0047847,
                             0.0047619, 0.0047393, 0.0047170, 0.0046948, 0.0046729, 0.0046512, 0.0046296, 0.0046083, 0.0045872, 0.0045662, 0.0045455,
                             0.0045249, 0.0045045, 0.0044843, 0.0044643, 0.0044444, 0.0044248, 0.0044053, 0.0043860, 0.0043668, 0.0043478, 0.0043290,
                             0.0043103, 0.0042918, 0.0042735, 0.0042553, 0.0042373, 0.0042194, 0.0042017, 0.0041841, 0.0041667, 0.0041494, 0.0041322,
                             0.0041152, 0.0040984, 0.0040816, 0.0040650, 0.0040486, 0.0040323, 0.0040161, 0.0040000, 0.0039841, 0.0039683, 0.0039526,
                             0.0039370, 0.0039216, 0.0039063, 0.0038911, 0.0038760, 0.0038610, 0.0038462, 0.0038314, 0.0038168, 0.0038023, 0.0037879,
                             0.0037736, 0.0037594, 0.0037453, 0.0037313, 0.0037175, 0.0037037, 0.0036900, 0.0036765, 0.0036630, 0.0036496, 0.0036364,
                             0.0036232, 0.0036101, 0.0035971, 0.0035842, 0.0035714, 0.0035587, 0.0035461, 0.0035336, 0.0035211, 0.0035088, 0.0034965,
                             0.0034843, 0.0034722, 0.0034602, 0.0034483, 0.0034364, 0.0034247, 0.0034130, 0.0034014, 0.0033898, 0.0033784, 0.0033670,
                             0.0033557, 0.0033445, 0.0033333, 0.0033223, 0.0033113, 0.0033003, 0.0032895, 0.0032787, 0.0032680, 0.0032573, 0.0032468,
                             0.0032362, 0.0032258, 0.0032154, 0.0032051, 0.0031949, 0.0031847, 0.0031746, 0.0031646, 0.0031546, 0.0031447, 0.0031348,
                             0.0031250, 0.0031153, 0.0031056, 0.0030960, 0.0030864, 0.0030769, 0.0030675, 0.0030581, 0.0030488, 0.0030395, 0.0030303,
                             0.0030211, 0.0030120, 0.0030030, 0.0029940, 0.0029851, 0.0029762, 0.0029674, 0.0029586, 0.0029499, 0.0029412, 0.0029326,
                             0.0029240, 0.0029155, 0.0029070, 0.0028986, 0.0028902, 0.0028818, 0.0028736, 0.0028653, 0.0028571, 0.0028490, 0.0028409,
                             0.0028329, 0.0028249, 0.0028169, 0.0028090, 0.0028011, 0.0027933, 0.0027855, 0.0027778, 0.0027701, 0.0027624, 0.0027548,
                             0.0027473, 0.0027397, 0.0027322, 0.0027248, 0.0027174, 0.0027100, 0.0027027, 0.0026954, 0.0026882, 0.0026810, 0.0026738,
                             0.0026667, 0.0026596, 0.0026525, 0.0026455, 0.0026385, 0.0026316, 0.0026247, 0.0026178, 0.0026110, 0.0026042, 0.0025974,
                             0.0025907, 0.0025840, 0.0025773, 0.0025707, 0.0025641, 0.0025575, 0.0025510, 0.0025445, 0.0025381, 0.0025316, 0.0025253,
                             0.0025189, 0.0025126, 0.0025063};*/


const float invTable[200] = {1,1.00000, 0.5000000, 0.3333333, 0.2500000, 0.2000000, 0.1666667, 0.1428571, 0.1250000, 0.1111111, 0.1000000, 0.0909091,
                             0.0833333, 0.0769231, 0.0714286, 0.0666667, 0.0625000, 0.0588235, 0.0555556, 0.0526316, 0.0500000, 0.0476190, 0.0454545,
                             0.0434783, 0.0416667, 0.0400000, 0.0384615, 0.0370370, 0.0357143, 0.0344828, 0.0333333, 0.0322581, 0.0312500, 0.0303030,
                             0.0294118, 0.0285714, 0.0277778, 0.0270270, 0.0263158, 0.0256410, 0.0250000, 0.0243902, 0.0238095, 0.0232558, 0.0227273,
                             0.0222222, 0.0217391, 0.0212766, 0.0208333, 0.0204082, 0.0200000, 0.0196078, 0.0192308, 0.0188679, 0.0185185, 0.0181818,
                             0.0178571, 0.0175439, 0.0172414, 0.0169492, 0.0166667, 0.0163934, 0.0161290, 0.0158730, 0.0156250, 0.0153846, 0.0151515,
                             0.0149254, 0.0147059, 0.0144928, 0.0142857, 0.0140845, 0.0138889, 0.0136986, 0.0135135, 0.0133333, 0.0131579, 0.0129870,
                             0.0128205, 0.0126582, 0.0125000, 0.0123457, 0.0121951, 0.0120482, 0.0119048, 0.0117647, 0.0116279, 0.0114943, 0.0113636,
                             0.0112360, 0.0111111, 0.0109890, 0.0108696, 0.0107527, 0.0106383, 0.0105263, 0.0104167, 0.0103093, 0.0102041, 0.0101010,
                             0.0100000, 0.0099010, 0.0098039, 0.0097087, 0.0096154, 0.0095238, 0.0094340, 0.0093458, 0.0092593, 0.0091743, 0.0090909,
                             0.0090090, 0.0089286, 0.0088496, 0.0087719, 0.0086957, 0.0086207, 0.0085470, 0.0084746, 0.0084034, 0.0083333, 0.0082645,
                             0.0081967, 0.0081301, 0.0080645, 0.0080000, 0.0079365, 0.0078740, 0.0078125, 0.0077519, 0.0076923, 0.0076336, 0.0075758,
                             0.0075188, 0.0074627, 0.0074074, 0.0073529, 0.0072993, 0.0072464, 0.0071942, 0.0071429, 0.0070922, 0.0070423, 0.0069930,
                             0.0069444, 0.0068966, 0.0068493, 0.0068027, 0.0067568, 0.0067114, 0.0066667, 0.0066225, 0.0065789, 0.0065359, 0.0064935,
                             0.0064516, 0.0064103, 0.0063694, 0.0063291, 0.0062893, 0.0062500, 0.0062112, 0.0061728, 0.0061350, 0.0060976, 0.0060606,
                             0.0060241, 0.0059880, 0.0059524, 0.0059172, 0.0058824, 0.0058480, 0.0058140, 0.0057803, 0.0057471, 0.0057143, 0.0056818,
                             0.0056497, 0.0056180, 0.0055866, 0.0055556, 0.0055249, 0.0054945, 0.0054645, 0.0054348, 0.0054054, 0.0053763, 0.0053476,
                             0.0053191, 0.0052910, 0.0052632, 0.0052356, 0.0052083, 0.0051813, 0.0051546, 0.0051282, 0.0051020, 0.0050761, 0.0050505,
                             0.0050251};

float invert(int i) {
    return invTable[i];
}

long fact(int n, int j) {
    if ((n<j)||(j==0)) return 1;
    else if (n == j)
        return j;
    else
        return n * fact(n - 1, j);
}

int angleFormat(int degrees) {
    while (degrees>=360) {
        degrees -= 360;

    }
    while (degrees<0){
        degrees += 360;
        return degrees;
    }
    return degrees;

}

