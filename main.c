#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "fsynth.h"

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
  FSampleBuffer *out;
  FSampleBuffer *buf = fs_create_sample_buffer(44100, 1);
  FSampleBuffer *mod = fs_create_sample_buffer(44100, 1);
  FSampleBuffer *hull = fs_create_sample_buffer(44100, 1);

  /*generate_wave_func(buf, WAVE_SAW, 500, 1);
  generate_wave_func(mod, WAVE_SINE, 120, .6);
  generate_wave_func(hig, WAVE_SINE, 1000, .4);
  modulate_buffer(buf, mod, MOD_MULT);
  modulate_buffer(buf, hig, MOD_MULT);
  fs_generate_wave_func(mod, FS_WAVE_SINE, 50, 500);*/

  fs_generate_pink_noise(buf, FS_WAVE_SINE, 50, 800, 30);

  /*fs_modulate_frequency(buf, mod, FS_WAVE_SINE, 1);*/
  fs_attack_decay(hull, FS_CURVE_CUBIC, .03, 1);
  fs_sustain(hull, .5);
  fs_release(hull, FS_CURVE_LINEAR);
  fs_modulate_buffer(buf, hull, FS_MOD_MULT);
  fs_normalize_buffer(buf);
  fs_scale_samples(buf, dB(-8));

  out = fs_repeat_sample_buffer(buf, 4);
  fs_samples_to_wave_file(out, "tone.wav", WAVE_PCM_16BIT, 1);
  fs_delete_sample_buffer(&out);

  out = test_noise();
  fs_samples_to_wave_file(out, "noise.wav", WAVE_PCM_16BIT, 1);
  fs_delete_sample_buffer(&out);

  fs_delete_sample_buffer(&hull);
  fs_delete_sample_buffer(&buf);
  fs_delete_sample_buffer(&mod);
  return 0;
}
