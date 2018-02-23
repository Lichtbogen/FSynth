/*
 * Copyright (c) 2018 Pierre Biermann
 *
 * Permission is hereby granted and free of charge.
 * Published under the terms of the MIT license, see LICENSE file for further information.
 */

/**
 * @brief Logging module
 * @author Pierre Biermann
 * @date 2017-04-06
 */

#include <stdio.h>
#include <stdarg.h>
#include "logging.h"

#ifndef DEBUG
int log_level = LOG_INFO | LOG_WARN | LOG_ERR;
#else
int log_level = LOG_INFO | LOG_WARN | LOG_ERR | LOG_DEBUG;
#endif

void fs_set_log_level(int level)
{
  log_level = level;
}

int fs_get_log_level(void)
{
  return log_level;
}

void fs_log(int level, const char *format, ...)
{
  va_list args;
  if ((log_level & LOG_ERR) && (level == LOG_ERR)) {
    fprintf(stderr, "Error: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
    va_end(args);
  }
  if ((log_level & LOG_WARN) && (level == LOG_WARN)) {
    fprintf(stderr, "Warning: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
    va_end(args);
  }
  if ((log_level & LOG_INFO) && (level == LOG_INFO)) {
    fprintf(stderr, "Info: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
    va_end(args);
  }
  if ((log_level & LOG_DEBUG) && (level == LOG_DEBUG)) {
    fprintf(stderr, "Debug: ");
    va_start(args, format);
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
    va_end(args);
  }
}
