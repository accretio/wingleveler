// a simple smoothing algorithm
// the problem is that it causes the signal to be delayed
// and we are already running behind somehow.

#define RING_BUFFER_SIZE 20


#include <stdio.h>
#include <math.h>
#include <pthread.h>


struct measurement_t {
  float timestamp;
  float value;
};
  
struct smoother_t
{
  struct measurement_t ring_buffer[RING_BUFFER_SIZE];
  int pos;
  pthread_mutex_t mtx;
  int ready;
};

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

void smoother_average(struct smoother_t *smoother, struct measurement_t *measurement, int pos)
{
  float sum_values = 0.0 ;
  float min_timestamp = INFINITY;
  float max_timestamp = 0.0;
  
  for (int p = 0; p < RING_BUFFER_SIZE/2; p++) {
    struct measurement_t m = smoother->ring_buffer[(pos + p + RING_BUFFER_SIZE) % RING_BUFFER_SIZE];
    sum_values += m.value ;
    min_timestamp = fmin(min_timestamp, m.timestamp);
    max_timestamp = fmax(max_timestamp, m.timestamp);
  }

  measurement->value = sum_values / (RING_BUFFER_SIZE/2);
  measurement->timestamp = (max_timestamp + min_timestamp) / 2.0;
  return;

}

float smoother_derivative(struct smoother_t *smoother)
{
  pthread_mutex_lock(&(smoother->mtx));

  struct measurement_t m1, m2;

  smoother_average(smoother, &m1, smoother->pos);
  smoother_average(smoother, &m2, smoother->pos + RING_BUFFER_SIZE/2); 
  
  pthread_mutex_unlock(&(smoother->mtx));

  printf("m1: %f %f, m2: %f, %f\n",
         m1.value, m1.timestamp, m2.value, m2.timestamp);

  return (m2.value - m1.value) / (m2.timestamp - m1.timestamp);
}

int main() {
  int rc; 
  printf("testing the smoother\n");
  struct smoother_t smoother;
  if (rc = smoother_init(&smoother)) {
    printf("couldn't init the smooother\n");
    return rc; 
  }

  for (int i = 0; i < 50; i++) {
    smoother_push(&smoother, (i * 1.0), (i % 2 - 1) * 0.20);
    if (smoother.ready > 0) {
      float d = smoother_derivative(&smoother);
      printf("derivative is %f\n", d);
    }
  }


  return 0 ;
}

  
