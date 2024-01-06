#ifndef SPY_SIMULATION
#define SPY_SIMULATION 

#include "memory.h"
#include <stdlib.h>
#include <time.h>
/*#include <sys/mmap.h>*/
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>


/**
 * \brief Handles a fatal error in the program by displaying an error message and terminating the application.
 *
 * \param message A descriptive error message.
 */
void handle_fatal_error(const char *message);

/**
 * \brief Initializes the city map with default values.
 *
 * \param cityMap Pointer to the map structure to be initialized.
 */
void init_map(map_t *cityMap);

/**
 * \brief Calculates the Manhattan distance between two positions in the city.
 *
 * \param pos1 The coordinates of the first position.
 * \param pos2 The coordinates of the second position.
 * \return The calculated distance.
 */
double distance(unsigned int pos1[2], unsigned int pos2[2]);

/**
 * \brief Initializes the houses.
 *
 * \param memory Pointer to the shared memory.
 */
void init_house(memory_t *memory);

/**
 * \brief Initializes the buildings.
 *
 * \param memory Pointer to the shared memory.
 */
void init_building(memory_t *memory);

/**
 * \brief Assigns a home to a citizen in the city.
 *
 * \param memory Pointer to the shared memory.
 * \param citizen Pointer to the citizen.
 */
void assign_home_to_citizen(memory_t *memory, citizen_t *citizen);

/**
 * \brief Assigns a company to a citizen in the city.
 *
 * \param memory Pointer to the shared memory.
 * \param citizen Pointer to the citizen.
 */
void assign_company_to_citizen(memory_t *memory, citizen_t *citizen);

/**
 * \brief Assigns a random supermarket to a citizen in the city.
 *
 * \param memory Pointer to the shared memory.
 * \param citizen Pointer to the citizent.
 */
void assign_random_supermarket(memory_t *memory, citizen_t *citizen);

/**
 * \brief Initializes the citizens.
 *
 * \param memory Pointer to the shared memory.
 */
void init_citizens(memory_t *memory);

/**
 * \brief Initializes the surveillance network.
 *
 * \param surveillance Pointer to the surveillance network structure.
 */
void init_surveillance(surveillanceNetwork_t *surveillance);

/**
 * \brief Starts the simulation processes.
 */
void start_simulation_processes();

/**
 * \brief Creates a shared memory segment with the specified name.
 *
 * \param name The name of the shared memory segment.
 * \return Pointer to the created shared memory.
 */
struct memory_s *create_shared_memory(const char *name);

/**
 * \brief Checks if the city map is connected, i.e., all cells are reachable from any other cell.
 *
 * \param cityMap Pointer to the map structure representing the city.
 * \return true if the city is connected, false otherwise.
 */
bool isConnected(map_t *cityMap);

/**
 * \brief Performs a Depth-First Search (DFS) to check the connectivity of the city map.
 *
 * \param cityMap Pointer to the map structure representing the city.
 * \param visited 2D array indicating visited cells.
 * \param row Current row in the DFS.
 * \param col Current column in the DFS.
 * \param endRow The target row for connectivity check.
 * \param endCol The target column for connectivity check.
 * \return true if the city is connected, false otherwise.
 */
bool dfs(map_t *cityMap, bool visited[MAX_ROWS][MAX_COLUMNS], int row, int col, int endRow, int endCol);

/**
 * \brief Creates a named semaphore with the specified value.
 *
 * \param name The name of the semaphore.
 * \param value The initial value of the semaphore.
 * \return Pointer to the created semaphore.
 */
sem_t *create_semaphore(const char *name, int value);

/**
 * \brief Opens an existing named semaphore.
 *
 * \param name The name of the semaphore.
 * \return Pointer to the opened semaphore.
 */
sem_t *open_semaphore(const char *name);

/**
 * \brief Starts the citizen manager process responsible for managing and simulating the behavior of citizens in the city.
 *
 * \return The PID of the citizen manager process.
 */
int start_citizen_manager();

/**
 * \brief Starts the monitor process responsible for monitoring and controlling various aspects of the simulation.
 *
 * \return The PID of the monitor process.
 */
int start_monitor();

/**
 * \brief Starts the enemy spy network process responsible for simulating the activities of the enemy spy network in the city.
 *
 * \return The PID of the spy network process.
 */
int start_enemy_spy_network();

/**
 * \brief Starts the counterintelligence officer process responsible for simulating the actions of the counterintelligence officer in the city.
 *
 * \return The PID of the counterintelligence officer process.
 */
int start_counterintelligence_officer();

/**
 * \brief Starts the process representing the enemy country in the simulation.
 *
 * \return The PID of the enemy country process.
 */
int start_enemy_country();

/**
 * \brief Starts the timer process responsible for tracking and controlling the simulation time.
 *
 * \return The PID of the timer process.
 */
int start_timer();


#endif // SPY_SIMULATION