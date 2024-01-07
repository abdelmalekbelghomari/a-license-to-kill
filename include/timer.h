/**
 * \file timer.h
 * \brief Header file for the Timer executable.
 *
 * The Timer executable is responsible for providing time and round information
 * to other processes. It uses `SIGALRM` to tick the clock and, if the maximum
 * number of rounds is reached, it terminates all other processes using `SIGINT`.
 */

#ifndef TIMER_H
#define TIMER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/select.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <semaphore.h>

#include "memory.h"

#define TIME_STEP 200000

/**
 * \brief Creates and initializes a new timer.
 *
 * This function creates and initializes a new timer represented by a
 * `simulated_clock_t` structure.
 *
 * \return A `simulated_clock_t` structure representing the new timer.
 */
simulated_clock_t new_timer();

/**
 * \brief Updates the timer using shared memory.
 *
 * This function updates the timer using shared memory by modifying the `memory`
 * structure.
 *
 * \param memory A pointer to the shared memory structure.
 */
void update_timer(memory_t *memory);

/**
 * \brief Ticks the clock when the `sigalarm` signal is received.
 *
 * This function is called when the `sigalarm` signal is received, and it ticks
 * the clock to advance the simulation time.
 *
 * \param sig The signal number (SIGALRM).
 */
void tick_clock(int sig);

/**
 * \brief Sets up the timer for the simulation.
 *
 * This function sets up the timer for the simulation by configuring the signal
 * handler and timer intervals.
 */
void set_timer(void);

#endif // TIMER_H

