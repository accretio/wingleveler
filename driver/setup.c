#include <stdio.h>
#include <pigpio.h>

#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "config.h"
#include "smoother.h"
#include "state.h"
#include "servo.h"

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

int setup_state(struct state_t *state)
{
  state->compass_offset[VEC_X] = 450;
  state->compass_offset[VEC_Y] = 450;  
  state->compass_range[VEC_X] = 640;
  state->compass_range[VEC_Y] = 640;
  state->action = DONT_MOVE;
  state->bank_delta = 0.0;
  state->bank_reference = 0.0;
  state->max_step = MAX_STEP;
  state->nema_pause = NEMA_17_STEP_PAUSE;
  state->check_limits = 1;

  smoother_init(&(state->bank_smoother));
  return 0; 
}

int setup(struct state_t *state)
{
  int ret ;
  if (ret = setup_state(state)) {
    return ret; 
  }
  if (ret = setup_gpio(state)) {
    return ret; 
  }
  if (ret = setup_mpu(state)) {
    return ret; 
  }  
  return ret; 
}


void cleanup_state(struct state_t *state)
{
  gpioTerminate();
  printf("everything was cleaned up\n");
}
