#ifndef Servos_H
#define Servos_H

#include "NewServo.h"

#define LFT_PIN   D5
#define RGT_PIN   D6

#define LFT 0
#define RGT 1

newServo servos[NUMSERVOS];

void InitServos() {

  for(byte id = 0; id < NUMSERVOS; id++) {
    if((config.limits[id].zero > config.limits[id].max) || (config.limits[id].zero < config.limits[id].min))
      config.limits[id].zero = (config.limits[id].min + config.limits[id].max) /2;
      
    servos[id].limits = config.limits[id];
  }

  // Set up the servos for the car
  servos[LFT].pin = LFT_PIN; servos[LFT].rateLimit = 0;
  servos[RGT].pin = RGT_PIN; servos[RGT].rateLimit = 0;
}


#endif
