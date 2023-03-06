#ifndef LASERLINE_DEFS_H
#define LASERLINE_DEFS_H

#define FREQUENCY_5_HZ  0 //  5 hertz, i.e.:  5 cycle per second
#define FREQUENCY_10_HZ 1 // 10 hertz, i.e.: 10 cycle per second
#define FREQUENCY_15_HZ 2 // 15 hertz, i.e.: 15 cycle per second
#define FREQUENCY_20_HZ 3 // 20 hertz, i.e.: 20 cycle per second
#define FREQUENCY_25_HZ 4 // 25 hertz, i.e.: 25 cycle per second
#define FREQUENCY_30_HZ 5 // 30 hertz, i.e.: 10 cycle per second
#define FREQUENCY_35_HZ 6 // 35 hertz, i.e.: 15 cycle per second
#define FREQUENCY_40_HZ 7 // 40 hertz, i.e.: 20 cycle per second
#define FREQUENCY_45_HZ 8 // 45 hertz, i.e.: 25 cycle per second
#define FREQUENCY_MAX FREQUENCY_45_HZ+1       // sentinel and array def

#define FREQUENCY_FULL_HZ 100
#define DUTYCYCLE_TOT 10

#define DUTYCYCLE_50 0   // 50% on, 50% off
#define DUTYCYCLE_40 1   // 40% on, 60% off
#define DUTYCYCLE_30 2   // 30% on, 70% off
#define DUTYCYCLE_20 3   // 20% on, 80% off
#define DUTYCYCLE_10 4   // 10% on, 90% off
#define DUTYCYCLE_MAX DUTYCYCLE_10+1

enum LaserStates {
  CYCLE_ON_START,
  CYCLE_ON_WAIT,
  CYCLE_ON_END,
  CYCLE_OFF_START,
  CYCLE_OFF_WAIT,
  CYCLE_OFF_END,
  
  COMMAND_ON,
  COMMAND_WAIT,
  COMMAND_OFF,
  COMMAND_FREQUENCY,
  COMMAND_DUTYCYCLE,

  MOMENTARY_ON,
  MOMENTARY_WAIT,
  MOMENTARY_OFF,
};

typedef struct {
  byte cycleON;
  byte cycleOFF;
} DutyCycle;

typedef struct {
  DutyCycle DutyFreq[ 5 ];
}  DutyCyclesByFrequency;

//             5           4           3           2           1 
//           on  off     on  off     on  off     on  off     on  off
const DutyCycle DutiesByFreq[FREQUENCY_MAX][DUTYCYCLE_MAX] =
{
/*  5 */{ {100, 100}, { 80, 120}, { 60, 140}, { 40, 160}, { 20, 180}, },
/* 10 */{ { 50,  50}, { 40,  60}, { 30,  70}, { 20,  80}, { 10,  90}, },
/* 15 */{ { 33,  33}, { 27,  40}, { 20,  47}, { 13,  53}, {  7,  60}, },
/* 20 */{ { 25,  25}, { 20,  30}, { 15,  35}, { 10,  40}, {  5,  45}, },
/* 25 */{ { 20,  20}, { 16,  24}, { 12,  28}, {  8,  32}, {  4,  36}, },
/* 30 */{ { 17,  17}, { 13,  20}, { 10,  23}, {  7,  27}, {  3,  30}, },
/* 35 */{ { 14,  14}, { 11,  17}, {  9,  20}, {  6,  23}, {  3,  26}, },
/* 40 */{ { 13 , 13}, { 10,  15}, {  8,  18}, {  5,  20}, {  3,  23}, },
/* 45 */{ { 11,  11}, {  9,  13}, {  7,  16}, {  4,  18}, {  2,  20}, },
};


#endif//LASERLINE_DEFS_H