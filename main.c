#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fsynth.h"

int main(int argc, char **argv)
{
  int idx;
  SampleBuffer *buf = create_sample_buffer(44100, 1);
  SampleBuffer *mod = create_sample_buffer(44100, 1);
  SampleBuffer *hig = create_sample_buffer(44100, 1);

  /*generate_wave_func(buf, WAVE_SAW, 500, 1);
  generate_wave_func(mod, WAVE_SINE, 120, .6);
  generate_wave_func(hig, WAVE_SINE, 1000, .4);
  modulate_buffer(buf, mod, MOD_MULT);
  modulate_buffer(buf, hig, MOD_MULT);*/
  generate_wave_func(mod, WAVE_SINE, 50, 500);
  modulate_frequency(buf, mod, WAVE_SINE, 1);
  normalize_buffer(buf);

  samples_to_wave_file(buf, "tone.wav", WAVE_FMT_16BIT, 1);

  delete_sample_buffer(&hig);
  delete_sample_buffer(&buf);
  delete_sample_buffer(&mod);
  return 0;
}
