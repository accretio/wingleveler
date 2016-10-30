#include<stdio.h>
#include <math.h>
#include <pigpio.h>

#include "inv_mpu.h"
#include "linux_glue.h"
#include "config.h"

struct state_t {
      short current_direction;

      // Compass calibration
      short compass_fsr; 
      short compass_offset[2];
      short compass_range[2];

      // Compass data
      short compass_data[3];
      float yaw; 

};


int setup_gpio(struct state_t *state) {
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
      if (gpioWrite(GPIO_NEMA_STEP_PIN, NEMA_DIRECTION_LEFT)) {
            printf("couldn't set up the direction to the left\n");
            return -1;
      }
      state->current_direction = NEMA_DIRECTION_LEFT;
      return 0 ;
}

int setup_mag(struct state_t *state) {
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


int step(struct state_t *state) {
      if (gpioWrite(GPIO_NEMA_STEP_PIN, 1)) {
            printf("couldn't write to the step pin\n");
            return -1;
      }
      if (gpioWrite(GPIO_NEMA_STEP_PIN, 0)) {
            printf("couldn't write to the step pin\n");
            return -1;
      }
      gpioSleep(PI_TIME_RELATIVE, 0, NEMA_17_STEP_PAUSE); // sleep for 0.1 seconds
      return 0; 
}

int setup(struct state_t *state) {
      int ret ;
      if (ret = setup_gpio(state)) {
            return ret; 
      }
      ret = setup_mag(state);
      return ret; 
}


void cleanup(struct state_t *state) {
      gpioTerminate();
      printf("everything was cleaned up\n");
}


int refresh_yaw(struct state_t *state) {
      if (mpu_get_compass_reg(state->compass_data, NULL)) {
            printf("couldn't get compass data\n");
            return -1;
      }

      state->yaw = -atan2f(
      (-(state->compass_data[VEC_X] - state->compass_offset[VEC_X]) * state->compass_fsr / state->compass_range[VEC_X]),
            (state->compass_data[VEC_Y] - state->compass_offset[VEC_Y]) * state->compass_fsr / state->compass_range[VEC_Y]);
            
      
      if (state->yaw < 0.0f)
            state->yaw = TWO_PI + state->yaw;
      
      state->yaw = state->yaw * 360.0f / TWO_PI; 
      
      printf("yaw is %f\n", state->yaw);
      //       linux_delay_ms(100);
      
}


     
int main() {

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
      
 finalize: 
      cleanup(&state);
      
      return 0;
      
}

