#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "config.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "quaternion.h"
#include "state.h"
#include "smoother.h"
#include "linux_glue.h"

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
      return 0; // (maybe -1??)
    }
  }

  return 0;
}

void refresh_ahrs(struct state_t *state)
{
  // only refresh bank for now
  
  int ret;
  quaternion_t dmpQuat;
  float dmpEuler[3];
  float bank;
  
  ret = read_dmp(state);
  if (ret) {
    printf("invalid dmp read..\n");
    // skip the beat
    return;
  } else {
             
    dmpQuat[QUAT_W] = (float)state->rawQuat[QUAT_W];
    dmpQuat[QUAT_X] = (float)state->rawQuat[QUAT_X];
    dmpQuat[QUAT_Y] = (float)state->rawQuat[QUAT_Y];
    dmpQuat[QUAT_Z] = (float)state->rawQuat[QUAT_Z];
    
    quaternionNormalize(dmpQuat);
    quaternionToEuler(dmpQuat, dmpEuler);
                
    bank = dmpEuler[0] * RAD_TO_DEGREE - state->bank_reference;

    state->bank_delta =
      (bank - state->bank) / (state->dmpTimestamp - state->bankTimestamp) * 1000.0 ; // bank delta in degree/s

    state->bankTimestamp = state->dmpTimestamp; 
    state->bank = bank;
    smoother_push(&(state->bank_smoother), state->dmpTimestamp, dmpEuler[0] * RAD_TO_DEGREE); 
    
    return;
  }
}

void *monitor(void *state)
{
  struct timespec time;
  while(1)
    {
      refresh_ahrs(state);
      linux_delay_ms(135); 
    }
}


int start_ahrs(struct state_t *state)
{
  int rc;
  pthread_t thread;

  if (rc = pthread_create(&thread, NULL, monitor, (void *)state)) {
    return rc; 
  }
  
  return 0;
}

/*



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

*/
