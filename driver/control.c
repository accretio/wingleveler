#include <stdio.h>
#include <pigpio.h>
#include "config.h"
#include "state.h"

int set_direction(struct state_t *state, int direction) {
  if (state->current_direction != direction) {
    if (gpioWrite(GPIO_NEMA_DIRECTION_PIN, direction)) {
      printf("couldn't set up the direction\n");
      return -1;
    }
    state->current_direction = direction;
  }
  return 0;
}

int step(struct state_t *state)
{
      if (gpioWrite(GPIO_NEMA_STEP_PIN, 1)) {
            printf("couldn't write to the step pin\n");
            return -1;
      }
      if (gpioWrite(GPIO_NEMA_STEP_PIN, 0)) {
            printf("couldn't write to the step pin\n");
            return -1;
      }
      if (state->current_direction == NEMA_DIRECTION_RIGHT) { 
            state->current_step = state->current_step + 1 ;
      } else {
            state->current_step = state->current_step - 1 ;
      }
      gpioSleep(PI_TIME_RELATIVE, 0, state->nema_pause);
      return 0; 
}
