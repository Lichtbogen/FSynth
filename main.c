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
 * @brief Main module
 * @author Pierre Biermann
 * @date 2017-04-05
 */

#include <stdio.h>

#include "fsynth.h"

void shell_loop();
void shell_cleanup();
void shell_register();

int prompt(const char *prn, char *buffer, int max_len);

int main(int argc, char **argv)
{
  //char buf[1024];
#ifndef DEBUG
  printf("FSynth: version %s build on %s %s\n", FS_VERSION, __DATE__, __TIME__);
#else
  printf("FSynth: version %s DEBUG build on %s %s\n", FS_VERSION, __DATE__, __TIME__);
#endif
  printf("GCC: %s\n", __VERSION__);
  shell_register();
  shell_loop();
  //prompt("FS> ", buf, sizeof(buf));
  shell_cleanup();
  return 0;
}
