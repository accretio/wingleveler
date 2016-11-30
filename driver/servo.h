#ifndef SERVO_H
#define SERVO_H

#include <pthread.h>
#include "state.h"

#define DONT_MOVE 0
#define MOVE_LEFT 1
#define MOVE_RIGHT 2
#define DISENGAGE -1

pthread_cond_t cond;
pthread_mutex_t mtx;

int start_servo(struct state_t *state);

#endif
