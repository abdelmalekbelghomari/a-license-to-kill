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
#include <pthread.h>
#include <locale.h>

#include "monitor_common.h"
#include "monitor.h"
#include "memory.h"
#include "spy_simulation.h"
#include "timer.h"

extern WINDOW *main_window;
extern int old_cursor;
sem_t *sem_producer, *sem_consumer, *sem_spy_producer, *sem_spy_consumer, *sem_memory;


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
    shm = shm_open("/SharedMemory", O_RDWR, 0666);
    if (shm == -1) {
        perror("shm_open error");
        exit(EXIT_FAILURE);
    }

    memory = (memory_t *)mmap(NULL, sizeof(memory_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    if (memory == MAP_FAILED) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    /* ---------------------------------------------------------------------- */
    /*Initialisation des sémaphores                                           */

    sem_producer = sem_open("/semProducer", 0);
    if (sem_producer == SEM_FAILED) {
        perror("sem_open monitor");
        exit(EXIT_FAILURE);
    }
    sem_consumer = sem_open("/semConsumer", 0);
    if (sem_consumer == SEM_FAILED) {
        perror("sem_open monitor");
        exit(EXIT_FAILURE);
    }
    sem_spy_producer = sem_open("/semSpyProducer", 0);
    if (sem_spy_producer == SEM_FAILED) {
        perror("sem_open monitor");
        exit(EXIT_FAILURE);
    }
    sem_spy_consumer = sem_open("/semSpyConsumer", 0);
    if (sem_spy_consumer == SEM_FAILED) {
        perror("sem_open monitor");
        exit(EXIT_FAILURE);
    }
    sem_memory = sem_open("/semMemory", 0);
    if (sem_memory == SEM_FAILED) {
        perror("sem_open failed in monitor");
        exit(EXIT_FAILURE);
    }

    close(shm);
    /* ---------------------------------------------------------------------- */ 
    monitor = (monitor_t *)malloc(sizeof(monitor_t));
    monitor->has_to_update = 0;
    set_timer();
    set_signals();

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
    int statusSharedMemory;

    /*  Loop and get user input  */
    while (1) {
        key = getch();

        switch (key) {
            case 'Q':
                

                // Replace 'file_name.txt' with the name of the file you want to delete
                statusSharedMemory = remove("/dev/shm/SharedMemory");

                if (statusSharedMemory == 0)
                    printf("File deleted successfully\n");
                else
                    printf("Error: unable to delete the file\n");

                return 0;
            case 'q':

                // Replace 'file_name.txt' with the name of the file you want to delete
                statusSharedMemory = remove("/dev/shm/SharedMemory");

                if (statusSharedMemory == 0)
                    printf("File deleted successfully\n");
                else
                    printf("Error: unable to delete the file\n");

                return 0;
            case 27:

                // Replace 'file_name.txt' with the name of the file you want to delete
                statusSharedMemory = remove("/dev/shm/SharedMemory");

                if (statusSharedMemory == 0)
                    printf("File deleted successfully\n");
                else
                    printf("Error: unable to delete the file\n");
                quit_nicely(NO_PARTICULAR_REASON);
            default:
                break;
        }
        // int sval;
        // sem_getvalue(sem_memory, &sval);
        // printf("Current value of sem_memory before wait: %d\n", sval);
        sem_wait(sem_memory);
        // sem_getvalue(sem_memory, &sval);
        // printf("Current value of sem_memory after wait: %d\n", sval);  
        if (memory->memory_has_changed) {
            update_values(memory);
            memory->memory_has_changed = 0;
        } 
        sem_post(sem_memory);
        // sem_getvalue(sem_memory, &sval);
        // printf("Current value of sem_memory after post: %d\n", sval);("Value of sem_memory after post: %d\n", sem_memory->__align);

    }
    sem_close(sem_consumer);
    sem_close(sem_producer);
    sem_close(sem_spy_consumer);
    sem_close(sem_spy_producer);
    sem_close(sem_memory);

}

