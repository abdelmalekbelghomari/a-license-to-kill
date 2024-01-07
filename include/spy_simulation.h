/**
 * \file spy_simulation.h
 * \brief Header file for the Spy Simulation executable.
 */

#ifndef SPY_SIMULATION
#define SPY_SIMULATION 

#include "memory.h"
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>

typedef struct memory_s memory_t;


#define CI_OFFICER_INDEX 4

/**
 * \brief Handles fatal errors and exits the program with an error message.
 *
 * This function prints an error message and terminates the program.
 *
 * \param message The error message to be displayed.
 */
void handle_fatal_error(const char *message);

/**
 * \brief Initializes the city map.
 *
 * Initializes the city map represented by the provided `cityMap`.
 *
 * \param cityMap A pointer to the city map structure.
 */
void init_map(map_t *cityMap);

/**
 * \brief Initializes the surveillance network.
 *
 * Initializes the surveillance network represented by the provided `surveillance`.
 *
 * \param surveillance A pointer to the surveillance network structure.
 */
void init_surveillance(surveillanceNetwork_t *surveillance);

/**
 * \brief Starts the simulation processes.
 *
 * Starts the processes responsible for the simulation.
 */
void start_simulation_processes();

/**
 * \brief Creates a shared memory segment.
 *
 * Creates a shared memory segment with the specified name.
 *
 * \param name The name of the shared memory segment.
 * \return A pointer to the created shared memory structure.
 */
struct memory_s *create_shared_memory(const char *name);

/**
 * \brief Checks if the city map is connected.
 *
 * Checks if the city map represented by `cityMap` is connected.
 *
 * \param cityMap A pointer to the city map structure.
 * \return `true` if the map is connected, `false` otherwise.
 */
bool isConnected(map_t *cityMap);

/**
 * \brief Performs depth-first search on the city map.
 *
 * Performs depth-first search (DFS) on the city map to find a path.
 *
 * \param cityMap A pointer to the city map structure.
 * \param visited A boolean array to track visited cells.
 * \param row The current row.
 * \param col The current column.
 * \param endRow The target row.
 * \param endCol The target column.
 * \return `true` if a path is found, `false` otherwise.
 */
bool dfs(map_t *cityMap, bool visited[MAX_ROWS][MAX_COLUMNS], int row, int col, int endRow, int endCol);

/**
 * \brief Creates a semaphore with the specified name and initial value.
 *
 * Creates a semaphore with the given name and initial value.
 *
 * \param name The name of the semaphore.
 * \param value The initial value of the semaphore.
 * \return A pointer to the created semaphore.
 */
sem_t *create_semaphore(const char *name, int value);

/**
 * \brief Opens an existing semaphore with the specified name.
 *
 * Opens an existing semaphore with the given name.
 *
 * \param name The name of the semaphore.
 * \return A pointer to the opened semaphore.
 */
sem_t *open_semaphore(const char *name);

#endif // SPY_SIMULATION