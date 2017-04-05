#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "fsynth.h"

void shell_loop();

FSampleBuffer *test_noise(void)
{
  double play_time;
  FSampleBuffer *out;
  FSTrackChannel channel;
  uint16_t track[16];
  size_t len;

  len = fs_parse_notes("c4d4@8e4@6f4c#4@8e#4@8", track, 16);

  //FSampleBuffer *base = fs_create_sample_buffer(44100, 0.2);
  channel.hull_curve = fs_create_sample_buffer(44100, 0.2);
  channel.func_type = FS_WAVE_SINE;

  //fs_generate_wave_func(base, FS_WAVE_RECT, 440, 1);

  play_time = fs_get_buffer_duration(channel.hull_curve);
  fs_attack_decay(channel.hull_curve, FS_CURVE_SQUARE, play_time * 0.1, 1);
  fs_release(channel.hull_curve, FS_CURVE_SQUARE);

  fs_track_sequence(&channel, 1, track, 6);

  fs_normalize_buffer(channel.output);
  fs_delete_sample_buffer(&channel.hull_curve);
  return channel.output;
}

int main(int argc, char **argv)
{
  shell_loop();
  return 0;
}
