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
