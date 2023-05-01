/*
 *@Note
 PWM output routine:
 TIM1_CH1(PD2)
 This example demonstrates that the TIM_CH1(PD2) pin outputs PWM in PWM mode 1
 and PWM mode 2.

 */

#include "debug.h"

/* PWM Output Mode Definition */
#define PWM_MODE1   0
#define PWM_MODE2

/* PWM Output Mode Selection */
#define PWM_MODE PWM_MODE1
//#define PWM_MODE PWM_MODE2

const uint16_t tonedata[] = { 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62,
        65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139, 147, 156,
        165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349,
        370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784,
        831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568,
        1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136,
        3322, 3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272,
        6645, 7040, 7459, 7902, 8372, 8870, 9397, 9956, 10548, 11175, 11840,
        12544, 13290, 14080, 14917, 15804, 16744, 17740, 18795, 19912, 21096,
        22351, 23680, 25088, 26580, 28160, 29834, 31609, 33488, 35479, 37589,
        39824, 42192, 44701, 47359, 50175 };

const uint8_t notes[] = { 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44,
        0, 43, 0, 43, 0, 43, 0, 43, 0, 44, 0, 44, 0, 44, 0, 44, 0, 48, 0, 48, 0,
        48, 0, 48, 0, 46, 0, 46, 0, 46, 0, 46, 0, 43, 0, 43, 0, 43, 0, 43, 0,
        44, 0, 44, 0, 44, 0, 44, 0, 46, 0, 46, 0, 46, 0, 46, 0, 44, 0, 44, 0,
        44, 0, 44, 0, 48, 0, 48, 0, 48, 0, 48, 0, 46, 0, 46, 0, 46, 0, 46, 0,
        43, 0, 43, 0, 43, 0, 43, 0, 44, 0, 44, 0, 44, 0, 44, 0, 48, 0, 48, 0,
        48, 0, 48, 0, 46, 0, 46, 0, 46, 0, 46, 0, 43, 0, 43, 0, 43, 0, 43, 0,
        44, 0, 44, 0, 44, 0, 44, 0, 48, 0, 48, 0, 48, 0, 48, 0, 46, 0, 46, 0,
        46, 0, 46, 0, 43, 0, 43, 0, 43, 0, 43, 0, 44, 0, 44, 0, 44, 0, 44, 0,
        48, 0, 48, 0, 48, 0, 48, 0, 46, 0, 46, 0, 46, 0, 46, 0, 43, 25, 43, 25,
        43, 25, 43, 25, 44, 25, 44, 25, 44, 25, 44, 25, 48, 32, 48, 41, 48, 32,
        48, 41, 46, 32, 46, 41, 46, 32, 46, 0, 43, 32, 43, 41, 43, 32, 43, 41,
        44, 32, 44, 41, 44, 32, 44, 41, 48, 29, 48, 29, 48, 29, 48, 29, 46, 29,
        46, 29, 46, 29, 46, 29, 43, 32, 43, 41, 43, 32, 43, 41, 44, 32, 44, 41,
        44, 32, 44, 0, 48, 32, 48, 41, 48, 32, 48, 41, 46, 32, 46, 41, 46, 32,
        46, 41, 43, 25, 43, 25, 43, 25, 43, 25, 44, 25, 44, 25, 44, 25, 44, 25,
        48, 32, 48, 41, 48, 32, 48, 41, 46, 32, 46, 41, 46, 32, 46, 0, 43, 32,
        43, 41, 43, 32, 43, 41, 44, 32, 44, 41, 44, 32, 44, 41, 46, 29, 46, 29,
        46, 29, 46, 29, 48, 29, 48, 29, 48, 29, 48, 29, 49, 32, 49, 41, 49, 32,
        49, 41, 51, 32, 51, 41, 51, 32, 51, 0, 53, 32, 53, 41, 53, 32, 53, 41,
        54, 32, 54, 41, 54, 32, 54, 41, 58, 20, 58, 20, 58, 20, 58, 20, 58, 20,
        58, 20, 58, 20, 58, 20, 58, 32, 58, 42, 58, 32, 58, 42, 56, 32, 56, 42,
        56, 32, 56, 0, 54, 32, 54, 42, 54, 32, 54, 42, 53, 32, 53, 42, 53, 32,
        0, 42, 53, 27, 53, 27, 53, 27, 53, 27, 51, 27, 51, 27, 51, 27, 0, 27,
        51, 32, 51, 42, 51, 32, 51, 42, 50, 32, 50, 42, 50, 32, 50, 0, 51, 32,
        51, 42, 51, 32, 51, 42, 51, 32, 51, 42, 51, 32, 51, 42, 58, 20, 58, 20,
        58, 20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 36, 58, 42, 58, 36,
        58, 42, 56, 36, 56, 42, 56, 36, 56, 0, 54, 36, 54, 42, 54, 36, 54, 42,
        53, 36, 53, 42, 53, 36, 0, 42, 53, 32, 53, 32, 53, 32, 53, 32, 51, 32,
        51, 32, 51, 32, 51, 32, 50, 36, 50, 42, 50, 36, 50, 42, 51, 36, 51, 42,
        51, 36, 51, 0, 53, 36, 53, 42, 53, 36, 53, 42, 46, 36, 46, 42, 46, 36,
        46, 42, 43, 25, 43, 25, 43, 25, 43, 25, 44, 25, 44, 25, 44, 25, 44, 25,
        48, 32, 48, 41, 48, 32, 48, 41, 46, 32, 46, 41, 46, 32, 46, 0, 43, 32,
        43, 41, 43, 32, 43, 41, 44, 32, 44, 41, 44, 32, 44, 41, 48, 29, 48, 29,
        48, 29, 48, 29, 46, 29, 46, 29, 46, 29, 46, 29, 43, 32, 43, 41, 43, 32,
        43, 41, 44, 32, 44, 41, 44, 32, 44, 0, 48, 32, 48, 41, 48, 32, 48, 41,
        46, 32, 46, 41, 46, 32, 46, 41, 43, 25, 43, 25, 43, 25, 43, 25, 44, 25,
        44, 25, 44, 25, 44, 25, 48, 32, 48, 41, 48, 32, 48, 41, 46, 32, 46, 41,
        46, 32, 46, 0, 43, 32, 43, 41, 43, 32, 43, 41, 44, 32, 44, 41, 44, 32,
        44, 41, 46, 29, 46, 29, 46, 29, 46, 29, 48, 29, 48, 29, 48, 29, 48, 29,
        49, 32, 49, 41, 49, 32, 49, 41, 51, 32, 51, 41, 51, 32, 51, 0, 53, 32,
        53, 41, 53, 32, 53, 41, 54, 32, 54, 41, 54, 32, 54, 41, 58, 20, 58, 20,
        58, 20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 32, 58, 42, 58, 32,
        58, 42, 56, 32, 56, 42, 56, 32, 56, 0, 54, 32, 54, 42, 54, 32, 54, 42,
        53, 32, 53, 42, 53, 32, 0, 42, 53, 27, 53, 27, 53, 27, 53, 27, 51, 27,
        51, 27, 51, 27, 0, 27, 51, 32, 51, 42, 51, 32, 51, 42, 50, 32, 50, 42,
        50, 32, 50, 0, 51, 32, 51, 42, 51, 32, 51, 42, 51, 32, 51, 42, 51, 32,
        51, 42, 58, 20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 20,
        58, 32, 58, 42, 58, 32, 58, 42, 56, 32, 56, 42, 56, 32, 56, 0, 54, 32,
        54, 42, 54, 32, 54, 42, 53, 32, 53, 42, 53, 32, 53, 42, 51, 32, 51, 32,
        51, 32, 51, 32, 53, 32, 53, 32, 53, 32, 53, 32, 51, 36, 51, 42, 51, 36,
        51, 42, 50, 36, 50, 42, 50, 36, 50, 42, 51, 20, 51, 20, 51, 20, 51, 20,
        52, 20, 52, 20, 52, 20, 52, 20, 53, 21, 53, 21, 54, 21, 54, 21, 54, 21,
        53, 21, 53, 21, 53, 21, 52, 29, 52, 39, 52, 29, 52, 39, 53, 29, 53, 39,
        53, 29, 53, 0, 56, 29, 56, 39, 56, 29, 56, 39, 54, 29, 54, 39, 54, 29,
        54, 39, 53, 22, 53, 22, 53, 22, 53, 22, 54, 22, 54, 22, 54, 22, 54, 22,
        53, 29, 53, 37, 53, 29, 53, 37, 52, 29, 52, 37, 52, 29, 52, 0, 53, 29,
        53, 37, 53, 29, 53, 37, 58, 29, 58, 37, 58, 29, 58, 37, 56, 24, 56, 24,
        56, 24, 58, 24, 58, 24, 58, 24, 56, 24, 56, 24, 55, 32, 55, 42, 55, 32,
        55, 42, 56, 32, 56, 42, 56, 32, 56, 0, 60, 32, 60, 42, 60, 32, 60, 42,
        58, 32, 58, 42, 58, 32, 58, 42, 56, 25, 56, 25, 56, 25, 56, 25, 58, 25,
        58, 25, 58, 25, 58, 25, 56, 32, 56, 41, 56, 32, 56, 41, 55, 32, 55, 41,
        55, 32, 55, 41, 56, 0, 56, 0, 56, 0, 56, 0, 61, 0, 61, 0, 61, 0, 61, 0,
        60, 30, 60, 30, 60, 30, 60, 30, 58, 30, 58, 30, 58, 30, 58, 32, 56, 34,
        56, 39, 56, 34, 56, 39, 54, 34, 54, 39, 54, 34, 54, 39, 53, 0, 53, 0,
        53, 0, 53, 0, 51, 0, 51, 0, 51, 0, 51, 0, 49, 20, 49, 20, 49, 20, 49,
        20, 48, 20, 48, 20, 48, 20, 48, 20, 46, 29, 46, 37, 46, 29, 46, 37, 44,
        29, 44, 37, 44, 29, 44, 37, 42, 0, 42, 0, 42, 0, 42, 0, 41, 0, 41, 0,
        41, 0, 41, 0, 39, 20, 39, 20, 39, 20, 39, 20, 37, 20, 37, 20, 37, 20,
        37, 20, 36, 30, 36, 32, 36, 30, 36, 32, 39, 30, 39, 32, 39, 30, 39, 0,
        46, 30, 46, 36, 46, 30, 46, 36, 44, 30, 44, 36, 44, 30, 44, 36, 43, 25,
        43, 25, 43, 25, 43, 25, 44, 25, 44, 25, 44, 25, 44, 25, 46, 32, 46, 41,
        46, 32, 46, 41, 48, 32, 48, 41, 48, 32, 48, 41, 49, 0, 49, 0, 49, 0, 49,
        0, 51, 0, 51, 0, 51, 0, 51, 0, 53, 32, 53, 32, 53, 32, 54, 32, 54, 32,
        54, 32, 53, 32, 53, 32, 52, 39, 52, 41, 52, 39, 52, 41, 53, 39, 53, 41,
        53, 39, 53, 0, 56, 39, 56, 41, 56, 39, 56, 41, 54, 39, 54, 41, 54, 39,
        54, 41, 53, 34, 53, 34, 53, 34, 53, 34, 54, 34, 54, 34, 54, 34, 54, 34,
        53, 37, 53, 41, 53, 37, 53, 41, 52, 37, 52, 41, 52, 37, 52, 0, 53, 37,
        53, 41, 53, 37, 53, 41, 58, 37, 58, 41, 58, 37, 58, 41, 56, 36, 56, 36,
        56, 36, 58, 36, 58, 36, 58, 36, 56, 36, 56, 36, 55, 42, 55, 44, 55, 42,
        55, 44, 56, 42, 56, 44, 56, 42, 56, 0, 60, 42, 60, 44, 60, 42, 60, 44,
        58, 42, 58, 44, 58, 42, 58, 44, 56, 37, 56, 37, 56, 37, 56, 37, 58, 37,
        58, 37, 58, 37, 58, 37, 56, 41, 56, 44, 56, 41, 56, 44, 55, 41, 55, 44,
        55, 41, 55, 44, 56, 0, 56, 0, 56, 0, 56, 0, 65, 0, 65, 0, 65, 0, 65, 0,
        63, 30, 63, 30, 63, 30, 63, 30, 61, 30, 61, 30, 61, 30, 61, 30, 60, 37,
        60, 46, 60, 37, 60, 46, 58, 37, 58, 46, 58, 37, 58, 46, 56, 0, 56, 0,
        56, 0, 56, 0, 54, 0, 54, 0, 54, 0, 54, 0, 53, 20, 53, 20, 53, 20, 53,
        20, 51, 20, 51, 20, 51, 20, 51, 20, 49, 29, 49, 37, 49, 29, 49, 37, 48,
        29, 48, 37, 48, 29, 48, 37, 46, 0, 46, 0, 46, 0, 46, 0, 44, 0, 44, 0,
        44, 0, 44, 0, 45, 20, 45, 20, 45, 20, 45, 20, 48, 20, 48, 20, 48, 20,
        48, 20, 46, 30, 46, 32, 46, 30, 46, 32, 41, 30, 41, 32, 41, 30, 41, 0,
        42, 30, 42, 32, 42, 30, 42, 32, 36, 30, 36, 32, 36, 30, 36, 32, 37, 25,
        37, 25, 37, 25, 37, 25, 37, 25, 37, 25, 37, 25, 37, 29, 0, 32, 0, 37, 0,
        32, 0, 37, 0, 32, 0, 37, 0, 32, 0, 37, 53, 0, 53, 0, 53, 0, 53, 0, 53,
        0, 53, 0, 53, 0, 53, 0, 53, 21, 53, 21, 54, 21, 54, 21, 54, 21, 53, 21,
        53, 21, 53, 21, 52, 29, 52, 39, 52, 29, 52, 39, 53, 29, 53, 39, 53, 29,
        53, 0, 56, 29, 56, 39, 56, 29, 56, 39, 54, 29, 54, 39, 54, 29, 54, 39,
        53, 22, 53, 22, 53, 22, 53, 22, 54, 22, 54, 22, 54, 22, 54, 22, 53, 29,
        53, 37, 53, 29, 53, 37, 52, 29, 52, 37, 52, 29, 52, 0, 53, 29, 53, 37,
        53, 29, 53, 37, 58, 29, 58, 37, 58, 29, 58, 37, 56, 24, 56, 24, 56, 24,
        58, 24, 58, 24, 58, 24, 56, 24, 56, 24, 55, 32, 55, 42, 55, 32, 55, 42,
        56, 32, 56, 42, 56, 32, 56, 0, 60, 32, 60, 42, 60, 32, 60, 42, 58, 32,
        58, 42, 58, 32, 58, 42, 56, 25, 56, 25, 56, 25, 56, 25, 58, 25, 58, 25,
        58, 25, 58, 25, 56, 32, 56, 41, 56, 32, 56, 41, 55, 32, 55, 41, 55, 32,
        55, 41, 56, 0, 56, 0, 56, 0, 56, 0, 61, 0, 61, 0, 61, 0, 61, 0, 60, 30,
        60, 30, 60, 30, 60, 30, 58, 30, 58, 30, 58, 30, 58, 32, 56, 34, 56, 39,
        56, 34, 56, 39, 54, 34, 54, 39, 54, 34, 54, 39, 53, 0, 53, 0, 53, 0, 53,
        0, 51, 0, 51, 0, 51, 0, 51, 0, 49, 20, 49, 20, 49, 20, 49, 20, 48, 20,
        48, 20, 48, 20, 48, 20, 46, 29, 46, 37, 46, 29, 46, 37, 44, 29, 44, 37,
        44, 29, 44, 37, 42, 0, 42, 0, 42, 0, 42, 0, 41, 0, 41, 0, 41, 0, 41, 0,
        39, 20, 39, 20, 39, 20, 39, 20, 37, 20, 37, 20, 37, 20, 37, 20, 36, 30,
        36, 32, 36, 30, 36, 32, 39, 30, 39, 32, 39, 30, 39, 0, 46, 30, 46, 36,
        46, 30, 46, 36, 44, 30, 44, 36, 44, 30, 44, 36, 43, 25, 43, 25, 43, 25,
        43, 25, 44, 25, 44, 25, 44, 25, 44, 25, 46, 32, 46, 41, 46, 32, 46, 41,
        48, 32, 48, 41, 48, 32, 48, 41, 49, 0, 49, 0, 49, 0, 49, 0, 51, 0, 51,
        0, 51, 0, 51, 0, 53, 32, 53, 32, 53, 32, 54, 32, 54, 32, 54, 32, 53, 32,
        53, 32, 52, 39, 52, 41, 52, 39, 52, 41, 53, 39, 53, 41, 53, 39, 53, 0,
        56, 39, 56, 41, 56, 39, 56, 41, 54, 39, 54, 41, 54, 39, 54, 41, 53, 34,
        53, 34, 53, 34, 53, 34, 54, 34, 54, 34, 54, 34, 54, 34, 53, 37, 53, 41,
        53, 37, 53, 41, 52, 37, 52, 41, 52, 37, 52, 0, 53, 37, 53, 41, 53, 37,
        53, 41, 58, 37, 58, 41, 58, 37, 58, 41, 56, 36, 56, 36, 56, 36, 58, 36,
        58, 36, 58, 36, 56, 36, 56, 36, 55, 42, 55, 44, 55, 42, 55, 44, 56, 42,
        56, 44, 56, 42, 56, 0, 60, 42, 60, 44, 60, 42, 60, 44, 58, 42, 58, 44,
        58, 42, 58, 44, 56, 37, 56, 37, 56, 37, 56, 37, 58, 37, 58, 37, 58, 37,
        58, 37, 56, 41, 56, 44, 56, 41, 56, 44, 55, 41, 55, 44, 55, 41, 55, 44,
        56, 0, 56, 0, 56, 0, 56, 0, 65, 0, 65, 0, 65, 0, 65, 0, 63, 30, 63, 30,
        63, 30, 63, 30, 61, 30, 61, 30, 61, 30, 61, 30, 60, 37, 60, 46, 60, 37,
        60, 46, 58, 37, 58, 46, 58, 37, 58, 46, 56, 0, 56, 0, 56, 0, 56, 0, 54,
        0, 54, 0, 54, 0, 54, 0, 53, 20, 53, 20, 53, 20, 53, 20, 51, 20, 51, 20,
        51, 20, 51, 20, 49, 29, 49, 37, 49, 29, 49, 37, 48, 29, 48, 37, 48, 29,
        48, 37, 46, 0, 46, 0, 46, 0, 46, 0, 44, 0, 44, 0, 44, 0, 44, 0, 45, 20,
        45, 20, 45, 20, 45, 20, 48, 20, 48, 20, 48, 20, 48, 20, 46, 30, 46, 32,
        46, 30, 46, 32, 41, 30, 41, 32, 41, 30, 41, 0, 42, 30, 42, 32, 42, 30,
        42, 32, 36, 30, 36, 32, 36, 30, 36, 32, 37, 25, 37, 25, 37, 25, 37, 25,
        37, 25, 37, 25, 37, 25, 37, 29, 0, 32, 0, 37, 0, 32, 0, 37, 0, 32, 0,
        37, 0, 32, 0, 37, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44,
        0, 44, 20, 44, 20, 44, 20, 44, 20, 44, 20, 44, 20, 44, 20, 44, 20, 44,
        30, 44, 36, 44, 30, 44, 36, 44, 30, 44, 36, 44, 30, 44, 0, 39, 30, 39,
        36, 39, 30, 39, 36, 39, 30, 39, 36, 39, 30, 39, 36, 44, 30, 44, 36, 44,
        30, 44, 36, 44, 30, 44, 36, 44, 30, 44, 0, 44, 30, 44, 36, 44, 30, 44,
        36, 44, 30, 44, 36, 44, 30, 44, 0, 40, 30, 40, 36, 40, 30, 40, 36, 40,
        30, 40, 36, 40, 30, 40, 0, 44, 25, 44, 25, 44, 25, 44, 25, 44, 25, 44,
        25, 44, 25, 44, 25, 44, 32, 44, 37, 44, 32, 44, 37, 44, 32, 44, 37, 44,
        32, 44, 0, 41, 32, 41, 37, 41, 32, 41, 37, 41, 32, 41, 37, 41, 32, 41,
        37, 53, 20, 53, 20, 53, 20, 53, 20, 53, 20, 53, 20, 53, 20, 53, 20, 53,
        32, 53, 41, 53, 32, 53, 41, 53, 32, 53, 41, 53, 32, 0, 0, 53, 32, 53,
        41, 53, 32, 53, 41, 53, 32, 53, 41, 53, 32, 53, 41, 53, 27, 53, 27, 53,
        27, 53, 27, 53, 27, 53, 27, 53, 27, 53, 27, 53, 32, 53, 42, 53, 32, 53,
        42, 53, 32, 53, 42, 53, 32, 53, 0, 46, 32, 46, 42, 46, 32, 46, 42, 46,
        32, 46, 42, 46, 32, 46, 42, 53, 20, 53, 20, 53, 20, 53, 20, 53, 20, 53,
        20, 53, 20, 53, 20, 53, 32, 53, 42, 53, 32, 53, 42, 53, 32, 53, 42, 53,
        32, 53, 0, 48, 32, 48, 42, 48, 32, 48, 42, 48, 32, 48, 42, 48, 32, 48,
        42, 51, 25, 51, 25, 51, 25, 51, 25, 51, 25, 51, 25, 51, 25, 51, 25, 51,
        32, 51, 41, 51, 32, 51, 41, 51, 32, 51, 41, 51, 32, 51, 0, 49, 32, 49,
        41, 49, 32, 49, 41, 49, 32, 49, 41, 49, 32, 49, 41, 48, 29, 48, 29, 48,
        29, 48, 29, 48, 29, 48, 29, 51, 29, 51, 29, 51, 32, 51, 41, 51, 32, 51,
        41, 49, 32, 49, 41, 49, 32, 49, 0, 49, 32, 49, 41, 46, 32, 46, 41, 46,
        32, 46, 41, 46, 32, 46, 41, 44, 27, 44, 27, 44, 27, 44, 27, 44, 27, 44,
        27, 44, 27, 44, 27, 44, 30, 44, 32, 44, 30, 44, 32, 44, 30, 44, 32, 44,
        30, 44, 0, 39, 30, 39, 32, 39, 30, 39, 32, 39, 30, 39, 32, 39, 30, 39,
        32, 44, 20, 44, 20, 44, 20, 44, 20, 44, 20, 44, 20, 44, 20, 44, 20, 44,
        30, 44, 36, 44, 30, 44, 36, 44, 30, 44, 36, 44, 30, 44, 0, 40, 30, 40,
        36, 40, 30, 40, 36, 40, 30, 40, 36, 40, 30, 40, 36, 44, 25, 44, 25, 44,
        25, 44, 25, 44, 25, 44, 25, 44, 25, 44, 25, 44, 32, 44, 37, 44, 32, 44,
        37, 44, 32, 44, 37, 44, 32, 44, 0, 41, 32, 41, 37, 41, 32, 41, 37, 41,
        32, 41, 37, 41, 32, 41, 37, 53, 23, 53, 23, 53, 23, 53, 23, 53, 23, 53,
        23, 53, 23, 53, 23, 53, 32, 53, 41, 53, 32, 53, 41, 53, 32, 53, 41, 53,
        32, 53, 0, 53, 32, 53, 41, 53, 32, 53, 41, 53, 32, 53, 41, 53, 32, 53,
        41, 48, 24, 48, 24, 48, 24, 48, 24, 48, 24, 48, 24, 48, 24, 48, 24, 47,
        32, 47, 41, 47, 32, 47, 41, 47, 32, 47, 41, 47, 32, 47, 0, 48, 32, 48,
        41, 48, 32, 48, 41, 48, 32, 48, 41, 48, 32, 48, 41, 56, 12, 56, 12, 56,
        12, 56, 12, 56, 12, 56, 12, 56, 12, 56, 12, 46, 31, 46, 40, 46, 31, 46,
        40, 46, 31, 46, 40, 46, 31, 46, 0, 55, 31, 55, 40, 55, 31, 55, 40, 55,
        31, 55, 40, 55, 31, 55, 40, 45, 17, 45, 17, 45, 17, 45, 17, 45, 17, 45,
        17, 45, 17, 45, 17, 54, 0, 54, 0, 54, 0, 54, 0, 54, 0, 54, 0, 54, 0, 54,
        0, 44, 29, 44, 36, 44, 29, 44, 36, 44, 29, 44, 36, 44, 29, 44, 36, 53,
        0, 53, 0, 53, 0, 53, 0, 53, 0, 53, 0, 53, 0, 53, 0, 41, 0, 41, 0, 41, 0,
        41, 0, 41, 0, 41, 0, 41, 0, 41, 0, 46, 0, 46, 0, 46, 0, 46, 0, 46, 0,
        46, 0, 46, 0, 46, 0, 44, 24, 44, 24, 44, 24, 44, 24, 44, 24, 44, 24, 44,
        24, 44, 24, 44, 30, 44, 32, 44, 30, 44, 32, 44, 30, 44, 32, 44, 30, 44,
        0, 39, 30, 39, 32, 39, 30, 39, 32, 39, 30, 39, 32, 39, 30, 39, 32, 56,
        20, 53, 20, 44, 20, 44, 20, 44, 20, 44, 20, 44, 20, 44, 20, 44, 30, 44,
        36, 44, 30, 44, 36, 44, 30, 44, 36, 44, 30, 44, 0, 40, 30, 40, 36, 40,
        30, 40, 36, 40, 30, 40, 36, 40, 30, 40, 36, 44, 25, 44, 25, 44, 25, 44,
        25, 44, 25, 44, 25, 44, 25, 44, 25, 44, 32, 44, 37, 44, 32, 44, 37, 44,
        32, 44, 37, 44, 32, 44, 0, 41, 32, 41, 37, 41, 32, 41, 37, 41, 32, 41,
        37, 41, 32, 41, 37, 56, 20, 53, 20, 53, 20, 53, 20, 53, 20, 53, 20, 53,
        20, 53, 20, 53, 32, 53, 41, 53, 32, 53, 41, 53, 32, 53, 41, 53, 32, 0,
        0, 53, 32, 53, 41, 53, 32, 53, 41, 53, 32, 53, 41, 53, 32, 53, 41, 56,
        27, 53, 27, 53, 27, 53, 27, 53, 27, 53, 27, 53, 27, 53, 27, 53, 32, 53,
        42, 53, 32, 53, 42, 53, 32, 53, 42, 53, 32, 53, 0, 48, 32, 48, 42, 48,
        32, 48, 42, 48, 32, 48, 42, 48, 32, 48, 42, 56, 20, 51, 20, 51, 20, 51,
        20, 51, 20, 51, 20, 51, 20, 51, 20, 49, 32, 49, 42, 49, 32, 49, 42, 49,
        32, 49, 42, 49, 32, 49, 0, 48, 32, 48, 42, 48, 32, 48, 42, 48, 32, 48,
        42, 48, 32, 48, 42, 56, 25, 51, 25, 51, 25, 51, 25, 51, 25, 51, 25, 51,
        25, 51, 25, 49, 32, 49, 41, 49, 32, 49, 41, 49, 32, 49, 41, 49, 32, 49,
        0, 49, 32, 49, 41, 49, 32, 49, 41, 46, 32, 46, 41, 46, 32, 46, 41, 56,
        29, 44, 29, 44, 29, 44, 29, 44, 29, 44, 29, 44, 29, 44, 29, 44, 32, 44,
        41, 44, 32, 44, 41, 44, 32, 44, 41, 44, 32, 44, 0, 39, 32, 39, 41, 39,
        32, 39, 41, 39, 32, 39, 41, 39, 32, 39, 41, 56, 24, 44, 24, 44, 24, 44,
        24, 44, 24, 44, 24, 44, 24, 44, 24, 44, 30, 44, 32, 44, 30, 44, 32, 44,
        30, 44, 32, 44, 30, 44, 0, 40, 30, 40, 32, 40, 30, 40, 32, 40, 30, 40,
        32, 40, 30, 40, 32, 56, 20, 44, 20, 44, 20, 44, 20, 44, 20, 44, 20, 44,
        20, 44, 20, 44, 30, 44, 36, 44, 30, 44, 36, 44, 30, 44, 36, 44, 30, 44,
        0, 41, 30, 41, 36, 41, 30, 41, 36, 41, 30, 41, 36, 41, 30, 41, 36, 53,
        23, 53, 23, 53, 23, 53, 23, 53, 23, 53, 23, 53, 23, 53, 23, 53, 29, 53,
        39, 53, 29, 53, 39, 53, 29, 53, 39, 53, 29, 53, 0, 53, 29, 53, 39, 53,
        29, 53, 39, 53, 29, 53, 39, 53, 29, 0, 39, 53, 22, 53, 22, 53, 22, 53,
        22, 53, 22, 53, 22, 53, 22, 53, 22, 53, 29, 53, 38, 53, 29, 53, 38, 53,
        29, 53, 38, 53, 29, 53, 0, 46, 29, 46, 38, 46, 29, 46, 38, 46, 29, 46,
        38, 46, 29, 46, 38, 51, 27, 51, 27, 51, 27, 51, 27, 51, 27, 51, 27, 51,
        27, 51, 27, 51, 37, 51, 43, 51, 37, 51, 43, 51, 37, 51, 43, 51, 37, 51,
        0, 46, 37, 46, 43, 46, 37, 46, 43, 46, 37, 46, 43, 46, 37, 46, 43, 51,
        0, 51, 0, 51, 0, 51, 0, 51, 0, 51, 0, 51, 0, 51, 0, 51, 37, 51, 42, 51,
        37, 51, 42, 51, 37, 51, 42, 51, 37, 51, 0, 46, 37, 46, 42, 46, 37, 46,
        42, 46, 37, 46, 42, 46, 37, 46, 42, 51, 32, 51, 32, 51, 32, 51, 32, 51,
        32, 51, 32, 51, 32, 51, 32, 44, 36, 44, 42, 44, 36, 44, 42, 44, 36, 44,
        42, 44, 36, 44, 42, 50, 0, 50, 0, 50, 0, 50, 0, 50, 0, 50, 0, 50, 0, 50,
        0, 53, 0, 53, 0, 53, 0, 53, 0, 53, 0, 53, 0, 53, 0, 53, 0, 51, 0, 51, 0,
        51, 0, 51, 0, 51, 0, 51, 0, 51, 0, 51, 0, 56, 0, 56, 0, 56, 0, 56, 0,
        56, 0, 56, 0, 56, 0, 56, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0,
        44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0,
        44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0,
        44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0,
        44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0,
        44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0,
        44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0,
        44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0,
        44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0,
        44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0, 44, 0,
        43, 0, 43, 0, 43, 0, 43, 0, 44, 0, 44, 0, 44, 0, 44, 0, 48, 0, 48, 0,
        48, 0, 48, 0, 46, 0, 46, 0, 46, 0, 46, 0, 43, 0, 43, 0, 43, 0, 43, 0,
        44, 0, 44, 0, 44, 0, 44, 0, 48, 0, 48, 0, 48, 0, 48, 0, 46, 0, 46, 0,
        46, 0, 46, 0, 43, 0, 43, 0, 43, 0, 43, 0, 44, 0, 44, 0, 44, 0, 44, 0,
        48, 0, 48, 0, 48, 0, 48, 0, 46, 0, 46, 0, 46, 0, 46, 0, 43, 0, 43, 0,
        43, 0, 43, 0, 44, 0, 44, 0, 44, 0, 44, 0, 48, 0, 48, 0, 48, 0, 48, 0,
        46, 0, 46, 0, 46, 0, 46, 0, 43, 0, 43, 0, 43, 0, 43, 0, 44, 0, 44, 0,
        44, 0, 44, 0, 48, 0, 48, 0, 48, 0, 48, 0, 46, 0, 46, 0, 46, 0, 46, 0,
        43, 0, 43, 0, 43, 0, 43, 0, 44, 0, 44, 0, 44, 0, 44, 0, 48, 0, 48, 0,
        48, 0, 48, 0, 46, 0, 46, 0, 46, 0, 46, 0, 43, 13, 43, 13, 43, 13, 43,
        13, 44, 13, 44, 13, 44, 13, 44, 13, 48, 32, 48, 41, 48, 32, 48, 41, 46,
        32, 46, 41, 46, 32, 46, 0, 43, 32, 43, 41, 43, 32, 43, 41, 44, 32, 44,
        41, 44, 32, 44, 41, 48, 29, 48, 29, 48, 29, 48, 29, 46, 29, 46, 29, 46,
        29, 46, 29, 43, 32, 43, 41, 43, 32, 43, 41, 44, 32, 44, 41, 44, 32, 44,
        0, 48, 32, 48, 41, 48, 32, 48, 41, 46, 32, 46, 41, 46, 32, 46, 41, 43,
        25, 43, 25, 43, 25, 43, 25, 44, 25, 44, 25, 44, 25, 44, 25, 48, 32, 48,
        41, 48, 32, 48, 41, 46, 32, 46, 41, 46, 32, 46, 0, 43, 32, 43, 41, 43,
        32, 43, 41, 44, 32, 44, 41, 44, 32, 44, 41, 46, 29, 46, 29, 46, 29, 46,
        29, 48, 29, 48, 29, 48, 29, 48, 29, 49, 32, 49, 41, 49, 32, 49, 41, 51,
        32, 51, 41, 51, 32, 51, 0, 53, 32, 53, 41, 53, 32, 53, 41, 54, 32, 54,
        41, 54, 32, 54, 41, 58, 20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 20, 58,
        20, 58, 20, 58, 32, 58, 42, 58, 32, 58, 42, 56, 32, 56, 42, 56, 32, 56,
        0, 54, 32, 54, 42, 54, 32, 54, 42, 53, 32, 53, 42, 53, 32, 0, 42, 53,
        27, 53, 27, 53, 27, 53, 27, 51, 27, 51, 27, 51, 27, 0, 27, 51, 32, 51,
        42, 51, 32, 51, 42, 50, 32, 50, 42, 50, 32, 50, 0, 51, 32, 51, 42, 51,
        32, 51, 42, 51, 32, 51, 42, 51, 32, 51, 42, 58, 20, 58, 20, 58, 20, 58,
        20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 36, 58, 42, 58, 36, 58, 42, 56,
        36, 56, 42, 56, 36, 56, 0, 54, 36, 54, 42, 54, 36, 54, 42, 53, 36, 53,
        42, 53, 36, 0, 42, 53, 32, 53, 32, 53, 32, 53, 32, 51, 32, 51, 32, 51,
        32, 51, 32, 50, 36, 50, 42, 50, 36, 50, 42, 51, 36, 51, 42, 51, 36, 51,
        0, 53, 36, 53, 42, 53, 36, 53, 42, 46, 36, 46, 42, 46, 36, 46, 42, 43,
        25, 43, 25, 43, 25, 43, 25, 44, 25, 44, 25, 44, 25, 44, 25, 48, 32, 48,
        41, 48, 32, 48, 41, 46, 32, 46, 41, 46, 32, 46, 0, 43, 32, 43, 41, 43,
        32, 43, 41, 44, 32, 44, 41, 44, 32, 44, 41, 48, 29, 48, 29, 48, 29, 48,
        29, 46, 29, 46, 29, 46, 29, 46, 29, 43, 32, 43, 41, 43, 32, 43, 41, 44,
        32, 44, 41, 44, 32, 44, 0, 48, 32, 48, 41, 48, 32, 48, 41, 46, 32, 46,
        41, 46, 32, 46, 41, 43, 25, 43, 25, 43, 25, 43, 25, 44, 25, 44, 25, 44,
        25, 44, 25, 48, 32, 48, 41, 48, 32, 48, 41, 46, 32, 46, 41, 46, 32, 46,
        0, 43, 32, 43, 41, 43, 32, 43, 41, 44, 32, 44, 41, 44, 32, 44, 41, 46,
        29, 46, 29, 46, 29, 46, 29, 48, 29, 48, 29, 48, 29, 48, 29, 49, 32, 49,
        41, 49, 32, 49, 41, 51, 32, 51, 41, 51, 32, 51, 0, 53, 32, 53, 41, 53,
        32, 53, 41, 54, 32, 54, 41, 54, 32, 54, 41, 58, 20, 58, 20, 58, 20, 58,
        20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 32, 58, 42, 58, 32, 58, 42, 56,
        32, 56, 42, 56, 32, 56, 0, 54, 32, 54, 42, 54, 32, 54, 42, 53, 32, 53,
        42, 53, 32, 0, 42, 53, 27, 53, 27, 53, 27, 53, 27, 51, 27, 51, 27, 51,
        27, 0, 27, 51, 32, 51, 42, 51, 32, 51, 42, 50, 32, 50, 42, 50, 32, 50,
        0, 51, 32, 51, 42, 51, 32, 51, 42, 51, 32, 51, 42, 51, 32, 51, 42, 58,
        20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 20, 58, 32, 58,
        42, 58, 32, 58, 42, 56, 32, 56, 42, 56, 32, 56, 0, 54, 32, 54, 42, 54,
        32, 54, 42, 53, 32, 53, 42, 53, 32, 53, 42, 51, 27, 51, 27, 51, 27, 51,
        27, 53, 27, 53, 27, 53, 27, 53, 27, 51, 32, 51, 42, 51, 32, 51, 42, 50,
        32, 50, 42, 50, 32, 50, 42, 51, 20, 51, 20, 51, 20, 51, 20, 52, 20, 52,
        20, 52, 20, 52, 20, 53, 21, 53, 21, 53, 21, 54, 21, 54, 21, 54, 21, 53,
        21, 53, 21, 52, 29, 52, 35, 52, 29, 52, 35, 53, 29, 53, 35, 53, 29, 53,
        0, 56, 29, 56, 35, 56, 29, 56, 35, 54, 29, 54, 35, 54, 29, 54, 35, 53,
        22, 53, 22, 53, 22, 53, 22, 54, 22, 54, 22, 54, 22, 54, 22, 53, 29, 53,
        37, 53, 29, 53, 37, 52, 29, 52, 37, 52, 29, 52, 0, 53, 29, 53, 37, 53,
        29, 53, 37, 58, 29, 58, 37, 58, 29, 58, 37, 56, 24, 56, 24, 56, 24, 58,
        24, 58, 24, 58, 24, 56, 24, 56, 24, 55, 32, 55, 42, 55, 32, 55, 42, 56,
        32, 56, 42, 56, 32, 56, 0, 60, 32, 60, 42, 60, 32, 60, 42, 58, 32, 58,
        42, 58, 32, 58, 42, 56, 25, 56, 25, 56, 25, 56, 25, 58, 25, 58, 25, 58,
        25, 58, 25, 56, 32, 56, 41, 56, 32, 56, 41, 55, 32, 55, 41, 55, 32, 55,
        41, 56, 0, 56, 0, 56, 0, 56, 0, 61, 0, 61, 0, 61, 0, 61, 0, 60, 30, 60,
        30, 60, 30, 60, 30, 58, 30, 58, 30, 58, 30, 58, 32, 56, 34, 56, 39, 56,
        34, 56, 39, 54, 34, 54, 39, 54, 34, 54, 39, 53, 0, 53, 0, 53, 0, 53, 0,
        51, 0, 51, 0, 51, 0, 51, 0, 49, 20, 49, 20, 49, 20, 49, 20, 48, 20, 48,
        20, 48, 20, 48, 20, 46, 29, 46, 37, 46, 29, 46, 37, 44, 29, 44, 37, 44,
        29, 44, 37, 42, 0, 42, 0, 42, 0, 42, 0, 41, 0, 41, 0, 41, 0, 41, 0, 39,
        20, 39, 20, 39, 20, 39, 20, 37, 20, 37, 20, 37, 20, 37, 20, 36, 30, 36,
        32, 36, 30, 36, 32, 39, 30, 39, 32, 39, 30, 39, 32, 46, 30, 46, 36, 46,
        30, 46, 36, 44, 30, 44, 36, 44, 30, 44, 36, 43, 25, 43, 25, 43, 25, 43,
        25, 44, 25, 44, 25, 44, 25, 44, 25, 46, 32, 46, 41, 46, 32, 46, 41, 48,
        32, 48, 41, 48, 32, 48, 41, 49, 0, 49, 0, 49, 0, 49, 0, 51, 0, 51, 0,
        51, 0, 51, 0, 53, 33, 53, 33, 53, 33, 54, 33, 54, 33, 54, 33, 53, 33,
        53, 33, 52, 39, 52, 41, 52, 39, 52, 41, 53, 39, 53, 41, 53, 39, 53, 0,
        56, 39, 56, 41, 56, 39, 56, 41, 54, 39, 54, 41, 54, 39, 54, 41, 53, 34,
        53, 34, 53, 34, 53, 34, 54, 34, 54, 34, 54, 34, 54, 34, 53, 37, 53, 41,
        53, 37, 53, 41, 52, 37, 52, 41, 52, 37, 52, 0, 53, 37, 53, 41, 53, 37,
        53, 41, 58, 37, 58, 41, 58, 37, 58, 41, 56, 36, 56, 36, 56, 36, 58, 36,
        58, 36, 58, 36, 56, 36, 56, 36, 55, 42, 55, 44, 55, 42, 55, 44, 56, 42,
        56, 44, 56, 42, 56, 0, 60, 42, 60, 44, 60, 42, 60, 44, 58, 42, 58, 44,
        58, 42, 58, 44, 56, 37, 56, 37, 56, 37, 56, 37, 58, 37, 58, 37, 58, 37,
        58, 37, 56, 41, 56, 44, 56, 41, 56, 44, 55, 41, 55, 44, 55, 41, 55, 44,
        56, 0, 56, 0, 56, 0, 56, 0, 65, 0, 65, 0, 65, 0, 65, 0, 63, 30, 63, 30,
        63, 30, 63, 30, 61, 30, 61, 30, 61, 30, 61, 30, 60, 37, 60, 46, 60, 37,
        60, 46, 58, 37, 58, 46, 58, 37, 58, 46, 56, 0, 56, 0, 56, 0, 56, 0, 54,
        0, 54, 0, 54, 0, 54, 0, 53, 20, 53, 20, 53, 20, 53, 20, 51, 20, 51, 20,
        51, 20, 51, 20, 49, 29, 49, 37, 49, 29, 49, 37, 48, 29, 48, 37, 48, 29,
        48, 37, 46, 0, 46, 0, 46, 0, 46, 0, 44, 0, 44, 0, 44, 0, 44, 0, 45, 20,
        45, 20, 45, 20, 45, 20, 48, 20, 48, 20, 48, 20, 48, 20, 46, 30, 46, 32,
        46, 30, 46, 32, 41, 30, 41, 32, 41, 30, 41, 0, 42, 30, 42, 32, 42, 30,
        42, 32, 36, 30, 36, 32, 36, 30, 36, 32, 37, 25, 37, 25, 37, 25, 37, 25,
        37, 25, 37, 25, 37, 25, 37, 25, 0, 32, 0, 41, 0, 32, 0, 41, 0, 32, 0,
        41, 0, 32, 0, 41, 53, 0, 53, 0, 53, 0, 53, 0, 53, 0, 53, 0, 53, 0, 53,
        0, 53, 21, 53, 21, 53, 21, 54, 21, 54, 21, 54, 21, 53, 21, 53, 21, 52,
        29, 52, 39, 52, 29, 52, 39, 53, 29, 53, 39, 53, 29, 53, 0, 56, 29, 56,
        39, 56, 29, 56, 39, 54, 29, 54, 39, 54, 29, 54, 39, 53, 22, 53, 22, 53,
        22, 53, 22, 54, 22, 54, 22, 54, 22, 54, 22, 53, 29, 53, 37, 53, 29, 53,
        37, 52, 29, 52, 37, 52, 29, 52, 0, 53, 29, 53, 37, 53, 29, 53, 37, 58,
        29, 58, 37, 58, 29, 58, 37, 56, 24, 56, 24, 56, 24, 58, 24, 58, 24, 58,
        24, 56, 24, 56, 24, 55, 32, 55, 42, 55, 32, 55, 42, 56, 32, 56, 42, 56,
        32, 56, 0, 60, 32, 60, 42, 60, 32, 60, 42, 58, 32, 58, 42, 58, 32, 58,
        42, 56, 25, 56, 25, 56, 25, 56, 25, 58, 25, 58, 25, 58, 25, 58, 25, 56,
        32, 56, 41, 56, 32, 56, 41, 55, 32, 55, 41, 55, 32, 55, 41, 56, 0, 56,
        0, 56, 0, 56, 0, 61, 0, 61, 0, 61, 0, 61, 0, 60, 30, 60, 30, 60, 30, 60,
        30, 58, 30, 58, 30, 58, 30, 58, 30, 56, 34, 56, 39, 56, 34, 56, 39, 54,
        34, 54, 39, 54, 34, 54, 39, 53, 0, 53, 0, 53, 0, 53, 0, 51, 0, 51, 0,
        51, 0, 51, 0, 49, 20, 49, 20, 49, 20, 49, 20, 48, 20, 48, 20, 48, 20,
        48, 20, 46, 29, 46, 37, 46, 29, 46, 37, 44, 29, 44, 37, 44, 29, 44, 37,
        42, 0, 42, 0, 42, 0, 42, 0, 41, 0, 41, 0, 41, 0, 41, 0, 39, 20, 39, 20,
        39, 20, 39, 20, 37, 20, 37, 20, 37, 20, 37, 20, 36, 30, 36, 32, 36, 30,
        36, 32, 39, 30, 39, 32, 39, 30, 39, 32, 46, 30, 46, 36, 46, 30, 46, 36,
        44, 30, 44, 36, 44, 30, 44, 36, 43, 25, 43, 25, 43, 25, 43, 25, 44, 25,
        44, 25, 44, 25, 44, 25, 46, 32, 46, 41, 46, 32, 46, 41, 48, 32, 48, 41,
        48, 32, 48, 41, 49, 0, 49, 0, 49, 0, 49, 0, 51, 0, 51, 0, 51, 0, 51, 0,
        53, 12, 53, 12, 53, 12, 54, 12, 54, 12, 54, 12, 53, 12, 53, 12, 52, 18,
        52, 20, 52, 18, 52, 20, 53, 18, 53, 20, 53, 18, 53, 0, 56, 18, 56, 20,
        56, 18, 56, 20, 54, 18, 54, 20, 54, 18, 54, 20, 53, 13, 53, 13, 53, 13,
        53, 13, 54, 13, 54, 13, 54, 13, 54, 13, 53, 17, 53, 20, 53, 17, 53, 20,
        52, 17, 52, 20, 52, 17, 52, 0, 53, 17, 53, 20, 53, 17, 53, 20, 58, 17,
        58, 20, 58, 17, 58, 20, 56, 15, 56, 15, 56, 15, 58, 15, 58, 15, 58, 15,
        56, 15, 56, 15, 55, 22, 55, 24, 55, 22, 55, 24, 56, 22, 56, 24, 56, 22,
        56, 0, 60, 22, 60, 24, 60, 22, 60, 24, 58, 22, 58, 24, 58, 22, 58, 24,
        56, 37, 56, 37, 56, 37, 56, 37, 58, 37, 58, 37, 58, 37, 58, 37, 56, 41,
        56, 44, 56, 41, 56, 44, 55, 41, 55, 44, 55, 41, 55, 44, 56, 0, 56, 0,
        56, 0, 56, 0, 65, 0, 65, 0, 65, 0, 65, 0, 63, 18, 63, 18, 63, 18, 61,
        18, 61, 18, 61, 18, 60, 18, 60, 18, 58, 37, 58, 46, 58, 37, 56, 46, 56,
        37, 56, 46, 54, 37, 54, 46, 53, 0, 53, 0, 53, 0, 51, 0, 51, 0, 51, 0,
        61, 0, 61, 0, 60, 0, 60, 0, 60, 0, 58, 0, 58, 0, 58, 0, 56, 0, 56, 0,
        54, 0, 54, 0, 54, 0, 53, 0, 53, 0, 53, 0, 51, 0, 51, 0, 49, 0, 49, 0,
        49, 0, 48, 0, 48, 0, 48, 0, 46, 0, 46, 0, 45, 20, 45, 20, 45, 20, 45,
        20, 48, 20, 48, 20, 48, 20, 48, 20, 46, 30, 46, 32, 46, 30, 46, 32, 41,
        30, 41, 32, 41, 30, 41, 0, 42, 30, 42, 32, 42, 30, 42, 32, 36, 30, 36,
        32, 36, 30, 36, 32, 37, 13, 37, 13, 37, 13, 37, 13, 37, 13, 37, 13, 37,
        13, 37, 13, 37, 29, 37, 41, 37, 29, 37, 41, 37, 29, 37, 41, 37, 29, 37,
        41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/*********************************************************************
 * @fn      TIM1_OutCompare_Init
 *
 * @brief   Initializes TIM1 output compare.
 *
 * @param   arr - the period value.
 *          psc - the prescaler value.
 *          ccp - the pulse value.
 *
 * @return  none
 */
void TIM1_PWMOut_Init(u16 arr, u16 psc, u16 ccp) {
    TIM_OCInitTypeDef TIM_OCInitStructure = { 0 };
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = { 0 };

    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

#if (PWM_MODE == PWM_MODE1)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

#elif (PWM_MODE == PWM_MODE2)
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;

#endif

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init( TIM1, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig( TIM1, ENABLE);
    TIM_Cmd( TIM1, ENABLE);
}

void toneinit(void) {

    GPIO_InitTypeDef GPIO_InitStructure = { 0 };

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOD, &GPIO_InitStructure);

}

void tone(uint16_t freq) {

    uint16_t duration;

    duration = 1000000 / freq;   // in usec

    TIM1_PWMOut_Init(duration, 47, duration / 2);

}

void notone(void) {

    TIM_Cmd(TIM1, DISABLE);
}

void notone2(void) {

    TIM1_PWMOut_Init(65535, 47, 65535);

}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void) {

    Delay_Init();

    toneinit();
    Delay_Ms(1000);

    while(1) {

        for(int i=0; i<6720; i++) {

            if(notes[i]!=0) {
                tone(tonedata[notes[i]]);
            } else {
                notone2();
            }

            Delay_Ms(11);

        }

        notone();
        Delay_Ms(5000);

    }
}