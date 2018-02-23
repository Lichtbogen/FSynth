/*
 * Copyright (c) 2018 Pierre Biermann
 *
 * Permission is hereby granted and free of charge.
 * Published under the terms of the MIT license, see LICENSE file for further information.
 */

/**
 * @brief Command prompt functions
 * @author Pierre Biermann
 * @date 2017-04-09
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>

int prompt(const char *prn, char *buffer, int max_len)
{
  struct termios tmios;

  /* Get current parameter of stdin*/
  tcgetattr(STDIN_FILENO, &tmios);

  /* Remove buffering and echoing */
  tmios.c_lflag &= ~(ICANON | ECHO);

  /* Write back settings of stdin */
  tcsetattr(STDIN_FILENO, TCSANOW, &tmios);

  return 0;
}
