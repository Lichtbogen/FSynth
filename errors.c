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
