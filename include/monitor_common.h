/*
 * The License to Kill Project
 *
 * Copyright (C) 1995-2022 Alain Lebret <alain.lebret [at] ensicaen [dot] fr>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MONITOR_COMMON_H
#define MONITOR_COMMON_H

/**
 * \file monitor_common.h
 *
 * \brief Constants and prototypes of some NCurses functions.
 */

#define TIME_STEP 200000
#define NO_PARTICULAR_REASON -1

typedef struct monitor_s monitor_t;

/**
 * \brief Quit nicely after an error occurred.
 *
 * \param msg The message to display before quitting.
 */
void quit_after_error(const char *msg);

/**
 * \brief Quit nicely.
 *
 * \param reason The reason to quit.
 */
void quit_nicely(int reason);

/**
 * \brief Clears a specific line in the given window.
 *
 * \param window The sub-window.
 * \param row The row of the line to clear
 */
void clear_line(WINDOW *window, int row);

/**
 * \brief Returns the size of the terminal.
 *
 * \param rows The number of rows.
 * \param cols The number of columns.
 */
void get_terminal_size(int *rows, int *cols);

/**
 *
 * \param rows The number of rows.
 * \param cols The number of columns.
 * @return
 */
int is_terminal_size_larger_enough(int *rows, int *cols);

/**
 * \brief Sets up the timer using the TIME_STEP value.
 */
void set_timer(void);

/**
 * \brief Defines signal handler for the monitor (used by \fn set_signals()).
 *
 * \param signum The signal number.
 */
void handler(int signum);

/**
 * \brief Sets up the signal handler.
 */
void set_signals(void);

#endif /* MONITOR_COMMON_H */
