// a simple smoothing algorithm
// the problem is that it causes the signal to be delayed
// and we are already running behind somehow.

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "smoother.h"

int smoother_init(struct smoother_t *smoother) {
  int rc;
  smoother->pos = 0;
  smoother->ready = 0;
  rc = pthread_mutex_init(&(smoother->mtx), NULL);
  return rc; 
}

void smoother_push(struct smoother_t *smoother, float timestamp, float value)
{
  pthread_mutex_lock(&(smoother->mtx));
  if (smoother->pos == RING_BUFFER_SIZE) {
    printf("smoother is ready\n");
    smoother->ready = 1;
    smoother->pos = 0;
  }
  struct measurement_t *measurement = &(smoother->ring_buffer[smoother->pos]);
  measurement->timestamp = timestamp;
  measurement->value = value;
  smoother->pos += 1;
  pthread_mutex_unlock(&(smoother->mtx));
}

void smoother_average__(struct smoother_t *smoother, struct measurement_t *measurement, int pos, int len)
{
  float sum_values = 0.0 ;
  float min_timestamp = INFINITY;
  float max_timestamp = 0.0;
  int p; 
  for (p = 0; p < len; p++) {
    struct measurement_t m = smoother->ring_buffer[(pos + p + RING_BUFFER_SIZE) % RING_BUFFER_SIZE];
    sum_values += m.value ;
    min_timestamp = fmin(min_timestamp, m.timestamp);
    max_timestamp = fmax(max_timestamp, m.timestamp);
  }

  measurement->value = sum_values / len;
  measurement->timestamp = (max_timestamp + min_timestamp) / 2.0;
  return;

}

float smoother_average_value(struct smoother_t *smoother) {
  struct measurement_t m1;
  smoother_average__(smoother, &m1, 0, RING_BUFFER_SIZE);
  return (m1.value);
}

float smoother_derivative(struct smoother_t *smoother)
{
  pthread_mutex_lock(&(smoother->mtx));

  struct measurement_t m1, m2;

  smoother_average__(smoother, &m1, smoother->pos, RING_BUFFER_SIZE/2);
  smoother_average__(smoother, &m2, smoother->pos + RING_BUFFER_SIZE/2, RING_BUFFER_SIZE/2); 
  
  pthread_mutex_unlock(&(smoother->mtx));

  return (m2.value - m1.value) / (m2.timestamp - m1.timestamp);
}

void smoother_display(struct smoother_t *smoother) {
  int i;
  for (i=0; i < RING_BUFFER_SIZE; i++) {
    int p = (smoother->pos + i +  RING_BUFFER_SIZE) % RING_BUFFER_SIZE;
    struct measurement_t m = smoother->ring_buffer[p];
    printf("(%f, %f) ", m.timestamp, m.value); 
  }
  printf("\n");
  return; 
  
}


/*
int main() {
  int rc; 
  printf("testing the smoother\n");
  struct smoother_t smoother;
  if (rc = smoother_init(&smoother)) {
    printf("couldn't init the smooother\n");
    return rc; 
  }

  for (int i = 0; i < 50; i++) {
    smoother_push(&smoother, (i * 1.0), (i % 2 - 0.5) * 0.40);
    if (smoother.ready > 0) {
      float d = smoother_derivative(&smoother);
      printf("derivative is %f\n", d);
    }
  }


  float avg = smoother_average_value(&smoother);

  printf("average is %f\n", avg);
  smoother_display(&smoother);
  return 0 ;
}

  
*/
