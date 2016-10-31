#include <stdio.h>
#include <sys/select.h>
#include <time.h>
#include <math.h>
#include <pigpio.h>

#include "inv_mpu.h"
#include "linux_glue.h"
#include "config.h"

struct state_t
{
      short current_direction;
      int current_step ;
      int stepping_rate ; // this is the number of steps we can do per monitoring period
      
      // Compass calibration
      short compass_fsr; 
      short compass_offset[2];
      short compass_range[2];

      // Compass data
      short compass_data[3];
      float yaw;

      // Compass reference
      float target_yaw;

     

};

int setup_gpio(struct state_t *state)
{
      if (gpioInitialise() < 0) {
            printf("couldn't initialize the gpio lib\n");
            return -1;
      }
      if (gpioGetMode(GPIO_NEMA_DIRECTION_PIN) != PI_OUTPUT) {
            if (gpioSetMode(GPIO_NEMA_DIRECTION_PIN, PI_OUTPUT)) {
                  printf("couldn't set up the direction pin mode\n");
                  return -1;
            }
      }
      if (gpioGetMode(GPIO_NEMA_STEP_PIN) != PI_OUTPUT) {
            if (gpioSetMode(GPIO_NEMA_STEP_PIN, PI_OUTPUT)) {
                  printf("couldn't set up the step pin mode\n");
                  return -1;
            }
      }
      // initially go to the left
      if (gpioWrite(GPIO_NEMA_DIRECTION_PIN, NEMA_DIRECTION_LEFT)) {
            printf("couldn't set up the direction to the left\n");
            return -1;
      }
      state->current_direction = NEMA_DIRECTION_LEFT;
      state->current_step = 0; // bottom dead center
      state->stepping_rate = MONITORING_DELAY * 1000 / NEMA_17_STEP_PAUSE;

      return 0 ;
}

int set_direction(struct state_t *state, int direction) {
      if (state->current_direction != direction) {
            if (gpioWrite(GPIO_NEMA_DIRECTION_PIN, direction)) {
                  printf("couldn't set up the direction\n");
                  return -1;
            }
            state->current_direction = direction ;
      }
      return 0;
}

int setup_mag(struct state_t *state)
{
      linux_set_i2c_bus(IMU_IC2_BUS);
      if (mpu_init(NULL)) {
            printf("mpu_init() failed\n");
            return -1;
      }    
      if (mpu_get_compass_fsr(&(state->compass_fsr))) {
            printf("couldn't get compass fsr\n");
      }
      printf("compass fsr is %hu\n", state->compass_fsr);
      if (mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS)) {
            printf("mpu_set_sensors() failed\n");
            return -1;
      }
      if (mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS)) {
            printf("mpu_configure_fifo() failed\n");
            return -1;
      }
      if (mpu_set_sample_rate(10)) {
            printf("mpu_set_sample_rate() failed\n");
            return -1;
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
      gpioSleep(PI_TIME_RELATIVE, 0, NEMA_17_STEP_PAUSE);
      return 0; 
}

int setup(struct state_t *state)
{
      int ret ;
      if (ret = setup_gpio(state)) {
            return ret; 
      }
      ret = setup_mag(state);
      return ret; 
}

void cleanup(struct state_t *state)
{
      gpioTerminate();
      printf("everything was cleaned up\n");
}

int refresh_yaw(struct state_t *state)
{
      if (mpu_get_compass_reg(state->compass_data, NULL)) {
            printf("couldn't get compass data\n");
            return -1;
      }

      float calibrated_x = (state->compass_data[VEC_X] - state->compass_offset[VEC_X]) * state->compass_fsr / state->compass_range[VEC_X];

      float calibrated_y = (state->compass_data[VEC_Y] - state->compass_offset[VEC_Y]) * state->compass_fsr / state->compass_range[VEC_Y];
      
      state->yaw = -atan2f(-calibrated_x, calibrated_y);
            
      if (state->yaw < 0.0f)
            state->yaw = TWO_PI + state->yaw;
      
      state->yaw = state->yaw * 360.0f / TWO_PI; 
      
      printf("yaw is %f\n", state->yaw);

      return 0;
}

int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    #define STDIN_FILENO 0
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

void timespec_diff(struct timespec *start, struct timespec *stop,
                   struct timespec *result)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }

    return;
}

int loop(struct state_t *state)
{
      
      while(!kbhit()) {

            // no point in refreshing the mag more than ever 135ms
            struct timespec start, end_of_cycle, elapsed;
            clock_gettime(CLOCK_REALTIME, &start);
           
            if (refresh_yaw(state)) {
                  printf("couldn't refresh yaw");
                  return -1;
            }

            float drift = state->target_yaw - state->yaw;
                           
            if (fabs(drift) > DRIFT_THRESHOLD) {
                  // here we need to decide where to move. 
                  // we have a function drift -> step
                  int target_step = fmin(drift * MAGNIFICATION_FACTOR, MAX_STEP);
                  printf("current drift is %f, current step is %d, targetting %d\n",
                         drift,
                         state->current_step,
                         target_step);
           
                  printf("targetting step %d\n", target_step);

                  if (target_step < state->current_step) {
                        set_direction(state, NEMA_DIRECTION_LEFT);
                  } else {
                        set_direction(state, NEMA_DIRECTION_RIGHT);
                  }

                  int delta = abs(state->current_step - target_step);

                  // let's move as much as we can during monitoring delay, then poll again

                  int steps = min(min(delta, state->stepping_rate), MAX_STEP - abs(state->current_step)); 

                  printf("going to run %d steps\n", steps);
                  for (steps > 0; steps--;) {
                        printf("stepping the motor, current step is %d\n", state->current_step);
                        if (step(state)) {
                              printf("couldn't step the motor\n");
                              return -1; 
                        };
                  }
            } else {
                  printf("current drift is %f, current step is %d, all good\n",
                         drift,
                         state->current_step);
            }

            printf("all done\n");
            clock_gettime(CLOCK_REALTIME, &end_of_cycle);
            
            timespec_diff(&start, &end_of_cycle, &elapsed);

            long wait_time = (MONITORING_DELAY - (elapsed.tv_nsec / 1000000l));

            printf("elapsed: %l, waiting for %u\n", elapsed.tv_nsec, wait_time); 
            if (wait_time > 0) {
                  linux_delay_ms(wait_time);
            }     
            
      }

      return 0;

}
int main()
{

      printf("testing the GPIO\n");

      struct state_t state ; 
      
      // todo: get this information from a file on the disk
      state.compass_offset[VEC_X] = 450;
      state.compass_offset[VEC_Y] = 450;
      
      state.compass_range[VEC_X] = 640;
      state.compass_range[VEC_Y] = 640;
      
      if (setup(&state)) {
            printf("couldn't set up properly, exiting\n");
            goto finalize; 
      }
      
      printf("all ready!\n");
      
      refresh_yaw(&state);

      // todo: maybe wait a few seconds here, or average the values, or something?
      // in the real product this assignement would be triggered by pushing a button
      // do we don't need to account for warm up time.
      state.target_yaw = state.yaw ;

      printf("our target yaw is %f\n", state.target_yaw);
      
      loop(&state);
     
 finalize:
      
      cleanup(&state);
      
      return 0;
      
}

