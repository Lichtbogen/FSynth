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
#define FS_WAVE_SINE           1
#define FS_WAVE_COSINE         2
#define FS_WAVE_SAW            3
#define FS_WAVE_TRIANGLE       4
#define FS_WAVE_RECT           5

/* Modulation types */
#define FS_MOD_ADD             1
#define FS_MOD_SUB             2
#define FS_MOD_MULT            3
#define FS_MOD_DIV             4
#define FS_MOD_SQUARE          5
#define FS_MOD_ROOT            6
#define FS_MOD_LOG             7
#define FS_MOD_LOG10           8

/* Wave output formats */
#define WAVE_PCM_8BIT        8
#define WAVE_PCM_16BIT       16

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
} FSampleBuffer;

/* Error handling */
void fs_set_error(int code);
void fs_set_warning(int code);
int fs_get_error(void);
int fs_clear_error(void);
void fs_print_error(int code);

/* Sample buffer base functions */
FSampleBuffer *fs_create_sample_buffer_raw(uint32_t sample_rate, size_t sample_count);
FSampleBuffer *fs_create_sample_buffer(uint32_t sample_rate, double duration);
FSampleBuffer *fs_clone_sample_buffer(FSampleBuffer *buffer);
FSampleBuffer *fs_cat_sample_buffers(FSampleBuffer *buffer_a, FSampleBuffer *buffer_b);
FSampleBuffer *fs_repeat_sample_buffer(FSampleBuffer *buffer, int times);
int fs_modulate_buffer(FSampleBuffer *dest, FSampleBuffer *src, int modulate_type);
int fs_modulate_frequency(FSampleBuffer *dest, FSampleBuffer *source, int func_type, double amp);
int fs_normalize_buffer(FSampleBuffer *buffer);
double fs_get_buffer_duration(FSampleBuffer *buffer);
int fs_generate_wave_func(FSampleBuffer *buffer, int func_type, double freq, double amp);
void fs_delete_sample_buffer(FSampleBuffer **buffer);

/* Wave file generation and sample conversion functions */
int fs_samples_to_wave_file(FSampleBuffer *buffer, const char *fname, int format, int channels);
void *fs_convert_samples(FSampleBuffer *buffer, int format);

#endif
