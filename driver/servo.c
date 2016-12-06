#include <stdio.h>
#include <stdlib.h>

#include "state.h"
#include "control.h"
#include "servo.h"
#include "config.h"

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void *execute(void *arg)
{
  struct state_t *state = arg; 
  while(1) {
    switch(state->action) {
    case DONT_MOVE:
      pthread_cond_wait(&cond, &mtx);
      pthread_mutex_unlock(&mtx);
      break; 
    case MOVE_LEFT:
      if (state->current_step > 0) {
        set_direction(state, NEMA_DIRECTION_LEFT);
        step(state);
      } else {
        // todo: replace that by a limit switch
        printf("reached LEFT stop\n");
      }
      break;
    case MOVE_RIGHT:
      if (state->current_step < MAX_STEP) {
        set_direction(state, NEMA_DIRECTION_RIGHT);
        step(state);
      } else {
        // todo: replace that by a limit switch
        printf("reached RIGHT stop\n");
      }
      break;
    case DISENGAGE:
      return;
    default:
      break;
    }
  }
}

int start_servo(struct state_t *state)
{
  int rc;
  rc = pthread_create(&(state->servo_thread), NULL, execute, (void *)state);
  return rc; 
}
