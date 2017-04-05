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
 * @brief Basic sequencer functionality
 * @author Pierre Biermann
 * @date 2017-03-10
 */

#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <math.h>
#include "fsynth.h"

const double midi_notes[] = {
/*   C       C#      D       D#      E       F       F#      G      G#       A      A#       H                 */
  8.662 ,  9.177,  9.723, 10.301, 10.913, 11.562, 12.250, 12.978, 13.750, 14.568, 15.434, 16.352, /* Octave -1 */
  17.324, 18.354, 19.445, 20.601, 21.826, 23.124, 24.499, 25.956, 27.50 , 29.135, 30.867, 32.703, /* Octave  0 */
  34.648, 36.708, 38.890, 41.203, 43.653, 46.249, 48.999, 51.913, 55.000, 58.270, 61.735, 65.406, /* Octave  1 */
  69.295, 73.416, 77.781, 82.406, 87.307, 92.499, 97.998, 103.82, 110.00, 116.54, 123.47, 130.81, /* Octave  2 */
  138.59, 146.83, 155.56, 164.81, 174.61, 184.99, 195.99, 207.65, 220.00, 233.08, 246.94, 261.63, /* Octave  3 */
  277.18, 293.66, 311.13, 349.23, 369.99, 391.99, 415.31, 440.00, 466.16, 493.88, 523.25, 554.37, /* Octave  4 */
  587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880.00, 932.32, 987.77, 1046.5, 1108.7, /* Octave  5 */
  1174.7, 1244.5, 1318.5, 1396.9, 1480.0, 1568.0, 1661.2, 1760.0, 1864.7, 1975.5, 2093.0, 2217.5, /* Octave  6 */
  2349.3, 2489.0, 2637.0, 2793.8, 2960.0, 3136.0, 3322.4, 3520.0, 3729.3, 3951.1, 4186.0, 4434.9, /* Octave  7 */
  4698.6, 4978.0, 5274.0, 5587.7, 5919.9, 6271.9, 6644.9, 7040.0, 7458.6, 7902.1, 8372.0, 8869.8, /* Octave  8 */
  9397.3, 9956.1, 10548.1, 11175.3, 11839.8, 12543.9                                              /* Octave  9 */
};

size_t fs_parse_notes(const char *seq, uint16_t *data, size_t length)
{
  int octave, dbi = -1;
  char ch, lastch = 0;
  char dbval[4];
  size_t dl = 0, di = 0;
  while (*seq) {
    ch = toupper((int)*seq);
    switch (ch) {
    case 'C':
      dl = di;
      data[di++] = 0;
      lastch = ch;
      break;
    case 'D':
      dl = di;
      data[di++] = 2;
      lastch = ch;
      break;
    case 'E':
      dl = di;
      data[di++] = 4;
      lastch = ch;
      break;
    case 'F':
      dl = di;
      data[di++] = 5;
      lastch = ch;
      break;
    case 'G':
      dl = di;
      data[di++] = 7;
      lastch = ch;
      break;
    case 'A':
      dl = di;
      data[di++] = 9;
      lastch = ch;
      break;
    case 'H':
      dl = di;
      data[di++] = 11;
      lastch = ch;
      break;
    case '#':
      if (lastch != 'E' && lastch != 'H') data[dl] += 1;
      break;
    case '@':
      dbi = 0;
      memset(dbval, '\0', sizeof(dbval));
      break;
    default:
      if (isdigit((int)ch)) {
        if (dbi == -1) {
          octave = (ch - 0x30 + 1) * 12;
          data[dl] += octave;
        } else {
          dbval[dbi++] = ch;
        }
      }
      break;
    }
    if (!isdigit((int)ch) && dbi > 0 && dl > 0) {
      data[dl-1] |= atoi(dbval) << 8;
      dbi = -1;
    }
    dbi = MIN(dbi, 3);
    if (di >= length) break;
    ++seq;
  }
  if (dbi > 0 && dl >= 0) {
    data[dl] |= atoi(dbval) << 8;
  }
  return di;
}

int fs_track_sequence(FSTrackChannel *channel, int octave, uint16_t *data, size_t length)
{
  size_t idx;
  uint8_t note, amp;
  FSampleBuffer *tone;
  fs_clear_error();
  tone = fs_create_sample_buffer_prop(channel->hull_curve);
  if (FAILED(fs_get_error())) {
    return fs_get_error();
  }
  for (idx = 0; idx < length; ++idx) {
    note = (data[idx] + octave * 12) & 0x7f;  /* Low byte used for note */
    amp = data[idx] >> 8;     /* High byte used for amplitude */
    fs_generate_wave_func(tone, channel->func_type, midi_notes[note], dB(-amp));
    fs_modulate_buffer(tone, channel->hull_curve, FS_MOD_MULT);
    if (FAILED(fs_get_error())) {
      return fs_get_error();
    }
    if (idx == 0) {
      channel->output = fs_clone_sample_buffer(tone);
    } else {
      fs_cat_sample_buffers_inplace(channel->output, tone);
    }
  }
  return fs_get_error();
}
