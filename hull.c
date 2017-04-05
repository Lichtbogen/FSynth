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
 * @brief Function for creating or modifyling cull curves
 * @author Pierre Biermann
 * @date 2017-03-10
 */

#include <stdlib.h>
#include <math.h>
#include "fsynth.h"

int fs_attack_decay(FSampleBuffer *buffer, int curve_type, double time, double level)
{
  size_t idx, start_pos, end_pos;
  sample_t start_level, end_level, x;
  sample_t range, pos, lerp;
  fs_clear_error();
  if (INVALID_BUFFER(buffer)) {
    fs_set_error(FS_INVALID_BUFFER);
    return fs_get_error();
  }
  start_pos = buffer->hull_ptr;
  end_pos = buffer->hull_ptr + fs_get_buffer_position(buffer, time);
  end_pos = MIN(buffer->sample_count, end_pos);
  if (end_pos < start_pos) {
    fs_set_error(FS_INVALID_OPERATION | FS_INVALID_ARGUMENT);
  } else {
    start_level = buffer->hull_level;
    end_level = buffer->hull_level + (sample_t)level;
    range = end_pos - start_pos;
    for (idx = start_pos; idx < end_pos; ++idx) {
      pos = idx - start_pos;
      x = pos / range;
      lerp = LERP(start_level, end_level, x);
      switch (curve_type) {
        case FS_CURVE_LINEAR:
          buffer->samples[idx] = lerp;
          break;
        case FS_CURVE_TAN:
          buffer->samples[idx] = tan(lerp);
          break;
        case FS_CURVE_SQUARE:
          buffer->samples[idx] = lerp * lerp;
          break;
        case FS_CURVE_CUBIC:
          buffer->samples[idx] = lerp * lerp * lerp;
          break;
        case FS_CURVE_HOLD:
          buffer->samples[idx] = buffer->hull_level;
          break;
        default:
          fs_set_error(FS_INVALID_ARGUMENT);
          return fs_get_error();
      }
    }
    buffer->hull_ptr = end_pos;
    buffer->hull_level = end_level;
  }
  return fs_get_error();
}

int fs_sustain(FSampleBuffer *buffer, double time)
{
  fs_attack_decay(buffer, FS_CURVE_HOLD, time, 0);
  return fs_get_error();
}

int fs_release(FSampleBuffer *buffer, int curve_type)
{
  double time = fs_get_time_position(buffer, buffer->sample_count);
  fs_attack_decay(buffer, curve_type, time, -buffer->hull_level);
  return fs_get_error();
}
