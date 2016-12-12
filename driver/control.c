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

  if (state->current_direction == NEMA_DIRECTION_RIGHT &&
      !gpioRead(GPIO_LIMIT_SWITCH_RIGHT)) {
    printf("going right, right switch on\n");
    return -1; 
  }
  
  if (state->current_direction == NEMA_DIRECTION_LEFT &&
      !gpioRead(GPIO_LIMIT_SWITCH_LEFT)) {
       printf("going left, left switch on\n");
       return -1; 
  }
    
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

int center(struct state_t *state)
{
  set_direction(state, NEMA_DIRECTION_LEFT);
  // go all the way to the left
  while(!step(state)) {
   
  };
  // reset the current position
  printf("left stop found\n");
  state->current_step = 0;
  // go all the way to the right
  set_direction(state, NEMA_DIRECTION_RIGHT);
  while(!step(state)) {};
  state->max_step = state->current_step;
  printf("right stop found at %d\n", state->max_step);
  // back in the middle
  set_direction(state, NEMA_DIRECTION_LEFT);
  while(state->current_step > state->max_step / 2) {
    step(state);
  }
  printf("centered\n");
  return 0;
}
