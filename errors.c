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
 * @brief Functions for basic error handling
 * @author Pierre Biermann
 * @date 2017-03-10
 */

#include <stdio.h>
#include "fsynth.h"

int error_code = 0;

void fs_set_error(int code)
{
  error_code = error_code | FS_ERROR | code;
}

void fs_set_warning(int code)
{
  error_code = error_code | FS_WARNING | code;
}

int fs_get_error(void)
{
  return error_code;
}

int fs_clear_error(void)
{
  error_code = FS_OK;
  return error_code;
}

void fs_print_error(int code)
{
  if (code & FS_ERROR) {
    printf("FS Error: ");
    if (code & FS_INVALID_BUFFER) {
      printf("Invalid buffer instance\n");
    }
    if (code & FS_INVALID_ARGUMENT) {
      printf("Invalid argument\n");
    }
    if (code & FS_INVALID_OPERATION) {
      printf("Invalid operation\n");
    }
    if (code & FS_DIVIDED_BY_ZERO) {
      printf("Division by zero\n");
    }
    if (code & FS_FILE_IO_ERROR) {
      printf("File IO error\n");
    }
    if (code & FS_WRONG_BUF_SIZE) {
      printf("Wrong buffer size\n");
    }
    if (code & FS_DIFF_SAMPLE_RATE) {
      printf("Input buffers with different sample rates\n");
    }
    if (code & FS_INDEX_OUT_OF_RANGE) {
      printf("Buffer index out of range\n");
    }
  }
}
