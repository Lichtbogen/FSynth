#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <math.h>
#include "fsynth.h"

FSampleBuffer *fs_create_sample_buffer_raw(uint32_t sample_rate, size_t sample_count)
{
  FSampleBuffer *buffer;
  buffer = (FSampleBuffer*) malloc(sizeof(FSampleBuffer));
  memset(buffer, 0, sizeof(FSampleBuffer));
  buffer->sample_count = sample_count;
  buffer->sample_rate = sample_rate;
  buffer->buffer_size = sizeof(sample_t) * sample_count;
  buffer->samples = (sample_t*) malloc(buffer->buffer_size);
  memset(buffer->samples, 0, buffer->buffer_size);
  return buffer;
}

FSampleBuffer *fs_create_sample_buffer_prop(FSampleBuffer *buffer)
{
  return fs_create_sample_buffer_raw(buffer->sample_rate, buffer->sample_rate);
}

int fs_clear_sample_buffer(FSampleBuffer *buffer)
{
  fs_clear_error();
  if (INVALID_BUFFER(buffer)) {
    fs_set_error(FS_INVLID_BUFFER);
    return fs_get_error();
  }
  memset(buffer->samples, 0, buffer->buffer_size);
  return fs_get_error();
}

FSampleBuffer *fs_cat_sample_buffers(FSampleBuffer *buffer_a, FSampleBuffer *buffer_b)
{
  FSampleBuffer *pout = NULL;
  fs_clear_error();
  if (!INVALID_BUFFER(buffer_a) && !INVALID_BUFFER(buffer_b)) {
    pout = fs_create_sample_buffer_raw(buffer_a->sample_rate, buffer_a->sample_count + buffer_b->sample_count);
    memcpy(pout->samples, buffer_a->samples, buffer_a->buffer_size);
    memcpy(&pout->samples[buffer_a->sample_count-1], buffer_b->samples, buffer_b->buffer_size);
  } else {
    fs_set_error(FS_INVLID_BUFFER);
  }
  return pout;
}

FSampleBuffer *fs_repeat_sample_buffer(FSampleBuffer *buffer, int times)
{
  int i;
  FSampleBuffer *temp;
  FSampleBuffer *pout = buffer;
  if (times > 0) {
    pout = fs_cat_sample_buffers(buffer, buffer);
    for (i = 1; i < times-1; ++i) {
      temp = pout;
      pout = fs_cat_sample_buffers(pout, buffer);
      fs_delete_sample_buffer(&temp);
    }
  }
  return pout;
}

FSampleBuffer *fs_clone_sample_buffer(FSampleBuffer *buffer)
{
  FSampleBuffer *clone = fs_create_sample_buffer_raw(buffer->sample_rate, buffer->sample_count);
  memcpy(clone->samples, buffer->samples, buffer->buffer_size);
  return clone;
}

FSampleBuffer *fs_create_sample_buffer(uint32_t sample_rate, double duration)
{
  uint32_t sample_count = (uint32_t)(sample_rate * duration);
  return fs_create_sample_buffer_raw(sample_rate, sample_count);
}

int fs_scale_samples(FSampleBuffer *buffer, double level)
{
  size_t idx;
  fs_clear_error();
  if (INVALID_BUFFER(buffer)) {
    fs_set_error(FS_INVLID_BUFFER);
  } else {
    FOREACH_SAMPLE(buffer, idx) {
      buffer->samples[idx] *= level;
    }
  }
  return fs_get_error();
}

double fs_get_buffer_duration(FSampleBuffer *buffer)
{
  if (INVALID_BUFFER(buffer)) {
    fs_set_error(FS_INVLID_BUFFER);
  }
  return buffer->sample_count / (double)buffer->sample_rate;
}

size_t fs_get_buffer_position(FSampleBuffer *buffer, double time)
{
  size_t position = 0;
  if (INVALID_BUFFER(buffer)) {
    fs_set_error(FS_INVLID_BUFFER);
  } else  {
    position = buffer->sample_rate * time;
    position = MIN(buffer->sample_count, position);
  }
  return position;
}

double fs_get_time_position(FSampleBuffer *buffer, size_t pos)
{
  double time = 0;
  if (INVALID_BUFFER(buffer)) {
    fs_set_error(FS_INVLID_BUFFER);
  } else  {
    if (pos > buffer->sample_count) {
      fs_set_error(FS_INVLID_OPERATION | FS_INVLID_ARGUMENT);
    } else {
      time = pos / (double)buffer->sample_rate;
    }
  }
  return time;
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

int fs_normalize_buffer(FSampleBuffer *buffer)
{
  int idx;
  sample_t min_max_val;
  sample_t x, min_val = 1e300, max_val = 0;
  fs_clear_error();
  if (INVALID_BUFFER(buffer)) {
    fs_set_error(FS_INVLID_BUFFER);
    return fs_get_error();
  }
  FOREACH_SAMPLE(buffer, idx) {
    min_val = MIN(min_val, buffer->samples[idx]);
    max_val = MAX(max_val, buffer->samples[idx]);
  }
  min_max_val = max_val - min_val;
  if (min_max_val == 0) {
    fs_set_error(FS_DIVIDED_BY_ZERO);
    return fs_get_error();
  }
  FOREACH_SAMPLE(buffer, idx) {
    x = buffer->samples[idx];
    x = (x - min_val) / min_max_val;
    x = (x - .5) * 2.;
    buffer->samples[idx] = x;
  }
  return fs_get_error();
}

int fs_modulate_buffer(FSampleBuffer *dest, FSampleBuffer *src, int modulate_type)
{
  int idx;
  fs_clear_error();
  if (INVALID_BUFFER(dest) || INVALID_BUFFER(src)) {
    fs_set_error(FS_INVLID_BUFFER);
    return fs_get_error();
  }
  if (dest->sample_count < src->sample_count) {
    fs_set_error(FS_DIFF_SAMPLE_RATE);
    return fs_get_error();
  }
  FOREACH_SAMPLE(dest, idx) {
    switch (modulate_type) {
    case FS_MOD_ADD:
      dest->samples[idx] += src->samples[idx];
      break;
    case FS_MOD_SUB:
      dest->samples[idx] -= src->samples[idx];
      break;
    case FS_MOD_MULT:
      dest->samples[idx] *= src->samples[idx];
      break;
    case FS_MOD_DIV:
      dest->samples[idx] /= src->samples[idx];
      break;
    case FS_MOD_SQUARE:
      dest->samples[idx] *= src->samples[idx] * src->samples[idx];
      break;
    case FS_MOD_ROOT:
      dest->samples[idx] *= sqrt(src->samples[idx]);
      break;
    case FS_MOD_LOG:
      dest->samples[idx] *= log(src->samples[idx]);
      break;
    case FS_MOD_LOG10:
      dest->samples[idx] *= log10(src->samples[idx]);
      break;
    default:
      fs_set_error(FS_INVLID_ARGUMENT);
      break;
    }
  }
  return fs_get_error();
}

int wave_func_intern(sample_t *out, int func_type, double phase, double amp)
{
  sample_t result = 0;
  switch (func_type) {
  case FS_WAVE_SINE:
    result = (sample_t)sin(phase) * amp;
    break;
  case FS_WAVE_COSINE:
    result = (sample_t)cos(phase) * amp;
    break;
  case FS_WAVE_TRIANGLE:
    result = triangle(phase) * amp;
    break;
  case FS_WAVE_SAW:
    result = saw(phase) * amp;
    break;
  case FS_WAVE_RECT:
    result = rect(phase) * amp;
    break;
  case FS_WAVE_NOISE:
    result = ((rand() & 0xffff) / 65535. - .5) * 2.;
    break;
  default:
    return FS_ERROR;
  }
  *out = result;
  return FS_OK;
}

int fs_generate_wave_func(FSampleBuffer *buffer, int func_type, double freq, double amp)
{
  int idx;
  double shift, phase = 0;
  fs_clear_error();
  if (INVALID_BUFFER(buffer)) {
    fs_set_error(FS_INVLID_BUFFER);
    return fs_get_error();
  }
  if (freq == 0) {
    fs_set_error(FS_INVLID_ARGUMENT);
    return fs_get_error();
  }
  shift = M_PI * 2. / ((double)buffer->sample_rate / freq);
  FOREACH_SAMPLE(buffer, idx) {
    if (wave_func_intern(&buffer->samples[idx], func_type, phase, amp) != FS_OK) {
      fs_set_error(FS_INVLID_ARGUMENT);
      break;
    }
    phase = fmod(phase + shift, M_PI * 2.);
  }
  return fs_get_error();
}

int fs_modulate_frequency(FSampleBuffer *dest, FSampleBuffer *source, int func_type, double amp)
{
  int idx;
  double shift, phase = 0;
  fs_clear_error();
  if (INVALID_BUFFER(dest) || INVALID_BUFFER(source)) {
    fs_set_error(FS_INVLID_BUFFER);
    return fs_get_error();
  }
  FOREACH_SAMPLE(dest, idx) {
    shift = M_PI * 2. / ((double)dest->sample_rate / source->samples[idx]);
    if (wave_func_intern(&dest->samples[idx], func_type, phase, amp) != FS_OK) {
      fs_set_error(FS_INVLID_ARGUMENT);
      break;
    }
    phase = fmod(phase + shift, M_PI * 2.);
  }
  return fs_get_error();
}

void fs_delete_sample_buffer(FSampleBuffer **buffer)
{
  if ((*buffer)->buffer_size > 0) {
    free((*buffer)->samples);
  }
  free(*buffer);
  *buffer = NULL;
}
