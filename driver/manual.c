// manual control of the weight + logging

#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include "config.h"
#include "state.h"
#include "control.h"

void sigterm_handler(int signo){
  endwin();
  exit(0);
}

int manual_loop(struct state_t *state)
{
  int c;
  initscr();
 
  sigset(SIGINT, sigterm_handler);

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
        set_direction(state, NEMA_DIRECTION_LEFT);
        printw("\rmoving left from %d", state->current_step);
        step(state);
        break;
      case KEY_RIGHT:
        set_direction(state, NEMA_DIRECTION_RIGHT);
        printw("\rmoving right from %d", state->current_step);
        step(state);
        break;
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

