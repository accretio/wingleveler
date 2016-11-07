#ifndef WINGLEVELER_CONFIG_H
#define WINGLEVELER_CONFIG_H

// GPIO Configuration

#define GPIO_NEMA_DIRECTION_PIN 23
#define GPIO_NEMA_STEP_PIN 24

// NEMA BEHAVIOR

#define NEMA_DIRECTION_LEFT 0
#define NEMA_DIRECTION_RIGHT 1
#define NEMA_17_STEP_PAUSE 4000

// IMU 9250

#define IMU_IC2_BUS 1

// Some helpers

#define VEC_X 0
#define VEC_Y 1

// Math 101

#define TWO_PI				(2.0f * (float)M_PI)
#define	RAD_TO_DEGREE		(180.0f / (float)M_PI)

// Wing leveler configuration

#define MAX_STEP 4000

#define DRIFT_THRESHOLD 1
#define DRIFT_MAX 30

#define MONITORING_DELAY 200
#define MAGNIFICATION_FACTOR 10

#define SAMPLE_RATE 10
#endif
