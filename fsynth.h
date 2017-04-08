/*
 * Copyright (c) 2017 Pierre Biermann
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @brief FSynth library main include file
 * @author Pierre Biermann
 * @date 2017-03-10
 */

#ifndef _FSYNTH_H_
#define _FSYNTH_H_

#define FS_VERSION             "0.0.0.1"

#ifndef _STDINT_H_
#include <stdint.h>
#endif

/* FSynth error codes */
#define FS_OK                   0
#define FS_ERROR               (1<<31)
#define FS_WARNING             (1<<30)
#define FS_EXIT                (1<<29)
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
#define dB(x) (pow(10, (x)/20.))
#define FAILED(x) (((x) & FS_ERROR) == FS_ERROR)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define LERP(min, max, t) ((1 - (t)) * (min)  + (t) * (max))
#define RAND_F ((rand() & 0xffff) / 65535.)
#define FOREACH_SAMPLE(buffer, idx) \
  for (idx = 0; idx < buffer->sample_count; ++idx)

#define INVALID_BUFFER(buffer) \
  (buffer == NULL || buffer->buffer_size == 0 || buffer->sample_count == 0 || buffer->sample_rate == 0)

/* Type definitions */
#ifndef SINGLE
#define DOUBLE_SAMPLE
typedef double sample_t;
#else
#define SINGLE_SAMPLE
typedef float sample_t;
#endif

/* Function pointer data type for shell command callbacks */
typedef int (*FShellCallback)(int, char **);

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

/**
 * @brief Creates a buffer with given sample rate and the amount of samples.
 * @param sample_rate the sample rate for the buffer
 * @param sample_count the amount of samples for the new buffer
 * @return a pointer to the new buffer or NULL on failure
 */
FSampleBuffer *fs_create_sample_buffer_raw(uint32_t sample_rate, size_t sample_count);

/**
 * @brief Creates a new sample buffer by copying the properties from an already existing buffer.
 * @param buffer the source buffer object
 * @return a pointer to the new buffer or NULL on failure
 */
FSampleBuffer *fs_create_sample_buffer_prop(FSampleBuffer *buffer);

/**
 * @brief Creates a new sample buffer object with given sample rate and play duration.
 * @param sample_rate the sample rate for the buffer
 * @param duration the duration time of the buffer.
 * @return a pointer to the new buffer or NULL on failure
 */
FSampleBuffer *fs_create_sample_buffer(uint32_t sample_rate, double duration);

/**
 * @brief Creates an exact copy of an existing buffer object with all properties and data.
 * @param buffer the buffer to copy
 * @return a pointer to the new buffer or NULL on failure
 */
FSampleBuffer *fs_clone_sample_buffer(FSampleBuffer *buffer);

/**
 * @brief Cats two sample buffers together and return a new one with that data.
 * @param buffer_a the first buffer
 * @param buffer_b the second buffer
 * @return a new buffer with combined data or NULL on failure
 */
FSampleBuffer *fs_cat_sample_buffers(FSampleBuffer *buffer_a, FSampleBuffer *buffer_b);

/**
 * @brief Cats copies the seconds buffer at the end of the first buffer
 * @param buffer_a the first buffer
 * @param buffer_b the second buffer
 * @return FS_OK or an error code on failure
 */
int fs_cat_sample_buffers_inplace(FSampleBuffer *buffer_a, FSampleBuffer *buffer_b);

/**
 * @brief Repeats the content of a buffer n-times and creates a new one from these data
 * @param buffer the buffer to be repeated
 * @param times the amount of copies which shall be created
 * @return a new buffer with repeated data or NULL on failure
 */
FSampleBuffer *fs_repeat_sample_buffer(FSampleBuffer *buffer, int times);

/**
 * @brief Repeats the content of a buffer n-times without creating a new one.
 * @param buffer the buffer to be repeated
 * @param times the amount of copies which shall be created
 * @return FS_OK or an error code on failure
 */
int fs_repeat_sample_buffer_inplace(FSampleBuffer *buffer, int times);

/**
 * @brief Sets a new size of the given sample buffer without destroying it's content
 * @param buffer the buffer to be repeated
 * @param new_size the new amount of samples for the buffer
 * @return FS_OK or an error code on failure
 */
int fs_resize_sample_buffer(FSampleBuffer *buffer, size_t new_size);

/**
 * @brief Performs a amplitude modulation based on the content of two input buffers.
 *        The result of the modulation will be stored back to the destination buffer (dest).
 *        The function call will fail, if the sample rate of the two buffers are different or
 *        if the source buffer is smaller than the destination buffer.
 * @param dest The dest buffer object
 * @param src The source buffer object
 * @param modulate_type The type of the modulation which shall be performed.
 *        Possible values are: FS_MOD_ADD, FS_MOD_SUB,
 *        FS_MOD_MULT, FS_MOD_DIV, FS_MOD_SQUARE, FS_MOD_ROOT, FS_MOD_LOG and FS_MOD_LOG10
 * @return FS_OK or an error code on failure
 */
int fs_modulate_buffer(FSampleBuffer *dest, FSampleBuffer *src, int modulate_type);

/**
 * @brief Performs a frequency modulation of a given waveform type by using the sample
 *        values from another sample buffer.
 * @param dest The target buffer object.
 *        The existing values will be overwritten and the previous content doesn't affect the output.
 * @param source The source sample buffer with the samples which shall be used for the modulation
 * @param func_type The waveform type which shall be modulated. Possible values are:
 *        FS_WAVE_SINE, FS_WAVE_COSINE, FS_WAVE_SAW, FS_WAVE_TRIANGLE, FS_WAVE_RECT or FS_WAVE_NOISE
 * @param amp The output amplitude as percentage value in range of 0 to 1 with fraction part
 * @return FS_OK or an error code on failure
 */
int fs_modulate_frequency(FSampleBuffer *dest, FSampleBuffer *source, int func_type, double amp);

/**
 * @brief Normalize all amplitude values from the given buffer object into a value range of -1 to 1.
 * @param The target buffer object
 * @return FS_OK or an error code on failure
 */
int fs_normalize_buffer(FSampleBuffer *buffer);

/**
 * @brief Returns the total play length of a given sample buffer object.
 * @param buffer the buffer object instance
 * @return the total play time in seconds with fraction
 */
double fs_get_buffer_duration(FSampleBuffer *buffer);

/**
 * @brief Gets the buffer offset position from given time value.
 * @param buffer The buffer object
 * @time the buffer position as time value given in seconds with fraction part
 * @return the buffer index of the sample
 */
size_t fs_get_buffer_position(FSampleBuffer *buffer, double time);

/**
 * @brief Scales the amplitude values of all samples from the given buffer object with the value level.
 * @param buffer Target buffer object
 * @param level scale value
 * @return FS_OK or an error code on failure
 */
int fs_scale_samples(FSampleBuffer *buffer, double level);

/**
 * @brief Converts a sample position into the corresponding time value.
 * @param pos the sample position
 * @return the time value
 */
double fs_get_time_position(FSampleBuffer *buffer, size_t pos);

/**
 * @brief This function generates a base waveform with given frequency and amplitude.
 * @param buffer the target buffer object
 * @param func_type the wave form type (e.g: FS_WAVE_SINE)
 * @param freq the frequency in Hz with fraction part
 * @param amp the amplitude value as percentage value with range from 0.0 - 1.0
 * @return FS_OK or an error code on failure
 */
int fs_generate_wave_func(FSampleBuffer *buffer, int func_type, double freq, double amp);

/**
 * @brief deletes an existing sample buffer object and frees its memory
 * @param buffer a pointer to the buffer object which should be deleted
 */
void fs_delete_sample_buffer(FSampleBuffer **buffer);

/**
 * @brief Adds an attack or decay phase to the output buffer and moves the
 *        hull curve pointer to the end of this phase
 *        It is also possible to add more than one attack or
 *        decay phases in series by calling this function multible times.
 * @param buffer the buffer object which holds the samples for the hull curve
 * @param curve_type the curve type defines the method how the curve is calculated
 *        possible values are: FS_CURVE_LINEAR, FS_CURVE_TAN, FS_CURVE_SQUARE, FS_CURVE_CUBIC  and FS_CURVE_HOLD
 * @param time The time how long the attack or decay phase should be. After this time the target amplitude level is reached
 * @param level The relative amplitude value which should be reached at the end of this phase.
 *        The value will be stored within he buffer object and is the starting value for the next call of fs_attack_decay, fs_sustain or fs_release
 * @return FS_OK or an error code on failure
 */
int fs_attack_decay(FSampleBuffer *buffer, int curve_type, double time, double level);

/**
 * @brief Adds a sustain phase to the hull curve buffer which means a phase with an constant amplitude level.
 *        Typically a sustain phase is inserted right after a attack or decay phase
 * @param time The time for this phase
 * @return FS_OK or an error code on failure
 */
int fs_sustain(FSampleBuffer *buffer, double time);

/**
 * @brief Adds an release phase to the hull curve which means a fade out to zero level amplitude at the end of the buffer.
 *        In general case the release phase is the last phase within a hull curve
 *        and an additional call to fs_attack_decay, fs_sustain or fs_release will not take any further effect.
 * @param curve_type the curve type defines the method how the curve is calculated
 *        possible values are: FS_CURVE_LINEAR, FS_CURVE_TAN, FS_CURVE_SQUARE, FS_CURVE_CUBIC  and FS_CURVE_HOLD
 * @return FS_OK or an error code on failure
 */
int fs_release(FSampleBuffer *buffer, int curve_type);

/**
 * @brief This function writes all sample values from the given buffer object
 *        into a WAVE file with the specified data format and the number of audio channels
 * @param buffer The buffer with the data which should be written
 * @return FS_OK or an error code on failure
 */
int fs_samples_to_wave_file(FSampleBuffer *buffer, const char *fname, int format, int channels);

/**
 * @brief Converts the content of a sample buffer into a format which can be used by common audio hardware for playback.
 * @param buffer the buffer with the samples which shall be converted
 * @return a pointer to the output data, after usage the used memory can be freed by calling the standard function free()
 */
void *fs_convert_samples(FSampleBuffer *buffer, int format);

/**
 * @brief Generates a pink noise which means a set of overlapped functions with limited bandwidth and randomized amplitudes
 * @param buffer the buffer with the samples which shall be converted
 * @param func_type the function type possible values are:
 *                  FS_WAVE_SINE, FS_WAVE_COSINE, FS_WAVE_SAW, FS_WAVE_TRIANGLE, FS_WAVE_RECT or FS_WAVE_NOISE
 * @param min_freq the smallest frequency to be generate
 * @param max_freq the gretest frequency to be generate
 * @param overlays the amount of overlapped functions
 * @return FS_OK or an error code on failure
 */
int fs_generate_pink_noise(FSampleBuffer *buffer, int func_type, double min_freq, double max_freq, int overlays);

/**
 * @brief Generates a sequence of MIDI notes based upon the given notes string
 * @param seq input string with notes
 * @param pointer to a buffer which carries the output data
 * @param length maximal length of the output data
 * @return number of values which has been written to the output buffer
 */
size_t fs_parse_notes(const char *seq, uint16_t *data, size_t length);

/**
 * @brief Generates a sequencer track with an output sample buffer and a given hull curve
 * @param channel pointer to a FSTrackChannel pointer
 * @param octave Octave adjustment as a relative value
 * @param data pointer to MIDI data
 * @param length number of elements within the MIDI data buffer
 * @return FS_OK or an error code on failure
 */
int fs_track_sequence(FSTrackChannel *channel, int octave, uint16_t *data, size_t length);

#endif /* _FSYNTH_H_ */
