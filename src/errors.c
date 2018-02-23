/*
 * Copyright (c) 2018 Pierre Biermann
 *
 * Permission is hereby granted and free of charge.
 * Published under the terms of the MIT license, see LICENSE file for further information.
 */

/**
 * @brief Functions for basic error handling
 * @author Pierre Biermann
 * @date 2017-03-10
 */

#include <stdio.h>
#include "fsynth.h"
#include "logging.h"

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
    if (code & FS_INVALID_BUFFER) {
      fs_log(LOG_ERR, "Invalid buffer instance");
    }
    if (code & FS_INVALID_ARGUMENT) {
      fs_log(LOG_ERR, "Invalid argument");
    }
    if (code & FS_INVALID_OPERATION) {
      fs_log(LOG_ERR, "Invalid operation");
    }
    if (code & FS_DIVIDED_BY_ZERO) {
      fs_log(LOG_ERR, "Division by zero");
    }
    if (code & FS_FILE_IO_ERROR) {
      fs_log(LOG_ERR, "File IO error");
    }
    if (code & FS_WRONG_BUF_SIZE) {
      fs_log(LOG_ERR, "Wrong buffer size");
    }
    if (code & FS_DIFF_SAMPLE_RATE) {
      fs_log(LOG_ERR, "Input buffers with different sample rates\n");
    }
    if (code & FS_INDEX_OUT_OF_RANGE) {
      fs_log(LOG_ERR, "Buffer index out of range\n");
    }
  }
}
