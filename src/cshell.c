/*
 * Copyright (c) 2018 Pierre Biermann
 *
 * Permission is hereby granted and free of charge.
 * Published under the terms of the MIT license, see LICENSE file for further information.
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
#include <math.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "fsynth.h"
#include "logging.h"
#include "list.h"

#define MAX_PARAM      32
#define CBUFFER_SIZE   1024

#define CHECK_ARGC(x) \
  if (argc < x) { \
    fs_log(LOG_ERR, "To few arguments!"); \
    return FS_ERROR; \
  }

struct NodeList cb_list = { NULL, NULL }; /* Shell command list */
struct NodeList sb_list = { NULL, NULL }; /* Sample buffer list */

void register_shell_command(FShellCallback *fptr, const char *fname)
{
  struct NodeItem *cb = push_back(&cb_list, fptr, 0);
  cb->hash = hash_sdbm(0, fname, strlen(fname));
}

int shell_pchar(const char *cmd)
{
  int hc, result = FS_OK;
  int cb_ptr = 0, ch, argc = 0;
  char *param = NULL, *argv[MAX_PARAM];
  char cbuffer[CBUFFER_SIZE];
  FShellCallback cmdFunc;
  struct NodeItem *cbItem;
  while (1) {
    if (cmd == NULL) {
      result = FS_EXIT;
      break;
    }
    ch = (int)*cmd; ++cmd;
    if (cb_ptr == 0) {
      memset(cbuffer, 0, sizeof(cbuffer));
      param = cbuffer;
      argc = 0;
    }
    if (ch == 0 || ch == 32) {
      if (argc < MAX_PARAM)
        argv[argc++] = param;
      else
        fs_log(LOG_WARN, "Parameter limit reached");
      if (ch == 0) {
        cb_ptr = 0;
        hc = hash_sdbm(0, argv[0], strlen(argv[0]));
        cbItem = find_item(&cb_list, hc);
        if (cbItem != NULL) {
          cmdFunc = cbItem->data;
          result = (cmdFunc)(argc, argv);
        } else {
          fs_log(LOG_ERR, "Unknown command: %s", argv[0]);
          result = FS_ERROR;
        }
        break;
      }
      if (ch == 32) {
        cbuffer[cb_ptr++] = 0;
        param = &cbuffer[cb_ptr];
      }
    } else if (isprint(ch)) {
      cbuffer[cb_ptr++] = ch;
    }
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
  FSampleBuffer *sb;
  struct NodeItem *sbItem;
  CHECK_ARGC(4);
  sample_rate = atoi(argv[2]);
  duration = atof(argv[3]);
  sb = fs_create_sample_buffer(sample_rate, duration);
  sbItem = push_back(&sb_list, sb, 0);
  sbItem->hash = hash_sdbm(0, argv[1], strlen(argv[1]));
  fs_log(LOG_DEBUG, "Buffer created: %s, sample_rate: %d, duration: %f", argv[1], sample_rate, duration);
  return FS_OK;
}

FSampleBuffer *get_buffer_by_name(char *name)
{
  FSampleBuffer *sb = NULL;
  struct NodeItem *sbItem;
  int hv = hash_sdbm(0, name, strlen(name));
  sbItem = find_item(&sb_list, hv);
  if (sbItem == NULL) {
    fs_log(LOG_ERR, "Unknown buffer identifier: %s", name);
  } else {
    sb = (FSampleBuffer*) sbItem->data;
  }
  return sb;
}

int shell_cmd_func(int argc, char **argv)
{
  double amp, freq;
  FSampleBuffer *sb;
  CHECK_ARGC(4);
  sb = get_buffer_by_name(argv[1]);
  if (sb == NULL) return FS_ERROR;
  freq = atof(argv[2]);
  amp = atof(argv[3]);
  if (strcmp(argv[0], "sine") == 0) {
    fs_generate_wave_func(sb, FS_WAVE_SINE, freq, amp);
    fs_print_error(fs_get_error());
    fs_log(LOG_DEBUG, "FuncSine(%s): freq: %f, level: %f", argv[1], freq, amp);
  }
  if (strcmp(argv[0], "rect") == 0) {
    fs_generate_wave_func(sb, FS_WAVE_RECT, freq, amp);
    fs_print_error(fs_get_error());
    fs_log(LOG_DEBUG, "FuncRectangle(%s): freq: %f, level: %f", argv[1], freq, amp);
  }
  if (strcmp(argv[0], "tri") == 0) {
    fs_generate_wave_func(sb, FS_WAVE_TRIANGLE, freq, amp);
    fs_print_error(fs_get_error());
    fs_log(LOG_DEBUG, "FuncTriangle(%s): freq: %f, level: %f", argv[1], freq, amp);
  }
  if (strcmp(argv[0], "saw") == 0) {
    fs_generate_wave_func(sb, FS_WAVE_SAW, freq, amp);
    fs_print_error(fs_get_error());
    fs_log(LOG_DEBUG, "FuncSaw(%s): freq: %f, level: %f", argv[1], freq, amp);
  }
  return FS_OK;
}

int shell_cmd_wave_out(int argc, char **argv)
{
  int bits;
  FSampleBuffer *sb;
  CHECK_ARGC(4);
  bits = atoi(argv[3]);
  if (bits != 8 && bits != 16) {
    fs_log(LOG_ERR, "Invalid sample format");
    return FS_ERROR;
  }
  sb = get_buffer_by_name(argv[1]);
  if (sb != NULL) {
    fs_log(LOG_DEBUG, "WaveOut(%s): file: %s, bits: %d", argv[1], argv[2], bits);
    fs_normalize_buffer(sb);
    fs_samples_to_wave_file(sb, argv[2], bits, 1);
    fs_print_error(fs_get_error());
  }
  return FS_OK;
}

int shell_cmd_info(int argc, char **argv)
{
  FSampleBuffer *sb;
  CHECK_ARGC(1);
  sb = get_buffer_by_name(argv[1]);
  if (sb == NULL) return FS_ERROR;
  printf("buffer address:\t0x%04llX\n", (unsigned long long)sb);
  printf("sample count:\t%u\n", (unsigned int)sb->sample_count);
  printf("sample rate:\t%u\n", (unsigned int)sb->sample_rate);
  printf("buffer size:\t%llu byte\n", (unsigned long long)sb->buffer_size);
  printf("buffer length:\t%f\n", fs_get_buffer_duration(sb));
  return FS_OK;
}

int shell_cmd_repeat(int argc, char **argv)
{
  int times;
  FSampleBuffer *sb;
  CHECK_ARGC(3);
  sb = get_buffer_by_name(argv[1]);
  if (sb == NULL) return FS_ERROR;
  times = atoi(argv[2]);
  fs_repeat_sample_buffer_inplace(sb, times);
  fs_print_error(fs_get_error());
  return FS_OK;
}

int shell_cmd_scale(int argc, char **argv)
{
  double sv;
  FSampleBuffer *sb;
  CHECK_ARGC(3);
  sb = get_buffer_by_name(argv[1]);
  if (sb == NULL) return FS_ERROR;
  sv = atof(argv[2]);
  fs_scale_samples(sb, sv);
  fs_print_error(fs_get_error());
  return FS_OK;
}

int shell_cmd_attack(int argc, char **argv)
{
  double level, time;
  FSampleBuffer *sb;
  CHECK_ARGC(4);
  sb = get_buffer_by_name(argv[1]);
  if (sb == NULL) return FS_ERROR;
  time = atof(argv[3]);
  level = atof(argv[4]);
  if (strcmp(argv[0], "attack")) {
    level = fabs(level);
  }
  if (strcmp(argv[0], "decay")) {
    level = -fabs(level);
  }
  if (time > fs_get_buffer_duration(sb)) {
    fs_log(LOG_ERR, "Invalid time value: %f", time);
    return FS_ERROR;
  }
  if (strcmp(argv[2], "linear") == 0) {
    fs_log(LOG_DEBUG, "Attack linear(%s): time: %f, level:%f", argv[1], time, level);
    fs_attack_decay(sb, FS_CURVE_LINEAR, time, level);
    fs_print_error(fs_get_error());
  } else if (strcmp(argv[2], "square") == 0) {
    fs_log(LOG_DEBUG, "Attack square(%s): time: %f, level:%f", argv[1], time, level);
    fs_attack_decay(sb, FS_CURVE_SQUARE, time, level);
    fs_print_error(fs_get_error());
  } else if (strcmp(argv[2], "tan") == 0) {
    fs_log(LOG_DEBUG, "Attack tan(%s): time: %f, level:%f", argv[1], time, level);
    fs_attack_decay(sb, FS_CURVE_TAN, time, level);
    fs_print_error(fs_get_error());
  } else if (strcmp(argv[2], "cubic") == 0) {
    fs_log(LOG_DEBUG, "Attack cubic(%s): time: %f, level:%f", argv[1], time, level);
    fs_attack_decay(sb, FS_CURVE_CUBIC, time, level);
    fs_print_error(fs_get_error());
  }
  return FS_OK;
}

int shell_cmd_sustain(int argc, char **argv)
{
  double time;
  FSampleBuffer *sb;
  CHECK_ARGC(2);
  sb = get_buffer_by_name(argv[1]);
  if (sb != NULL) {
    time = atof(argv[2]);
    fs_sustain(sb, time);
    fs_log(LOG_DEBUG, "Sustain(%s): time: %f", argv[1], time);
    fs_print_error(fs_get_error());
  }
  return FS_OK;
}

int shell_cmd_mod(int argc, char **argv)
{
  FSampleBuffer *sb1, *sb2;
  CHECK_ARGC(3);
  sb1 = get_buffer_by_name(argv[1]);
  sb2 = get_buffer_by_name(argv[2]);
  if (sb1 != NULL || sb2 != NULL) {
    if (strcmp(argv[0], "mult") == 0) {
      fs_log(LOG_DEBUG, "Multiply(%s, %s)", argv[1], argv[2]);
      fs_modulate_buffer(sb1, sb2, FS_MOD_MULT);
      fs_print_error(fs_get_error());
    }
    if (strcmp(argv[0], "div") == 0) {
      fs_log(LOG_DEBUG, "Divide(%s, %s)", argv[1], argv[2]);
      fs_modulate_buffer(sb1, sb2, FS_MOD_DIV);
      fs_print_error(fs_get_error());
    }
    if (strcmp(argv[0], "add") == 0) {
      fs_log(LOG_DEBUG, "Add(%s, %s)", argv[1], argv[2]);
      fs_modulate_buffer(sb1, sb2, FS_MOD_ADD);
      fs_print_error(fs_get_error());
    }
    if (strcmp(argv[0], "sub") == 0) {
      fs_log(LOG_DEBUG, "Subtract(%s, %s)", argv[1], argv[2]);
      fs_modulate_buffer(sb1, sb2, FS_MOD_SUB);
      fs_print_error(fs_get_error());
    }
    if (strcmp(argv[0], "cat") == 0) {
      fs_log(LOG_DEBUG, "Concat(%s, %s)", argv[1], argv[2]);
      fs_cat_sample_buffers_inplace(sb1, sb2);
      fs_print_error(fs_get_error());
    }
  } else {
    if (sb1 == NULL) {
      fs_log(LOG_ERR, "Invalid buffer identifier: %s", argv[1]);
    }
    if (sb2 == NULL) {
      fs_log(LOG_ERR, "Invalid buffer identifier: %s", argv[2]);
    }
  }
  return FS_OK;
}

int shell_cmd_help(int argc, char **argv)
{
  if (argc == 1) {
    printf("Available commands:\n");
    printf("\tbuffer\tCreates a new sample buffer object\n");
    printf("\tsine\tGenerates a sine wave form\n");
    printf("\trect\tGenerates a rectangle wave form\n");
    printf("\ttri\tGenerates a triangle wave form\n");
    printf("\tsaw\tGenerates a saw tooth wave form\n");
    printf("\thelp\tGet help in generel or for a specific command\n");
    printf("\texit\tExit FSynth\n");
    printf("\twaveout\tWrites the buffer content to a WAVE file\n");
    printf("\tmult\tMultiplies the content of two buffers\n");
    printf("\tdiv\tDivides the content of two buffers\n");
    printf("\tadd\tAdds the content of two buffers\n");
    printf("\tsub\tSubtracts the content of two buffers\n");
    printf("\tcat\tConcats the content of two buffers\n");
    printf("\trepeat\tRepeats the content of an sample buffer n-times\n");
    printf("\tscale\tScales the samples of a given buffer object\n");
    printf("\tinfo\tProvides detailed information about the given object\n");
    printf("\tattack\tAdds an 'attack' hull curve to the output buffer\n");
    printf("\tdecay\tAdds an 'decay' hull curve to the output buffer\n");
    printf("\tsustain\tAdds an 'sustain' hull curve to the output buffer\n");
    printf("\nType help [command] to get help for a specific command\n");
  } else {
    if (strcmp(argv[1], "buffer") == 0) {
      printf("Creates a new sample buffer object\n");
      printf("with given sample rate and playing duration\n");
      printf("usage: buffer <buffer_name> <sample_rate> <duration>\n");
    }
    if (strcmp(argv[1], "sine") == 0) {
      printf("Generates a sine wave form\n");
      printf("usage: sine <buffer_name> <frequency> <amplitude>\n");
    }
    if (strcmp(argv[1], "rect") == 0) {
      printf("Generates a rectangle wave form\n");
      printf("usage: rect <buffer_name> <frequency> <amplitude>\n");
    }
    if (strcmp(argv[1], "tri") == 0) {
      printf("Generates a triangle wave form\n");
      printf("usage: tri <buffer_name> <frequency> <amplitude>\n");
    }
    if (strcmp(argv[1], "saw") == 0) {
      printf("Generates a saw tooth wave form\n");
      printf("usage: saw <buffer_name> <frequency> <amplitude>\n");
    }
    if (strcmp(argv[1], "mult") == 0) {
      printf("Multiplies the content of two sample buffers\n");
      printf("and store the result back into the first one\n");
      printf("usage: mult <buffer_name_1> <buffer_name_2>\n");
    }
    if (strcmp(argv[1], "div") == 0) {
      printf("Divides the content of two sample buffers\n");
      printf("and store the result back into the first one\n");
      printf("usage: div <buffer_name_1> <buffer_name_2>\n");
    }
    if (strcmp(argv[1], "add") == 0) {
      printf("Adds the content of two sample buffers\n");
      printf("and store the result back into the first one\n");
      printf("usage: add <buffer_name_1> <buffer_name_2>\n");
    }
    if (strcmp(argv[1], "sub") == 0) {
      printf("Subtracts the content of two sample buffers\n");
      printf("and store the result back into the first one\n");
      printf("usage: sub <buffer_name_1> <buffer_name_2>\n");
    }
    if (strcmp(argv[1], "sub") == 0) {
      printf("Concats the content of two sample buffers\n");
      printf("and store the combined content back into the first one\n");
      printf("usage: sub <buffer_name_1> <buffer_name_2>\n");
    }
    if (strcmp(argv[1], "repeat") == 0) {
      printf("Repeats an sample buffer n-times\n");
      printf("usage: repeat <buffer_name> <times>\n");
    }
    if (strcmp(argv[1], "scale") == 0) {
      printf("Scales the samples of a given buffer object\n");
      printf("usage: scale <buffer_name> <scale_value>\n");
    }
    if (strcmp(argv[1], "info") == 0) {
      printf("Provides detailed information about the given object\n");
      printf("usage: info [object_name]\n");
    }
    if (strcmp(argv[1], "attack") == 0) {
      printf("Adds an 'attack' hull curve to the given buffer\n");
      printf("usage: attck <buffer_name> <hull_type{linear|tan|cubic> <time> <level>\n");
    }
    if (strcmp(argv[1], "decay") == 0) {
      printf("Adds an 'decay' hull curve to the given buffer\n");
      printf("usage: attck <buffer_name> <hull_type{linear|tan|cubic}> <time> <level>\n");
    }
    if (strcmp(argv[1], "sustain") == 0) {
      printf("Adds an 'sustain' hull curve to the given buffer\n");
      printf("usage: attck <buffer_name> <time>\n");
    }
  }
  return FS_OK;
}

void shell_register(void)
{
  register_shell_command((FShellCallback*)&shell_cmd_exit, "exit");
  register_shell_command((FShellCallback*)&shell_cmd_buffer, "buffer");
  register_shell_command((FShellCallback*)&shell_cmd_func, "sine");
  register_shell_command((FShellCallback*)&shell_cmd_func, "rect");
  register_shell_command((FShellCallback*)&shell_cmd_func, "tri");
  register_shell_command((FShellCallback*)&shell_cmd_func, "saw");
  register_shell_command((FShellCallback*)&shell_cmd_wave_out, "waveout");
  register_shell_command((FShellCallback*)&shell_cmd_help, "help");
  register_shell_command((FShellCallback*)&shell_cmd_mod, "mult");
  register_shell_command((FShellCallback*)&shell_cmd_mod, "div");
  register_shell_command((FShellCallback*)&shell_cmd_mod, "add");
  register_shell_command((FShellCallback*)&shell_cmd_mod, "sub");
  register_shell_command((FShellCallback*)&shell_cmd_mod, "cat");
  register_shell_command((FShellCallback*)&shell_cmd_repeat, "repeat");
  register_shell_command((FShellCallback*)&shell_cmd_scale, "scale");
  register_shell_command((FShellCallback*)&shell_cmd_info, "info");
  register_shell_command((FShellCallback*)&shell_cmd_attack, "attack");
  register_shell_command((FShellCallback*)&shell_cmd_attack, "decay");
  register_shell_command((FShellCallback*)&shell_cmd_sustain, "sustain");
}

void shell_cleanup(void)
{
  struct NodeItem *item = sb_list.head;
  while (item != NULL) {
    fs_delete_sample_buffer((FSampleBuffer**)&item->data);
    item = item->next;
  }
  delete_list(&sb_list);
  delete_list(&cb_list);
}

void shell_loop(void)
{
  char *input;
  do {
    input = readline("fs>");
    add_history(input);
    if (shell_pchar(input) & FS_EXIT) break;
  } while (1);
}
