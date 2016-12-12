#ifndef SMOOTHER_H
#define SMOOTHER_H

#define RING_BUFFER_SIZE 20

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

int smoother_init(struct smoother_t *smoother);
void smoother_push(struct smoother_t *smoother, float timestamp, float value);
float smoother_average_value(struct smoother_t *smoother);
float smoother_derivative(struct smoother_t *smoother);
void smoother_display(struct smoother_t *smoother);

#endif
