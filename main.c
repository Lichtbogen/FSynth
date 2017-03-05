#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fsynth.h"

int main(int argc, char **argv)
{
  int idx;
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
  fs_attack_decay(hull, .2, 1);
  fs_sustain(hull, .5);
  fs_release(hull);
  fs_modulate_buffer(buf, hull, FS_MOD_MULT);
  fs_normalize_buffer(buf);

  out = fs_repeat_sample_buffer(buf, 4);
  fs_samples_to_wave_file(out, "tone.wav", WAVE_PCM_16BIT, 1);

  fs_delete_sample_buffer(&out);
  fs_delete_sample_buffer(&hull);
  fs_delete_sample_buffer(&buf);
  fs_delete_sample_buffer(&mod);
  return 0;
}
