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

#define LOG_DEBUG     0x01
#define LOG_INFO      0x02
#define LOG_WARN      0x04
#define LOG_ERR       0x08

void fs_log(int level, const char *fromat, ...);
void fs_set_log_level(int level);
int fs_get_log_level(void);
