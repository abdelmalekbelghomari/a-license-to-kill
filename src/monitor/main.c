/*
 * The License to Kill Project
 *
 * Copyright (C) 2021 Alain Lebret <alain.lebret [at] ensicaen [dot] fr>
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "monitor_common.h"
#include "monitor.h"
#include "memory.h"

extern WINDOW *main_window;
extern int old_cursor;

/**
 * \file main.c
 *
 * \brief Creates a new "Terminal User Interface" using ncurses and associated
 * with the given shared memory.
 *
 * The size of the terminal must have at least 40 rows and 145 columns. It
 * is decomposed in four sub-windows:
 * - The "city map and status" window (upper-left)
 * - The "character information" window (upper-right)
 * - The "mailbox content" window (bottom-left)
 * - The "enemy country monitor" window (bottom-right)
 * 'Q', 'q' and 'Esc' keys are used to exit from the TUI.
 */
int main()
{
    int rows;
    int cols;
    int key;
    int shm;

    memory_t *memory;
    monitor_t *monitor;

    /* ---------------------------------------------------------------------- */ 
    /* The following code only allows to avoid segmentation fault !           */ 
    /* Change it to access to the real shared memory.                         */
    shm = shm_open("SharedMemory", O_RDONLY, 0666);
    if (shm == -1) {
        perror("shm_open error");
        exit(EXIT_FAILURE);
    }

    memory = (memory_t *)mmap(NULL, sizeof(memory_t), PROT_READ, MAP_SHARED, shm, 0);
    if (memory == MAP_FAILED) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }
    // Après avoir mappé la mémoire partagée
    if (memory != MAP_FAILED) {
        printf("Memory mapping successful.\n");

        printf("Memory has changed: %d\n", memory->memory_has_changed);
        printf("Simulation has ended: %d\n", memory->simulation_has_ended);
        
        // Afficher des informations sur la carte
        printf("Map details:\n");
        for (int i = 0; i < MAX_ROWS; i++) {
            for (int j = 0; j < MAX_COLUMNS; j++) {
                printf("Cell[%d][%d] Type: %d, Capacity: %d\n", i, j, memory->map.cells[i][j].type, memory->map.cells[i][j].current_capacity);
            }
        }

        // Afficher des informations sur les espions
        printf("Spy details:\n");
        for (int i = 0; i < 3; i++) {
            printf("Spy %d - Health: %d, Location: (%d, %d)\n", i, memory->spies[i].health_point, memory->spies[i].location_row, memory->spies[i].location_column);
        }

        // Afficher des informations sur l'officier de cas
        printf("Case Officer - Health: %d, Location: (%d, %d)\n", memory->case_officer.health_point, memory->case_officer.location_row, memory->case_officer.location_column);

        // Afficher des informations sur l'officier du contre-espionnage
        printf("Counterintelligence Officer - Health: %d, Location: (%d, %d)\n", memory->counterintelligence_officer.health_point, memory->counterintelligence_officer.location_row, memory->counterintelligence_officer.location_column);

        // Afficher des informations sur les citoyens
        printf("Citizen details:\n");
        for (int i = 0; i < NB_CITIZEN; i++) {
            printf("Citizen %d - Type: %d, Health: %d, Location: (%d, %d)\n", i, memory->citizens[i].type, memory->citizens[i].health, memory->citizens[i].position[0], memory->citizens[i].position[1]);
        }

        // Afficher des informations sur le réseau de surveillance
        printf("Surveillance network status:\n");
        for (int i = 0; i < MAX_ROWS; i++) {
            for (int j = 0; j < MAX_COLUMNS; j++) {
                printf("Cell[%d][%d] - Standard camera: %d, Infrared camera: %d, Lidar: %d\n", i, j, memory->surveillanceNetwork.devices[i][j].standard_camera, memory->surveillanceNetwork.devices[i][j].infrared_camera, memory->surveillanceNetwork.devices[i][j].lidar);
            }
        }
        if(memory->memory_has_changed){
            printf("Memory has changed: %d\n", memory->memory_has_changed);
            printf("Timer: %d:%d\n", memory->timer.hours, memory->timer.minutes);
        }

    } else {
        perror("Memory mapping failed");
    }

    close(shm);
    /* ---------------------------------------------------------------------- */ 

    monitor = (monitor_t *)malloc(sizeof(monitor_t));
    monitor->has_to_update = 0;

    set_timer();
    set_signals();

    if ((main_window = initscr()) == NULL) {
        quit_after_error("Error initializing library ncurses!");
    }

    clear();                  /* Start from an empty screen. */
    cbreak();                 /* No line buffering, pass every key pressed. */
    noecho();                 /* Do not echo the keyboard input. */
    old_cursor = curs_set(0); /* Use and invisible cursor. */
    keypad(stdscr, TRUE);     /* Allows functions keys, arrows, etc. */

    start_color();            /* Allow using colors... */
    create_color_pairs();     /* ... and create color pairs to use */

    if (!is_terminal_size_larger_enough(&rows, &cols)) {
        quit_after_error("Minimal terminal dimensions: 45 rows and 140 columns!");
    }
    /*Initialize the spy simulation*/
    /* Initialize terminal user interface elements */
    init_monitor_elements(main_window, memory, rows, cols);

    /*  Loop and get user input  */
    while (true) {
        key = getch();

        switch (key) {
            case 'Q':
            case 'q':
            case 27:
                quit_nicely(NO_PARTICULAR_REASON);
            default:
                break;
        }

        if (memory->memory_has_changed) {
            update_values(memory);
            memory->memory_has_changed = 0;
        }

    }

}

