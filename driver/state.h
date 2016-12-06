#ifndef STATE_H
#define STATE_H

#include <stdio.h>
#include <pthread.h>

struct state_t
{
  short current_direction;
  int current_step;
  int stepping_rate; // this is the number of steps we can do per monitoring period
  int max_step;
  int nema_pause;
      
  // Compass calibration
  short compass_fsr; 
  short compass_offset[2];
  short compass_range[2];

  // Compass data
  short compass_data[3];
  float yaw;

  // Compass reference
  float target_yaw;

  // dmp data:
  short rawGyro[3];
  short rawAccel[3];
  long rawQuat[4];
  unsigned long dmpTimestamp;

  float bank;
  float bank_reference;
  float bank_delta;
  unsigned long bankTimestamp; 
  
  // logging
  FILE *logger;

  // servo
  int action;
  pthread_t servo_thread;
  int check_limits;

};

#endif
