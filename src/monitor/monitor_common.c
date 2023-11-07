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
#include <signal.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <ncurses.h>

#include "monitor_common.h"
#include "memory.h"

extern WINDOW *main_window;
extern WINDOW *city_window;
extern WINDOW *character_window;
extern WINDOW *mailbox_content_window;
extern WINDOW *enemy_country_monitor;

extern int old_cursor;
memory_t *mem;

void quit_after_error(const char *msg)
{
    delwin(city_window);
    delwin(character_window);
    delwin(mailbox_content_window);
    delwin(enemy_country_monitor);
    delwin(main_window);
    curs_set(old_cursor);
    endwin();
    refresh();
	
    /* Do not forget to clean shared memory calling a function*/
	/* cleanup_memory(mem); */

    perror(msg);

    exit(EXIT_FAILURE);
}

void quit_nicely(int reason)
{
    delwin(city_window);
    delwin(character_window);
    delwin(mailbox_content_window);
    delwin(enemy_country_monitor);
    delwin(main_window);
    curs_set(old_cursor);
    endwin();
    refresh();

    /* Do not forget to clean shared memory calling a function*/
	/* cleanup_memory(mem); */

    switch (reason) {
        default:
            printf("\nGoodbye!\n");
            break;
    }

    exit(EXIT_SUCCESS);
}

void clear_line(WINDOW *window, int row) 
{
	/* Have an idea for something smarter? */
    wmove(window, row, 1);
    wclrtoeol(window);
	box(window, 0, 0);
	wrefresh(window);
}

/*
 * A call to: "getmaxyx(stdscr, rows, cols);" should be as efficient.
 */
void get_terminal_size(int *rows, int *cols)
{
    struct winsize ws;

    if (ioctl(0, TIOCGWINSZ, &ws) < 0) {
        perror("Could not get the terminal size!");
        exit(EXIT_FAILURE);
    }

    *rows = ( int )ws.ws_row;
    *cols = ( int )ws.ws_col;
}

int is_terminal_size_larger_enough(int *rows, int *cols) {
	int is_larger = 0;

	get_terminal_size(rows, cols);
	if (*rows >= 45 && *cols >= 140) {
		is_larger = 1;
	}
	return is_larger;
}

void handler(int signum)
{
    switch (signum) {
        case SIGALRM:
            return;
        case SIGTERM:
        case SIGINT:
            quit_nicely(NO_PARTICULAR_REASON);
        default:
            break;
    }
}

void set_timer(void)
{
    struct itimerval it;

    /* Clear itimerval struct members */
    timerclear(&it.it_interval);
    timerclear(&it.it_value);

    /* Set timer */
    it.it_interval.tv_usec = TIME_STEP;
    it.it_value.tv_usec = TIME_STEP;
    setitimer(ITIMER_REAL, &it, NULL);
}

void set_signals()
{
    struct sigaction action;

    action.sa_handler = handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);

    /* Set signal handlers */
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGALRM, &action, NULL);

    /*  Ignore SIGTSTP  */
    action.sa_handler = SIG_IGN;
    sigaction(SIGTSTP, &action, NULL);
}
