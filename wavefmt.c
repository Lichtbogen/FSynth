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
 * @brief Simple Wave file format exporter
 * @author Pierre Biermann
 * @date 2017-03-10
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include "fsynth.h"

typedef struct {
  uint16_t wFromatTag;
  uint16_t wChannels;
  uint32_t dwSamplesPerSec;
  uint32_t dwAvgBytesPerSec;
  uint16_t wBlockAlign;
  uint16_t wBitsPerSample;
} FmtHeader;

const char riff[] = "RIFF";
const char wave[] = "WAVE";
const char fmt[]  = "fmt ";
const char data[] = "data";

void *fs_convert_samples(FSampleBuffer *buffer, int format)
{
  int idx;
  unsigned char *buffer_8bit;
  short *buffer_16bit;
  if (format == WAVE_PCM_8BIT) {
    buffer_8bit = (unsigned char*) malloc(buffer->sample_count);
    FOREACH_SAMPLE(buffer, idx) {
      buffer_8bit[idx] = (unsigned char)((buffer->samples[idx]+1) * 128);
    }
    return buffer_8bit;
  } else if (format == WAVE_PCM_16BIT) {
    buffer_16bit = (short*) malloc(buffer->sample_count * 2);
    FOREACH_SAMPLE(buffer, idx) {
      buffer_16bit[idx] = (short)(buffer->samples[idx] * 32767);
    }
    return buffer_16bit;
  } else {
    fs_set_error(FS_INVALID_ARGUMENT);
  }
  return NULL;
}

int fs_samples_to_wave_file(FSampleBuffer *buffer, const char *fname, int format, int channels)
{
  size_t fsize, fdata;
  FmtHeader fmthdr;
  void *data_ptr;

  /* Clear error flags */
  fs_clear_error();

  /* Open file in binary mode */
  FILE* fout = fopen(fname, "wb");
  if (!fout) {
    fs_set_error(FS_FILE_IO_ERROR);
    return fs_get_error();
  }

  /* Write RIFF header */
  fwrite(riff, 4, 1, fout);
  if (format == WAVE_PCM_8BIT) fdata = buffer->sample_count;
  if (format == WAVE_PCM_16BIT) fdata = buffer->sample_count * 2;
  fsize = fdata + sizeof(FmtHeader) + 16;
  fwrite(&fsize, 4, 1, fout);

  /* Write Wave and format header */
  fwrite(wave, 4, 1, fout);
  fwrite(fmt, 4, 1, fout);
  fsize = 16; /* length format header */
  fwrite(&fsize, 4, 1, fout);

  /* Fill format header */
  fmthdr.wFromatTag = 1;  /* PCM format */
  fmthdr.wChannels = channels;
  fmthdr.wBitsPerSample = format;
  fmthdr.dwSamplesPerSec = buffer->sample_rate;
  fmthdr.dwAvgBytesPerSec = format / 8 * buffer->sample_rate;
  fmthdr.wBlockAlign = fmthdr.wChannels * ((fmthdr.wBitsPerSample + 7) / 8);
  fwrite(&fmthdr, sizeof(FmtHeader), 1, fout);

  /* Write data chunk */
  data_ptr = fs_convert_samples(buffer, format);
  fwrite(data, 4, 1, fout);
  fwrite(&fdata, 4, 1, fout);
  fwrite(data_ptr, fdata, 1, fout);

  free(data_ptr);
  fclose(fout);
  return fs_get_error();
}
