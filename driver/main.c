#include <stdio.h>
#include <sys/select.h>
#include <time.h>
#include <math.h>
#include <pigpio.h>
#include <pthread.h>
#include <unistd.h>

#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "quaternion.h"
#include "linux_glue.h"
#include "config.h"
#include "state.h"
#include "control.h"
#include "manual.h"

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
  if (gpioWrite(GPIO_NEMA_DIRECTION_PIN, NEMA_DIRECTION_RIGHT)) {
    printf("couldn't set up the direction to the left\n");
    return -1;
  }
  state->current_direction = NEMA_DIRECTION_RIGHT;
  state->current_step = 0; // left 
  state->stepping_rate = MONITORING_DELAY * 1000 / NEMA_17_STEP_PAUSE;

  return 0 ;
}


int setup_mpu(struct state_t *state)
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
  if (mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL)) {
    printf("mpu_configure_fifo() failed\n");
    return -1;
  }
  if (mpu_set_sample_rate(SAMPLE_RATE)) {
    printf("mpu_set_sample_rate() failed\n");
    return -1;
  }
  if (mpu_set_compass_sample_rate(SAMPLE_RATE)) {
    printf("mpu_set_compass_sample_rate() failed\n");
    return -1;
  }
  if (dmp_load_motion_driver_firmware()) {
    printf("dmp_load_motion_driver_firmware() failed\n");
    //  return -1;
  }
  if (dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT)) {
    printf("dmp_enable_feature() failed\n");
    return -1;
  }
  if (dmp_set_fifo_rate(SAMPLE_RATE)) {
    printf("dmp_set_fifo_rate() failed\n");
    return -1;
  }
      
  if (mpu_set_dmp_state(1)) {
    printf("nmpu_set_dmp_state(1) failed\n");
    return -1;
  }
  return 0;
}


int setup(struct state_t *state)
{
  int ret ;
  if (ret = setup_gpio(state)) {
    return ret; 
  }
  if (ret = setup_mpu(state)) {
    return ret; 
  }  
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
      
  printf("yaw is %f - x:%hi, y:%hi, z:%hi\n", state->yaw, state->compass_data[0], state->compass_data[1], state->compass_data[2]);

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

      // not sure if we'll be able to do something that doesn't rely on the rate of change.

      int delta = (int)abs((drift * MAGNIFICATION_FACTOR));

      printf("current drift is %f, current step is %d, delta %d\n",
             drift,
             state->current_step,
             delta);

      int steps; 
      if (drift < 0) {
        set_direction(state, NEMA_DIRECTION_LEFT);
        steps = min(min(delta, state->stepping_rate), state->current_step); 

      } else {
        set_direction(state, NEMA_DIRECTION_RIGHT);
        steps = min(min(delta, state->stepping_rate), state->max_step - state->current_step); 
      }

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

int calibrate(struct state_t *state) {
  printf("hit any key when the weight reaches the end of the rail\n");
      
  while(!kbhit()) {
    step(state);
  }
  state->max_step = state->current_step;
  return;
}

int data_ready()
{
  short status;

  if (mpu_get_int_status(&status) < 0) {
    printf("mpu_get_int_status() failed\n");
    return 0;
  }

  return (status == 0x0103);
}

int read_dmp(struct state_t *state)
{
  short sensors;
  unsigned char more = 1;

  if (!data_ready()) {
    return -1;
  }

  while (more) {
    // Fell behind, reading again
    if (dmp_read_fifo(state->rawGyro, state->rawAccel, state->rawQuat, &state->dmpTimestamp, &sensors, &more) < 0) {
      printf("dmp_read_fifo() failed\n");
      return -1;
    }
  }

  return 0;
}

int update_bank(struct state_t *state)
{
  int ret = read_dmp(state);
  if (ret) {
    return ret;
  } else {
    printf("new mpu data: %ld %ld %ld %ld\n", state->rawQuat[0], state->rawQuat[1], state->rawQuat[2], state->rawQuat[3]);
            
    quaternion_t dmpQuat;
            
            
    dmpQuat[QUAT_W] = (float)state->rawQuat[QUAT_W];
    dmpQuat[QUAT_X] = (float)state->rawQuat[QUAT_X];
    dmpQuat[QUAT_Y] = (float)state->rawQuat[QUAT_Y];
    dmpQuat[QUAT_Z] = (float)state->rawQuat[QUAT_Z];
            
    quaternionNormalize(dmpQuat);
            
    float dmpEuler[3];
    quaternionToEuler(dmpQuat, dmpEuler);
            
    // printf("euler: %f %f %f\n", dmpEuler[0] * RAD_TO_DEGREE, dmpEuler[1] * RAD_TO_DEGREE, dmpEuler[2] * RAD_TO_DEGREE);
            
    state->bank = dmpEuler[0] * RAD_TO_DEGREE;
            
    return 0;
  }
}


int loop_dmp(struct state_t *state)
{
      
  while(!kbhit()) {

    // no point in refreshing the mag more than ever 135ms
    struct timespec start, end_of_cycle, elapsed;
    clock_gettime(CLOCK_REALTIME, &start);
           
    if (!update_bank(state)) {
                  
      float bank = state->bank - state->bank_reference;
                  
      if (fabs(bank) > DRIFT_THRESHOLD) {
        // here we need to decide where to move. 
        // we have a function drift -> step

        // not sure if we'll be able to do something that doesn't rely on the rate of change.

        int delta = (int)abs((bank * MAGNIFICATION_FACTOR));

        printf("current bank is %f, current step is %d, delta %d\n",
               bank,
               state->current_step,
               delta);

        int steps; 
        if (bank < 0) {
          set_direction(state, NEMA_DIRECTION_LEFT);
          steps = min(min(delta, state->stepping_rate), state->current_step); 

        } else {
          set_direction(state, NEMA_DIRECTION_RIGHT);
          steps = min(min(delta, state->stepping_rate), state->max_step - state->current_step); 
        }

        printf("going to run %d steps\n", steps);
        for (steps > 0; steps--;) {
          printf("stepping the motor, current step is %d\n", state->current_step);
          if (step(state)) {
            printf("couldn't step the motor\n");
            return -1; 
          };
        }
                  
      } else {
        printf("current bank is %f, current step is %d, all good\n",
               bank,
               state->current_step);
      }

      clock_gettime(CLOCK_REALTIME, &end_of_cycle);
                  
      timespec_diff(&start, &end_of_cycle, &elapsed);
                  
      long wait_time = (MONITORING_DELAY - (elapsed.tv_nsec / 1000000l));
                  
      printf("elapsed: %l, waiting for %u\n", elapsed.tv_nsec, wait_time); 
      if (wait_time > 0) {
        linux_delay_ms(wait_time);
      }     
            
    }
  }

  return 0;
      
}



int main(int argc, char **argv)
{

  printf("testing the GPIO\n");

  int opt;

  struct state_t state ; 
    
  // todo: get this information from a file on the disk
  state.compass_offset[VEC_X] = 450;
  state.compass_offset[VEC_Y] = 450;
      
  state.compass_range[VEC_X] = 640;
  state.compass_range[VEC_Y] = 640;

  state.action = 0;
  
  if (setup(&state)) {
    printf("couldn't set up properly, exiting\n");
    goto finalize; 
  }
   
  state.max_step = MAX_STEP;
  state.nema_pause = NEMA_17_STEP_PAUSE;
      
  while ((opt = getopt(argc, argv, "clra:m:")) != -1) {
    switch (opt) {
    case 'c':
      calibrate(&state);
      break;
    case 'l':
      set_direction(&state, NEMA_DIRECTION_LEFT);
      while(!kbhit()) {
        step(&state);
      }
      break;
    case 'r':
      set_direction(&state, NEMA_DIRECTION_RIGHT);
      while(!kbhit()) {
        step(&state);
      }
      break;
    case 'm':
      start_logging(&state, optarg);
      manual_loop(&state);
      goto finalize;
    case 'a':
      printf("wing leveler is ready (max_step: %d) !\n", state.max_step);
      start_logging(&state, optarg);
      linux_delay_ms(2000);
      while(update_bank(&state)) {
        printf("couldn't read initial bank\n");
        linux_delay_ms(2000);
      }
      state.bank_reference = state.bank; 
      loop_dmp(&state);
      goto finalize;
    default:
      break;
    }
  }
        
 
  
 finalize:

  // pthread_exit(NULL);
  // cleanup(&state);
         
  return 0;
      
}

