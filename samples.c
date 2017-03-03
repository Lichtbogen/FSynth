#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include "fsynth.h"

SampleBuffer *create_sample_buffer_raw(uint32_t sample_rate, size_t sample_count)
{
  SampleBuffer *p_buf;
  p_buf = (SampleBuffer*) malloc(sizeof(SampleBuffer));
  memset(p_buf, 0, sizeof(SampleBuffer));
  p_buf->sample_count = sample_count;
  p_buf->sample_rate = sample_rate;
  p_buf->buffer_size = sizeof(sample_t) * sample_count;
  p_buf->samples = (sample_t*) malloc(p_buf->buffer_size);
  memset(p_buf->samples, 0, p_buf->buffer_size);
  return p_buf;
}

SampleBuffer *create_sample_buffer(uint32_t sample_rate, double duration)
{
  uint32_t sample_count = (uint32_t)(sample_rate * duration);
  return create_sample_buffer_raw(sample_rate, sample_count);
}

double get_buffer_duration(SampleBuffer *buffer)
{
  assert(INVALID_BUFFER(buffer));
  return buffer->sample_count / buffer->sample_rate;
}

sample_t triangle(double phase)
{
  sample_t sample;
  double normalized = fmod(phase, M_PI * 2.) / (M_PI * 2.);
  if (normalized < .5) {
    sample = LERP(0, 2., normalized);
  } else {
    sample = LERP(2., 0, normalized);
  }
  //sample = normalized;
  return sample;
}

int generate_wave_func(SampleBuffer *buffer, int func_type, double freq, double amp)
{
  int result = FS_OK;
  double shift, phase = 0;
  if (INVALID_BUFFER(buffer) || freq == 0) {
    assert(0);
    return FS_ERROR;
  }
  shift = M_PI * 2. / ((double)buffer->sample_rate / freq);
  if (func_type == WAVE_SINE) {
    FOREACH_SAMPLE(buffer) {
      buffer->samples[idx] = (sample_t)sin(phase) * amp;
      phase += shift;
    }
  } else if (func_type == WAVE_COSINE) {
    FOREACH_SAMPLE(buffer) {
      buffer->samples[idx] = (sample_t)cos(phase) * amp;
      phase += shift;
    }
  } else if (func_type == WAVE_TRIANGLE) {
    FOREACH_SAMPLE(buffer) {
      buffer->samples[idx] = triangle(phase) * amp;
      phase += shift;
    }
  }
  else result = FS_ERROR;
  return result;
}

void delete_sample_buffer(SampleBuffer **buffer)
{
  if ((*buffer)->buffer_size > 0) {
    free((*buffer)->samples);
    free(*buffer);
    *buffer = NULL;
  }
}
