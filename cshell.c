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

int exit_flag = 0;
struct NodeList cb_list = { NULL, NULL }; /* Shell command list */
struct NodeList sb_list = { NULL, NULL }; /* Sample buffer list */

void register_shell_command(FShellCallback *fptr, const char *fname)
{
  struct NodeItem *cb = push_back(&cb_list, fptr, 0);
  cb->hash = hash_sdbm(0, fname, strlen(fname));
}

void process_command(char *cline)
{
  int ch, hc, argc = 0;
  char *cptr = cline;
  char *argv[MAX_PARAM];
  FShellCallback cmdFunc;
  struct NodeItem *cbItem;
  while (*cline) {
    ch = *cline;
    if (isspace(ch)) {
      if (argc == 0) {
        *cline = 0;
        hc = hash_sdbm(0, cptr, strlen(cptr));
        cbItem = find_item(&cb_list, hc);
        argv[argc++] = cptr;
        if (cbItem != NULL) {
          cmdFunc = cbItem->data;
          (cmdFunc)(argc, argv);
        }
        cptr = cline + 1;
      } else {
        *cline = 0;
        argv[argc++] = cptr;
        cptr = cline + 1;
      }
    }
    ++cline;
  }
}

int shell_cmd_exit(int argc, char **argv)
{
  exit_flag = 1;
  return FS_OK;
}

int shell_cmd_buffer(int argc, char **argv)
{
  return FS_OK;
}

void shell_register(void)
{
  register_shell_command((FShellCallback*)&shell_cmd_exit, "exit");
}

void shell_cleanup(void)
{
  delete_list(&cb_list);
}

void shell_loop(void)
{
  char cbuffer[CBUFFER_SIZE];
  printf("fs> ");
  while (fgets(cbuffer, CBUFFER_SIZE, stdin) != NULL) {
    process_command(cbuffer);
    if (exit_flag) break;
    printf("fs> ");
  }
}
