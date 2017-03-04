#ifndef _FSYNTH_H_
#define _FSYNTH_H_

#ifndef _STDINT_H_
#include <stdint.h>
#endif

/* FSynth error codes */
#define FS_OK                 0
#define FS_ERROR              (1<<31)
#define FS_WARNING            (1<<30)
#define FS_INVLID_BUFFER      (1<<0)
#define FS_INVLID_ARGUMENT    (1<<1)
#define FS_INVLID_OPERATION   (1<<2)
#define FS_DIVIDED_BY_ZERO    (1<<3)
#define FS_FILE_IO_ERROR      (1<<4)
#define FS_WRONG_BUF_SIZE     (1<<5)
#define FS_DIFF_SAMPLE_RATE   (1<<6)
#define FS_INDEX_OUT_OF_RANGE (1<<7)

/* Waveform types */
#define WAVE_SINE            1
#define WAVE_COSINE          2
#define WAVE_SAW             3
#define WAVE_TRIANGLE        4
#define WAVE_RECT            5

/* Modulation types */
#define MOD_ADD              1
#define MOD_SUB              2
#define MOD_MULT             3
#define MOD_DIV              4
#define MOD_SQUARE           5
#define MOD_ROOT             6
#define MOD_LOG              7
#define MOD_LOG10            8

/* Wave output formats */
#define WAVE_FMT_8BIT        8
#define WAVE_FMT_16BIT       16

/* Function macros */
#define FAILED(x) (((x) & FS_ERROR) == FS_ERROR)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define LERP(min, max, t) ((1 - (t)) * (min)  + (t) * (max))
#define FOREACH_SAMPLE(buffer, idx) \
  for (idx = 0; idx < buffer->sample_count; ++idx)

#define INVALID_BUFFER(buffer) \
  (buffer == NULL || buffer->buffer_size == 0 || buffer->sample_count == 0 || buffer->sample_rate == 0)

/* Type definitions */
#ifndef SINGLE
typedef double sample_t;
#else
typedef float sample_t;
#endif

typedef struct {
  uint32_t sample_rate;
  size_t buffer_size;
  size_t sample_count;
  sample_t *samples;
} SampleBuffer;

/* Sample buffer base functions */
SampleBuffer *create_sample_buffer_raw(uint32_t sample_rate, size_t sample_count);
SampleBuffer *create_sample_buffer(uint32_t sample_rate, double duration);
SampleBuffer *clone_sample_buffer(SampleBuffer *buffer);
SampleBuffer *cat_sample_buffers(SampleBuffer *buffer_a, SampleBuffer *buffer_b);
SampleBuffer *repeat_sample_buffer(SampleBuffer *buffer, int times);
int modulate_buffer(SampleBuffer *dest, SampleBuffer *src, int modulate_type);
int modulate_frequency(SampleBuffer *dest, SampleBuffer *source, int func_type, double amp);
int normalize_buffer(SampleBuffer *buffer);
double get_buffer_duration(SampleBuffer *buffer);
int generate_wave_func(SampleBuffer *buffer, int func_type, double freq, double amp);
void delete_sample_buffer(SampleBuffer **buffer);

/* Wave file generation and sample conversion functions */
int samples_to_wave_file(SampleBuffer *buffer, const char *fname, int format, int channels);
void *convert_samples(SampleBuffer *buffer, int format);

#endif
