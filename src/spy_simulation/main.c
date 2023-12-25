#include <stdio.h>
#include "spy_simulation.c"
#include "memory.h"

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "../../include/monitor_common.h"
#include "../../include/monitor.h"

extern WINDOW *main_window;
extern int old_cursor;

int main(int argc, char **argv)
{
    int rows;
    int cols;
    int key;
    memory_t *memory;
    monitor_t *monitor;
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
    /*Initialize the spy simulation */
    create_shared_memory("SharedMemory");
    init_map(&memory->map);
    init_citizens(memory->citizens);
    init_surveillance(&memory->surveillanceNetwork);
    start_simulation_processes();
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
