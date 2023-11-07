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
#ifndef MONITOR_H
#define MONITOR_H

/**
 * \file monitor.h
 *
 * \brief Constants types and prototypes of the NCurses Terminal User Interface.
 */

#include <ncurses.h>

#include "memory.h"

/** The number of fields to display for a spy. */
#define NB_OF_INFORMATION_FIELDS 8

struct monitor_s {
    int has_to_update;
};

/**
 * \brief Creates pairs of colors (see "NCurses" documentation).
 *
 * Basic available colors in ncurses are:
 * - COLOR_BLACK
 * - COLOR_RED
 * - COLOR_GREEN
 * - COLOR_YELLOW
 * - COLOR_BLUE
 * - COLOR_MAGENTA
 * - COLOR_CYAN
 * - COLOR_WHITE
 */
void create_color_pairs();

/**
 * \brief Creates the four sub-window boxes within the given window.
 *
 * \param window The main window in which we create the four boxes.
 * \param mem The given shared memory.
 * \param rows The number of rows of the main window.
 * \param columns The number of columns of the main window.
 */
void init_monitor_elements(WINDOW *window, memory_t *mem, int rows, int columns);

/**
 * \brief Sets the given window with the given title.
 *
 * \param window The window.
 * \param title The title.
 */
void set_monitor_title(WINDOW *window, const char *title);

/**
 * \brief Detects and displays if the current version of NCurses supports mouse
 * or not.
 */
void detect_mouse();

/**
 * \brief Sets the city map legend within the given sub-window ("city map and
 * general information" sub_window).
 *
 * \param window The sub-window.
 * \param row The row of the top of city legend
 * \param col The column of the left of city legend
 */
void set_city_legend(WINDOW *window, int row, int col);

/**
 * \brief Sets a cell from the city map with the given color (the city map is
 * displayed in the "City map and status information" sub-window).
 *
 * \param window The sub-window.
 * \param color The color of the cell.
 * \param row The row of the cell.
 * \param The column of the cell.
 */
void set_cell_color(WINDOW *window, int color, int row, int col);

/**
 * \brief Displays the city map within the "City map and status information"
 * sub-window.
 *
 * \param window The sub-window.
 * \param map The map structure that defines cells.
 * \param rows The number of rows of the city map.
 * \param columns The number of columns of the city map.
 */
void display_city(WINDOW *window, map_t map, int rows, int columns);

/**
 * \brief Shows information within the "City map and status information"
 * sub-window.
 *
 * It displays information such as the title and labels of the number of turns
 * and the elapsed time.
 *
 * \param window The sub-window.
 */
void show_general_information(WINDOW *window);

/**
 * \brief Displays the number of steps and the elapsed time such as an
 * ending message within the "City map and status information" sub-window.
 *
 * \param window The sub-window.
 * \param mem The given shared memory.
 */
void display_general_information_values(WINDOW *window, memory_t *mem);

/**
 * \brief Updates values after consulting the given memory.
 *
 * \param mem The given shared memory.
 */
void update_values(memory_t *mem);

/**
 * \brief Displays the citizen information starting at the given row and column.
 *
 * \param window The sub-window.
 * \param mem The given shared memory.
 * \param row The row to start.
 * \param column The column to start.
 */
void display_citizen_information(WINDOW *window, memory_t *mem, int row, int column);

/**
 * \brief Displays the information of the spy whose number is given starting
 * at the given row and column.
 *
 * \param window The sub-window.
 * \param mem The given shared memory.
 * \param row The row to start.
 * \param column The column to start.
 * \param number The number of the spy (0, 1 or 2)
 */
void display_spy_information(WINDOW *window, memory_t *mem, int row, int column, int number);

/**
 * \brief Displays the information of case officer starting at the given row
 * and column.
 *
 * \param window The sub-window.
 * \param mem The given shared memory.
 * \param row The row to start.
 * \param column The column to start.
 */
void display_case_officer_information(WINDOW *window, memory_t *mem, int row, int column);

/**
 * \brief Displays the information of the counter officer starting at the
 * given row and column.
 *
 * \param window The sub-window.
 * \param mem The given shared memory.
 * \param row The row to start.
 * \param column The column to start.
 */
void display_counter_officer_information(WINDOW *window, memory_t *mem, int row, int col);

/**
 * \brief Displays all characters information.
 *
 * The function calls:
 * - \fn display_citizen_information();
 * - \fn display_spy_information();
 * - \fn display_case_officer_information();
 * - \fn display_counter_officer_information().
 *
 * \param window The sub-window.
 * \param mem The given shared memory.
 */
void display_character_information(WINDOW *window, memory_t *mem);

/**
 * \brief Displays the mailbox content. Messages are encrypted.
 *
 * \param window The sub-window in which encrypted messages are displayed.
 * \param mem The given shared memory in which encrypted messages from the 
 *            mailbox are retrieved.
 */
void display_mailbox_content(WINDOW *window, memory_t *mem);

/**
 * \brief Displays messages received by the enemy country.
 *
 * \param window The sub-window in which messages are displayed.
 */
void display_enemy_country_monitor(WINDOW *window);

#endif /* MONITOR_H */
