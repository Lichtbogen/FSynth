#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fsynth.h"

FSampleBuffer *test_noise(void)
{
  double play_time;
  FSampleBuffer *out;
  FSTrackChannel channel;
  uint16_t track[16];

  //FSampleBuffer *base = fs_create_sample_buffer(44100, 0.2);
  channel.hull_curve = fs_create_sample_buffer(44100, 0.2);
  channel.func_type = FS_WAVE_SINE;

  //fs_generate_wave_func(base, FS_WAVE_RECT, 440, 1);

  play_time = fs_get_buffer_duration(channel.hull_curve);
  fs_attack_decay(channel.hull_curve, FS_CURVE_SQUARE, play_time * 0.1, 1);
  fs_release(channel.hull_curve, FS_CURVE_SQUARE);

  track[0] = MIDI_NOTE(5, 2, 255);
  track[1] = MIDI_NOTE(5, 3, 255);
  track[2] = MIDI_NOTE(5, 4, 255);
  track[3] = MIDI_NOTE(5, 3, 127);
  track[4] = MIDI_NOTE(5, 2, 127);
  track[5] = MIDI_NOTE(5, 1, 255);
  fs_track_sequence(&channel, track, 6);

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
  modulate_buffer(buf, hig, MOD_MULT);*/
  fs_generate_wave_func(mod, FS_WAVE_SINE, 50, 500);
  fs_modulate_frequency(buf, mod, FS_WAVE_SINE, 1);
  fs_attack_decay(hull, FS_CURVE_CUBIC, .03, 1);
  fs_sustain(hull, .5);
  fs_release(hull, FS_CURVE_LINEAR);
  fs_modulate_buffer(buf, hull, FS_MOD_MULT);
  fs_normalize_buffer(buf);

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
