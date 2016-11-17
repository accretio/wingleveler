// manual control of the weight + logging

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <pthread.h>
#include "config.h"
#include "state.h"
#include "control.h"

void sigterm_handler(int signo){
  endwin();
  exit(0);
}

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void *execute(struct state_t *state)
{
  while(1) {
    switch(state->action) {
    case 0:
      printf("waiting for action to change\n");
      pthread_cond_wait(&cond, &mtx);
      pthread_mutex_unlock(&mtx);
      break; 
    case 1:
      set_direction(state, NEMA_DIRECTION_LEFT);
      step(state);
      break;
    case 2:
      set_direction(state, NEMA_DIRECTION_RIGHT);
      step(state);
      break;
    case -1:
      return;
    default:
      break;
    }
  }
}

int manual_loop(struct state_t *state)
{
  int c;
  int rc;
  pthread_t thread;

  initscr();
 
  sigset(SIGINT, sigterm_handler);

  if (rc = pthread_create(&thread, NULL, execute, (void *)state)) {
    return rc; 
  }
  
  noecho();
  cbreak(); 
  
  keypad(stdscr, TRUE);
  printf("starting the manual loop\n");
  
  while(1) {
    c = getch();
   
    switch(c)
      {
      case KEY_UP:
        state->nema_pause -= NEMA_17_STEP_PAUSE_INCREMENT;
        printw("\rpause is now %d", state->nema_pause);
        break;
      case KEY_DOWN:
        state->nema_pause += NEMA_17_STEP_PAUSE_INCREMENT;
        printw("\rpause is now %d", state->nema_pause);
        break;
      case KEY_LEFT:
        printw("\rmoving left from %d", state->current_step);
        state->action = 1;
        pthread_cond_signal(&cond);
        break;
      case KEY_RIGHT:
        printw("\rmoving right from %d", state->current_step);
        state->action = 2;
        pthread_cond_signal(&cond);
        break;
      case ' ':
        state->action = 0 ;
        break ;
      case 'q':
        goto finalize;
      default:
        break; 
      }
     refresh();  
  }
  
 finalize:
  endwin();
  return 0;
}

