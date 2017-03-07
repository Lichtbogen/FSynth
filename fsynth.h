#ifndef _FSYNTH_H_
#define _FSYNTH_H_

#ifndef _STDINT_H_
#include <stdint.h>
#endif

/* FSynth error codes */
#define FS_OK                   0
#define FS_ERROR               (1<<31)
#define FS_WARNING             (1<<30)
#define FS_INVALID_BUFFER      (1<<0)
#define FS_INVALID_ARGUMENT    (1<<1)
#define FS_INVALID_OPERATION   (1<<2)
#define FS_DIVIDED_BY_ZERO     (1<<3)
#define FS_FILE_IO_ERROR       (1<<4)
#define FS_WRONG_BUF_SIZE      (1<<5)
#define FS_DIFF_SAMPLE_RATE    (1<<6)
#define FS_INDEX_OUT_OF_RANGE  (1<<7)
#define FS_OUT_OF_MEMORY       (1<<8)

/* Waveform types */
#define FS_WAVE_SINE           1
#define FS_WAVE_COSINE         2
#define FS_WAVE_SAW            3
#define FS_WAVE_TRIANGLE       4
#define FS_WAVE_RECT           5
#define FS_WAVE_NOISE          6

/* Modulation types */
#define FS_MOD_ADD             1
#define FS_MOD_SUB             2
#define FS_MOD_MULT            3
#define FS_MOD_DIV             4
#define FS_MOD_SQUARE          5
#define FS_MOD_ROOT            6
#define FS_MOD_LOG             7
#define FS_MOD_LOG10           8

/* Hull curve types */
#define FS_CURVE_LINEAR        1
#define FS_CURVE_TAN           2
#define FS_CURVE_SQUARE        3
#define FS_CURVE_CUBIC         4
#define FS_CURVE_HOLD          5

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

#define MIDI_NOTE(octave, note, level) \
  ((octave * 12 + (note & 0x7f)) | (level << 8) & 0xffff)

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
  size_t hull_ptr;
  sample_t hull_level;
  sample_t *samples;
} FSampleBuffer;

typedef struct {
  int func_type;
  FSampleBuffer* hull_curve;
  FSampleBuffer* output;
} FSTrackChannel;

/* Error handling */
void fs_set_error(int code);
void fs_set_warning(int code);
int fs_get_error(void);
int fs_clear_error(void);
void fs_print_error(int code);

/* Sample buffer base functions */
FSampleBuffer *fs_create_sample_buffer_raw(uint32_t sample_rate, size_t sample_count);
FSampleBuffer *fs_create_sample_buffer_prop(FSampleBuffer *buffer);
FSampleBuffer *fs_create_sample_buffer(uint32_t sample_rate, double duration);
FSampleBuffer *fs_clone_sample_buffer(FSampleBuffer *buffer);
FSampleBuffer *fs_cat_sample_buffers(FSampleBuffer *buffer_a, FSampleBuffer *buffer_b);
FSampleBuffer *fs_repeat_sample_buffer(FSampleBuffer *buffer, int times);
int fs_cat_sample_buffers_inplace(FSampleBuffer *buffer_a, FSampleBuffer *buffer_b);
int fs_repeat_sample_buffer_inplace(FSampleBuffer *buffer, int times);
int fs_modulate_buffer(FSampleBuffer *dest, FSampleBuffer *src, int modulate_type);
int fs_modulate_frequency(FSampleBuffer *dest, FSampleBuffer *source, int func_type, double amp);
int fs_normalize_buffer(FSampleBuffer *buffer);
double fs_get_buffer_duration(FSampleBuffer *buffer);
size_t fs_get_buffer_position(FSampleBuffer *buffer, double time);
int fs_scale_samples(FSampleBuffer *buffer, double level);
int fs_resize_sample_buffer(FSampleBuffer *buffer, size_t new_size);

/**
 * @brief Converts a sample position into the corresponding time value.
 * @param pos the sample position
 * @return the time value
 */
double fs_get_time_position(FSampleBuffer *buffer, size_t pos);

/**
 * @brief This function generates a base waveform with given frequnecy and amplitude.
 * @param buffer the target buffer object
 * @param func_type the wave form type (e.g: FS_WAVE_SINE)
 * @param freq the frequnecy in Hz with fraction part
 * @param amp the aplitude value as percentage value with range from 0.0 - 1.0
 * @return FS_OK or an error code on failure
 */
int fs_generate_wave_func(FSampleBuffer *buffer, int func_type, double freq, double amp);

/**
 * @brief deletes an existing sample buffer object and frees its memory
 * @param buffer a pointer to the buffer object which should be deleted
 */
void fs_delete_sample_buffer(FSampleBuffer **buffer);

/* Hull curves */
int fs_attack_decay(FSampleBuffer *buffer, int curve_type, double time, double level);
int fs_sustain(FSampleBuffer *buffer, double time);
int fs_release(FSampleBuffer *buffer, int curve_type);

/* Wave file generation and sample conversion functions */
int fs_samples_to_wave_file(FSampleBuffer *buffer, const char *fname, int format, int channels);
void *fs_convert_samples(FSampleBuffer *buffer, int format);

int fs_track_sequence(FSTrackChannel *channel, uint16_t* data, size_t length);

#endif
