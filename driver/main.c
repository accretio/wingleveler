#include<stdio.h>
#include "linux_glue.h"

#include <math.h>


int main() {
      printf("Accretio Wing Leveler\n");

      linux_set_i2c_bus(1);
      
      if (mpu_init(NULL)) {
            printf("\nmpu_init() failed\n");
            return -1;
      }    

      unsigned short compass_sample_rate = 0; 

      if (mpu_get_compass_sample_rate(&compass_sample_rate)) {
            printf("couldn't get compass sample rate\n");
            return -1;
      }
      printf("compass sample rate is %hu\n", compass_sample_rate);
 
      unsigned short compass_fsr; 

      if (mpu_get_compass_fsr(&compass_fsr)) {
            printf("couldn't get compass fsr\n");
      }

      printf("compass fsr is %hu\n", compass_fsr);

      if (mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS)) {
            printf("\nmpu_set_sensors() failed\n");
            return -1;
      }

      if (mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS)) {
            printf("\nmpu_configure_fifo() failed\n");
            return -1;
      }

      if (mpu_set_sample_rate(10)) {
            printf("\nmpu_set_sample_rate() failed\n");
            return -1;
      }

      short compass_data[3] ;

      while(1) {
            if (mpu_get_compass_reg(compass_data, NULL)) {
                  printf("couldn't get compass data\n");
                  return -1;
            }

#define TWO_PI				(2.0f * (float)M_PI)

            float newMagYaw = atan2f(compass_data[0], compass_data[1]);

            if (newMagYaw != newMagYaw) {
                  printf("newMagYaw NAN\n");
                  return -1;
            }

            if (newMagYaw < 0.0f)
                  newMagYaw = TWO_PI + newMagYaw;

            newMagYaw = newMagYaw * 360 / TWO_PI; 

            printf("yaw is %f\n", newMagYaw);
            linux_delay_ms(100);
      }

    
}
