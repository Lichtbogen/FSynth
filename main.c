#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fsynth.h"

int main(int argc, char **argv)
{
  int idx;
  SampleBuffer *buf = create_sample_buffer(44100, 1);
  SampleBuffer *mod = create_sample_buffer(44100, 1);

  generate_wave_func(buf, WAVE_SINE, 500, 1);
  generate_wave_func(mod, WAVE_SINE, 120, .6);
  modulate_buffer(buf, mod, MOD_MULT);
  normalize_buffer(buf);

  samples_to_wave_file(buf, "tone.wav", WAVE_FMT_16BIT, 1);

  delete_sample_buffer(&buf);
  delete_sample_buffer(&mod);
  return 0;
}
