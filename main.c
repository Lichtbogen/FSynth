#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fsynth.h"

int main(int argc, char **argv)
{
  int idx;
  SampleBuffer *buf = create_sample_buffer(30, 1);
  SampleBuffer *mod = create_sample_buffer(30, 1);
  generate_wave_func(mod, WAVE_SINE, 1.0, 0.5);
  generate_wave_func(buf, WAVE_SINE, 2.0, 1.0);
  modulate_buffer(buf, mod, MOD_MULT);
  normalize_buffer(buf);
  FOREACH_SAMPLE(buf, idx) {
    //printf("Sample(%d): %1.3f\n", idx, buf->samples[idx]);
    printf("%d;%1.3f\n", idx, buf->samples[idx]);
  }
  delete_sample_buffer(&buf);
  delete_sample_buffer(&mod);
  return 0;
}
