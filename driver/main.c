#include <stdio.h>
#include <time.h>
#include <math.h>
#include <pigpio.h>
#include <pthread.h>
#include <unistd.h>

#include "utils.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "quaternion.h"
#include "linux_glue.h"
#include "config.h"
#include "utils.h"
#include "state.h"
#include "control.h"
#include "manual.h"
#include "setup.h"
#include "ahrs.h"

int calibrate(struct state_t *state) {
  printf("hit any key when the weight reaches the end of the rail\n");
      
  while(!kbhit()) {
    step(state);
  }
  state->max_step = state->current_step;
  printf("max step is %d\n", state->max_step); 
  return;
}


int main(int argc, char **argv)
{

  int opt;
  struct state_t state ; 
  
  if (setup(&state)) {
    printf("couldn't set up properly, exiting\n");
    goto finalize; 
  }
  
  while ((opt = getopt(argc, argv, "culrp:a:m:k:")) != -1) {
    switch (opt) {
    case 'p':
      start_ahrs(&state);
      start_logging_directly(&state, optarg);
      break;
    case 'c':
      calibrate(&state);
      break;
    case 'u':
      center(&state);
      break;
    case 'l':
      set_direction(&state, NEMA_DIRECTION_LEFT);
      while(!kbhit()) {
        step(&state);
      }
      printf("final position is %d\n", state.current_step);
      break;
    case 'r':
      set_direction(&state, NEMA_DIRECTION_RIGHT);
      while(!kbhit()) {
        step(&state);
      }
      printf("final position is %d\n", state.current_step);
      break;
    case 'm':
      center(&state);
      start_logging(&state, optarg);
      start_ahrs(&state); 
      manual_loop(&state);
      goto finalize;
    case 'a':
      center(&state);
      printf("wing leveler is ready (max_step: %d) !\n", state.max_step);
      start_logging(&state, optarg);
      automated_loop(&state);
      goto finalize;
    case 'k':
      printf("wing leveler is ready(centered) (max_step: %d) !\n", state.max_step);
      state.current_step = MAX_STEP/2 ;
      start_logging(&state, optarg);
      automated_loop(&state);
      goto finalize;
    default:
      break;
    }
  }
        
 
  
 finalize:

  cleanup_state(&state);
         
  return 0;
      
}

