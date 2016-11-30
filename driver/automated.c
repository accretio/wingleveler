// automated control of the weight

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <math.h>

#include "utils.h"
#include "config.h"
#include "state.h"
#include "control.h"
#include "servo.h"
#include "ahrs.h"
#include "linux_glue.h"

int automated_loop(struct state_t *state)
{

  while(!kbhit()) {
    refresh_ahrs(state);
    printf("current bank: %f, current bank rate: %f\n",
           state->bank,
           state->bank_delta);

   
    if (fabs(state->bank) < BANK_TOLERANCE) {
      state->action = DONT_MOVE;
    } else {
      // let's target a bank_delta of -state->bank
      float target = - BANK_CORRECTION_FACTOR * state->bank;
      if (state->bank < 0) {
        if (state->bank_delta < target) {
          state->action = MOVE_RIGHT; 
        } else {
          state->action = MOVE_LEFT;
        }
      } else {
        if (state->bank_delta < target) {
          state->action = MOVE_LEFT; 
        } else {
          state->action = MOVE_RIGHT;
        }
      }
      
      pthread_cond_signal(&cond);
    }
    
    linux_delay_ms(135); 
  }

  return 0;
}