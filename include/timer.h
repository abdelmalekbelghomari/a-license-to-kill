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

#define STEP 1000000

/**
 * \brief Creates a new simulated clock instance with initial values.
 *
 * This function initializes a new simulated clock structure with default values for rounds, hours, minutes, and days.
 *
 * \return A simulated_clock_t structure representing the new timer.
 */
simulated_clock_t new_timer();

/**
 * \brief Updates the simulated clock based on the elapsed time since the last update.
 *
 * This function is responsible for advancing the simulated clock based on the time elapsed since the last update.
 * It increments the rounds, minutes, hours, and days accordingly, considering the simulation speed and maintaining consistency.
 *
 * \param memory Pointer to the shared memory.
 */
void update_timer(memory_t *memory);

/**
 * \brief Advances the simulated clock by one tick in response to a timer signal.
 *
 * This function is called in response to a timer signal (SIGALRM) and increments the simulated clock by one tick.
 * It updates the timer in a way that reflects the passage of time in the simulation.
 *
 * \param sig The signal received to trigger the clock tick (SIGALRM in this case).
 */
void tick_clock(int sig);


#endif // TIMER_H
