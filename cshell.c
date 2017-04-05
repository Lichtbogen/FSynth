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
 * @brief Command shell interface
 * @author Pierre Biermann
 * @date 2017-04-05
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "fsynth.h"
#include "list.h"

#define MAX_PARAM      32
#define CBUFFER_SIZE   1024

struct NodeList cb_list = { NULL, NULL };

void register_shell_command(FShellCallback *fptr, const char *fname)
{
  struct NodeItem *cb = push_back(&cb_list, fptr, 0);
  cb->hash = hash_sdbm(0, fname, strlen(fname));
}

void process_command(char *cline)
{
  int ch, argc = 0;
  char *cptr = cline;
  char *argv[MAX_PARAM];

  while (*cline) {
    ch = *cline;
    if (isspace(ch)) {
      if (argc == 0) {
        *cline = 0;
        printf("%s\n", cptr);
        cptr = cline + 1;
        ++argc;
      } else {
        *cline = 0;
        printf("%s\n", cptr);
        argv[argc++] = cptr;
        cptr = cline + 1;
      }
    }
    ++cline;
  }
}

void shell_loop(void)
{
  char cbuffer[CBUFFER_SIZE];
  printf("fs> ");
  while (fgets(cbuffer, CBUFFER_SIZE, stdin) != NULL) {
    process_command(cbuffer);
    printf("fs> ");
  }
}
