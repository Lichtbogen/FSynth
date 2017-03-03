#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fsynth.h"

int main(int argc, char **argv)
{
  SampleBuffer *buf = create_sample_buffer(30, 1);
  generate_wave_func(buf, WAVE_SINE, 2.0, 1.0);
  FOREACH_SAMPLE(buf) {
    //printf("Sample(%d): %1.3f\n", idx, buf->samples[idx]);
    printf("%d;%1.3f\n", idx, buf->samples[idx]);
  }
  delete_sample_buffer(&buf);
  return 0;
}
