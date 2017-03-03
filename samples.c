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
  return sample;
}

sample_t saw(double phase)
{
  sample_t sample;
  double normalized = fmod(phase, M_PI * 2.) / (M_PI * 2.);
  sample = LERP(0, 1., normalized);
  return sample;
}

sample_t rect(double phase)
{
  sample_t sample;
  double normalized = fmod(phase, M_PI * 2.) / (M_PI * 2.);
  if (normalized < .5) {
    sample = 0;
  } else {
    sample = 1;
  }
  return sample;
}

int normalize_buffer(SampleBuffer *buffer)
{
  sample_t x, min_val = 1e300, max_val = 0;
  int idx, result = FS_OK;
  if (INVALID_BUFFER(buffer)) {
    assert(0);
    return FS_ERROR;
  }
  FOREACH_SAMPLE(buffer, idx) {
    min_val = MIN(min_val, buffer->samples[idx]);
    max_val = MAX(max_val, buffer->samples[idx]);
  }
  assert(max_val - min_val);
  FOREACH_SAMPLE(buffer, idx) {
    x = buffer->samples[idx];
    buffer->samples[idx] = (x - min_val) / (max_val - min_val);
  }
  return result;
}

int modulate_buffer(SampleBuffer *dest, SampleBuffer *src, int modulate_type)
{
  int idx, result = FS_OK;
  if (INVALID_BUFFER(dest) || INVALID_BUFFER(src) || dest->sample_count < src->sample_count) {
    assert(0);
    return FS_ERROR;
  }
  FOREACH_SAMPLE(dest, idx) {
    switch (modulate_type) {
      case MOD_ADD:
        dest->samples[idx] += src->samples[idx];
      break;
      case MOD_SUB:
        dest->samples[idx] -= src->samples[idx];
      break;
      case MOD_MULT:
        dest->samples[idx] *= src->samples[idx];
      break;
      case MOD_DIV:
        dest->samples[idx] /= src->samples[idx];
      break;
      default:
        result = FS_ERROR;
      break;
    }
  }
  return result;
}

int generate_wave_func(SampleBuffer *buffer, int func_type, double freq, double amp)
{
  int idx, result = FS_OK;
  double shift, phase = 0;
  if (INVALID_BUFFER(buffer) || freq == 0) {
    assert(0);
    return FS_ERROR;
  }
  shift = M_PI * 2. / ((double)buffer->sample_rate / freq);
  if (func_type == WAVE_SINE) {
    FOREACH_SAMPLE(buffer, idx) {
      buffer->samples[idx] = (sample_t)sin(phase) * amp;
      phase += shift;
    }
  } else if (func_type == WAVE_COSINE) {
    FOREACH_SAMPLE(buffer, idx) {
      buffer->samples[idx] = (sample_t)cos(phase) * amp;
      phase += shift;
    }
  } else if (func_type == WAVE_TRIANGLE) {
    FOREACH_SAMPLE(buffer, idx) {
      buffer->samples[idx] = triangle(phase) * amp;
      phase += shift;
    }
  } else if (func_type == WAVE_SAW) {
    FOREACH_SAMPLE(buffer, idx) {
      buffer->samples[idx] = saw(phase) * amp;
      phase += shift;
    }
  } else if (func_type == WAVE_RECT) {
    FOREACH_SAMPLE(buffer, idx) {
      buffer->samples[idx] = rect(phase) * amp;
      phase += shift;
    }
  } else result = FS_ERROR;
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
