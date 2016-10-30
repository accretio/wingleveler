#include<stdio.h>

#include "MPU9250/MPU9250.h"

const int i2c_bus = 1;

int main() {
      printf("Accretio Wing Leveler\n");

      mpu9250_init();

      mpu9250_configure_i2c();

      printf("Fully configured");

      return 0;
   

}
