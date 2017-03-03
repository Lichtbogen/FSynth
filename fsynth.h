#ifndef _FSYNTH_H_
#define _FSYNTH_H_

#define FS_OK          0
#define FS_ERROR      -1

/* Waveform types */
#define WAVE_SINE       1
#define WAVE_COSINE     2
#define WAVE_SAW        3
#define WAVE_TRIANGLE   4
#define WAVE_RECT       5

/* Modulation types */
#define MOD_ADD         1
#define MOD_SUB         2
#define MOD_MULT        3
#define MOD_DIV         4

/* Wave output formats */
#define WAVE_FMT_8BIT   8
#define WAVE_FMT_16BIT  16

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define LERP(min, max, t) ((1 - (t)) * (min)  + (t) * (max))
#define FOREACH_SAMPLE(buffer, idx) \
  for (idx = 0; idx < buffer->sample_count; ++idx)

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
int modulate_buffer(SampleBuffer *dest, SampleBuffer *src, int modulate_type);
int normalize_buffer(SampleBuffer *buffer);
double get_buffer_duration(SampleBuffer *buffer);
int generate_wave_func(SampleBuffer *buffer, int func_type, double freq, double amp);
void delete_sample_buffer(SampleBuffer **buffer);
int samples_to_wave_file(SampleBuffer *buffer, const char *fname, int format, int channels);

#endif
