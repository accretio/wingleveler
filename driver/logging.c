// logging of the state

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "state.h"

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

    fprintf(state->logger,"%d.%06d: bank: %f step:%d\n",
            tv.tv_sec, tv.tv_usec, 
            state->yaw,
            state->current_step);
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

  printf("starting logging to %s\n", path);
  state->logger = fopen(path, "a+");
  
  if (rc = pthread_create(&thread, NULL, logging_loop, (void *)state)) {
    return rc; 
  } 
  return 0;
}
