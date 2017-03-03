#ifndef _FSYNTH_H_
#define _FSYNTH_H_

#define FS_OK       0
#define FS_ERROR   -1

/* Waveform types */
#define WAVE_SINE      1
#define WAVE_COSINE    2
#define WAVE_SAW       3
#define WAVE_TRIANGLE  4
#define WAVE_RECT      5

#define LERP(min, max, t) ((1 - t) * min  + t * max)
#define FOREACH_SAMPLE(buffer) \
  for (size_t idx = 0; idx < buffer->sample_count; ++idx)

#define INVALID_BUFFER(buffer) \
  (buffer == NULL || buffer->buffer_size == 0 || buffer->sample_count == 0 || buffer->sample_rate == 0)

typedef double sample_t;

typedef struct {
  uint32_t sample_rate;
  size_t buffer_size;
  size_t sample_count;
  sample_t *samples;
} SampleBuffer;

SampleBuffer *create_sample_buffer_raw(uint32_t sample_rate, size_t sample_count);
SampleBuffer *create_sample_buffer(uint32_t sample_rate, double duration);
double get_buffer_duration(SampleBuffer *buffer);
int generate_wave_func(SampleBuffer *buffer, int func_type, double freq, double amp);
void delete_sample_buffer(SampleBuffer **buffer);

#endif
