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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "fsynth.h"
#include "logging.h"
#include "list.h"

#define MAX_PARAM      32
#define CBUFFER_SIZE   1024

int cb_ptr = 0;
int argc = 0;
char *param;
char *argv[MAX_PARAM];
char cbuffer[CBUFFER_SIZE];

struct NodeList cb_list = { NULL, NULL }; /* Shell command list */
struct NodeList sb_list = { NULL, NULL }; /* Sample buffer list */

void register_shell_command(FShellCallback *fptr, const char *fname)
{
  struct NodeItem *cb = push_back(&cb_list, fptr, 0);
  cb->hash = hash_sdbm(0, fname, strlen(fname));
}

int shell_pchar(int ch)
{
  int hc, result = FS_OK;
  FShellCallback cmdFunc;
  struct NodeItem *cbItem;
  if (cb_ptr == 0) {
    memset(cbuffer, 0, sizeof(cbuffer));
    param = cbuffer;
    argc = 0;
  }
  if (ch == 10 || ch == 13 || ch == 32) {
    if (argc < MAX_PARAM)
      argv[argc++] = param;
    else
      fs_log(LOG_WARN, "Parameter limit reached");
    if (ch == 10 || ch == 13) {
      cb_ptr = 0;
      hc = hash_sdbm(0, argv[0], strlen(argv[0]));
      cbItem = find_item(&cb_list, hc);
      if (cbItem != NULL) {
        cmdFunc = cbItem->data;
        result = (cmdFunc)(argc, argv);
      }
    }
    if (ch == 32) {
      cbuffer[cb_ptr++] = 0;
      param = &cbuffer[cb_ptr];
    }
  } else if (isprint(ch)) {
    cbuffer[cb_ptr++] = ch;
  }
  return result;
}

int shell_cmd_exit(int argc, char **argv)
{
  return FS_EXIT;
}

int shell_cmd_buffer(int argc, char **argv)
{
  uint32_t sample_rate;
  double duration;
  if (argc < 4) {
    fs_log(LOG_ERR, "To few arguments!");
    return FS_ERROR;
  }
  sample_rate = atoi(argv[2]);
  duration = atof(argv[3]);
  fs_log(LOG_DEBUG, "Buffer created: %s, sample_rate: %d, duration: %f", argv[1], sample_rate, duration);
  return FS_OK;
}

void shell_register(void)
{
  register_shell_command((FShellCallback*)&shell_cmd_exit, "exit");
  register_shell_command((FShellCallback*)&shell_cmd_buffer, "buffer");
}

void shell_cleanup(void)
{
  delete_list(&cb_list);
}

void shell_loop(void)
{
  int ch;
  do {
    if (cb_ptr == 0)
      printf("fs> ");
    ch = getchar();
    if (shell_pchar(ch) & FS_EXIT) break;
  } while (ch != -1);
}
