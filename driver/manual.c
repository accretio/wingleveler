// manual control of the weight

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <pthread.h>
#include "config.h"
#include "state.h"
#include "servo.h"

void sigterm_handler(int signo){
  endwin();
  exit(0);
}

int manual_loop(struct state_t *state)
{
  int c;
  int rc;

  initscr();
 
  sigset(SIGINT, sigterm_handler);

  if (rc = start_servo(state)) {
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
        state->action = MOVE_LEFT;
        pthread_cond_signal(&cond);
        break;
      case KEY_RIGHT:
        printw("\rmoving right from %d", state->current_step);
        state->action = MOVE_RIGHT;
        pthread_cond_signal(&cond);
        break;
      case ' ':
        state->action = DONT_MOVE;
        pthread_cond_signal(&cond);
        break ;
      case 'q':
        state->action = DISENGAGE;
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

