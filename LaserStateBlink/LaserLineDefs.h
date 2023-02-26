#ifndef LASERLINE_DEFS_H
#define LASERLINE_DEFS_H

#define FREQUENCY_5_HZ 5   //  5 hertz, i.e.:  5 cycle per second
#define FREQUENCY_10_HZ 10 // 10 hertz, i.e.: 10 cycle per second
#define FREQUENCY_15_HZ 15 // 15 hertz, i.e.: 15 cycle per second
#define FREQUENCY_20_HZ 20 // 20 hertz, i.e.: 20 cycle per second
#define FREQUENCY_25_HZ 25 // 20 hertz, i.e.: 25 cycle per second
#define FREQUENCY_FULL_HZ 100

#define DUTYCYCLE_TOT 10
#define DUTYCYCLE_50 5   // 50% on, 50% off
#define DUTYCYCLE_40 4   // 40% on, 60% off
#define DUTYCYCLE_30 3   // 30% on, 70% off
#define DUTYCYCLE_20 2   // 20% on, 80% off
#define DUTYCYCLE_10 1   // 10% on, 90% off

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

#endif//LASERLINE_DEFS_H