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

int clear_sample_buffer(SampleBuffer *buffer)
{
  int result = FS_OK;
  if (INVALID_BUFFER(buffer)) {
    assert(0);
    result = FS_ERROR;
    return result;
  }
  memset(buffer->samples, 0, buffer->buffer_size);
  return result;
}

SampleBuffer *cat_sample_buffers(SampleBuffer *buffer_a, SampleBuffer *buffer_b)
{
  SampleBuffer *pout = NULL;
  if (!INVALID_BUFFER(buffer_a) && !INVALID_BUFFER(buffer_b)) {
    pout = create_sample_buffer_raw(buffer_a->sample_rate, buffer_a->sample_count + buffer_b->sample_count);
    memcpy(pout->samples, buffer_a->samples, buffer_a->buffer_size);
    memcpy(pout->samples + buffer_a->buffer_size, buffer_b->samples, buffer_b->buffer_size);
  }
  return pout;
}

SampleBuffer *repeat_sample_buffer(SampleBuffer *buffer, int times)
{
  int i;
  SampleBuffer *temp;
  SampleBuffer *pout = buffer;
  if (times > 0) {
    pout = cat_sample_buffers(buffer, buffer);
    for (i = 1; i < times; ++i) {
      temp = pout;
      pout = cat_sample_buffers(pout, buffer);
      delete_sample_buffer(&temp);
    }
  }
  return pout;
}

SampleBuffer *clone_sample_buffer(SampleBuffer *buffer)
{
  SampleBuffer *clone = create_sample_buffer_raw(buffer->sample_rate, buffer->sample_count);
  memcpy(clone->samples, buffer->samples, buffer->buffer_size);
  return clone;
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
  double normalized = phase / (M_PI * 2.);
  if (normalized < .5) {
    sample = LERP(0, 2, normalized);
  } else {
    sample = LERP(2, 0, normalized);
  }
  return (sample - .5) * 2.;
}

sample_t saw(double phase)
{
  sample_t sample;
  double normalized = phase / (M_PI * 2.);
  sample = LERP(-1, 1., normalized);
  return sample;
}

sample_t rect(double phase)
{
  sample_t sample;
  double normalized = phase / (M_PI * 2.);
  if (normalized < .5) {
    sample = -1;
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
    x = (x - min_val) / (max_val - min_val);
    x = (x - .5) * 2.;
    buffer->samples[idx] = x;
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

int wave_func_intern(sample_t *out, int func_type, double phase, double amp)
{
  sample_t result = 0;
  switch (func_type) {
  case WAVE_SINE:
    result = (sample_t)sin(phase) * amp;
    break;
  case WAVE_COSINE:
    result = (sample_t)cos(phase) * amp;
    break;
  case WAVE_TRIANGLE:
    result = triangle(phase) * amp;
    break;
  case WAVE_SAW:
    result = saw(phase) * amp;
    break;
  case WAVE_RECT:
    result = rect(phase) * amp;
    break;
  default:
    return FS_ERROR;
  }
  *out = result;
  return FS_OK;
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
  FOREACH_SAMPLE(buffer, idx) {
    if (wave_func_intern(&buffer->samples[idx], func_type, phase, amp) != FS_OK) {
      result = FS_ERROR;
      break;
    }
    phase = fmod(phase + shift, M_PI * 2.);
  }
  return result;
}

int modulate_frequency(SampleBuffer *dest, SampleBuffer *source, int func_type, double amp)
{
  int idx, result = FS_OK;
  double shift, phase = 0;
  if (INVALID_BUFFER(dest) || INVALID_BUFFER(source)) {
    assert(0);
    return FS_ERROR;
  }
  FOREACH_SAMPLE(dest, idx) {
    shift = M_PI * 2. / ((double)dest->sample_rate / source->samples[idx]);
    if (wave_func_intern(&dest->samples[idx], func_type, phase, amp) != FS_OK) {
      result = FS_ERROR;
      break;
    }
    phase = fmod(phase + shift, M_PI * 2.);
  }
  return result;
}

void delete_sample_buffer(SampleBuffer **buffer)
{
  if ((*buffer)->buffer_size > 0) {
    free((*buffer)->samples);
  }
  free(*buffer);
  *buffer = NULL;
}
