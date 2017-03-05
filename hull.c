#include <stdlib.h>
#include "fsynth.h"

int fs_attack_decay(FSampleBuffer *buffer, double time, double level)
{
  size_t idx, start_pos, end_pos;
  sample_t start_level, end_level, x;
  sample_t range, pos;
  fs_clear_error();
  if (INVALID_BUFFER(buffer)) {
    fs_set_error(FS_INVLID_BUFFER);
    return fs_get_error();
  }
  start_pos = buffer->hull_ptr;
  end_pos = buffer->hull_ptr + fs_get_buffer_position(buffer, time);
  end_pos = MIN(buffer->sample_count, end_pos);
  if (end_pos < start_pos) {
    fs_set_error(FS_INVLID_OPERATION | FS_INVLID_ARGUMENT);
  } else {
    start_level = buffer->hull_level;
    end_level = buffer->hull_level + (sample_t)level;
    range = end_pos - start_pos;
    for (idx = start_pos; idx < end_pos; ++idx) {
      pos = idx - start_pos;
      x = pos / range;
      buffer->samples[idx] = LERP(start_level, end_level, x);
    }
    buffer->hull_ptr = end_pos;
    buffer->hull_level = end_level;
  }
  return fs_get_error();
}

int fs_sustain(FSampleBuffer *buffer, double time)
{
  fs_attack_decay(buffer, time, 0);
  return fs_get_error();
}

int fs_release(FSampleBuffer *buffer)
{
  double time = fs_get_time_position(buffer, buffer->sample_count);
  fs_attack_decay(buffer, time, -buffer->hull_level);
  return fs_get_error();
}
