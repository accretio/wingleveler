// logging of the state

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "state.h"
#include "smoother.h"

int log_state(struct state_t *state)
{
  if (state->logger) {
    struct timeval tv;
    gettimeofday(&tv, NULL); 
 
    /*   
         time_t now;
         time(&now);
         char s[1000];
         struct tm * p = localtime(&now);
         strftime(s, 1000, "%s.", p);
    */

    
    /*  fprintf(state->logger,"%d.%06d: bank: %f, bank_delta: %f, step:%d",
            tv.tv_sec, tv.tv_usec, 
            state->bank,
            state->bank_delta,
            state->current_step);
    */
    if (state->bank_smoother.ready) {
      float bank_average;
      float bank_derivative;
      bank_average = smoother_average_value(&(state->bank_smoother));
      bank_derivative = 1000 * smoother_derivative(&(state->bank_smoother));
      fprintf(state->logger, "%d.%06d, %f, %f, %f, %f",  tv.tv_sec, tv.tv_usec, 
            state->bank,
              state->bank_delta, bank_average, bank_derivative);

     fprintf(state->logger, "\n");
    }
   
  }
}

void *logging_loop(void *state)
{
  struct timespec time;
  while(1)
    {
      log_state(state);
      time.tv_sec = 0;
      time.tv_nsec = 200000000L; // 200ms
      nanosleep(&time , NULL) ;  
    }
}

int start_logging(struct state_t *state, char *path)
{
  int rc;
  pthread_t thread;

  if (strcmp(path, "stdout")) {
    state->logger = fopen(path, "a+");
  } else {
    state->logger = stdout; 
  }
  if (rc = pthread_create(&thread, NULL, logging_loop, (void *)state)) {
    return rc; 
  } 
  return 0;
}

int start_logging_directly(struct state_t *state, char *path)
{
  if (strcmp(path, "stdout")) {
    state->logger = fopen(path, "a+");
  } else {
    state->logger = stdout; 
  }
  while(!kbhit()) {
    logging_loop(state);
  }
}
