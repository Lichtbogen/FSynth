/*
 * Copyright (c) 2018 Pierre Biermann
 *
 * Permission is hereby granted and free of charge.
 * Published under the terms of the MIT license, see LICENSE file for further information.
 */

/**
 * @brief Main module
 * @author Pierre Biermann
 * @date 2017-04-05
 */

#include <stdio.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "fsynth.h"

void shell_cleanup();
void shell_register();
int shell_pchar(const char *cmd);

int prompt(const char *prn, char *buffer, int max_len);

int main(int argc, char **argv)
{
  char *input;
#ifndef DEBUG
  printf("FSynth: version %s build on %s %s\n", FS_VERSION, __DATE__, __TIME__);
#else
  printf("FSynth: version %s DEBUG build on %s %s\n", FS_VERSION, __DATE__, __TIME__);
#endif
  printf("GCC: %s\n", __VERSION__);
  shell_register();
  do {
    input = readline("fs>");
    add_history(input);
    if (shell_pchar(input) & FS_EXIT) break;
  } while (1);
  shell_cleanup();
  return 0;
}
