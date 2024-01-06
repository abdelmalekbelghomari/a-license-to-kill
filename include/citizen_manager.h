#ifndef CITIZEN_MANAGER_H
#define CITIZEN_MANAGER_H
#include "memory.h"


/**
 * \brief Initializes synchronization tools.
 *
 * Initializes synchronization tools including barriers and mutex.
 */
void initialize_synchronization_tools();

/**
 * \brief Manages citizen threads.
 *
 * \param characters_list Pointer to the array of citizen structures.
 */
void manage_citizens(citizen_t *characters_list);

/**
 * \brief Defines the behavior of individual citizens in a multi-threaded environment.
 *
 * \param arg Pointer to the citizen structure.
 * \param memory Pointer to the shared memory.
 */
void *citizen_behavior(void *arg, memory_t *memory);

/**
 * \brief Starts individual threads for each citizen in the provided list.
 *
 * \param characters_list Pointer to the array of citizens.
 */
void start_citizen_threads(citizen_t *characters_list);

/**
 * \brief Handles actions for normal citizens based on the current simulation time.
 *
 * \param character Pointer to the citizen structure.
 * \param currentTime The current simulation time.
 */
void handle_normal_citizen_actions(citizen_t *character, double currentTime);

/**
 * \brief Manages shopping activities and returning home for citizens.
 *
 * \param character Pointer to the citizen structure.
 */
void handle_citizen_shopping_and_return_home(citizen_t *character);

/**
 * \brief Moves a citizen to their home based on specific conditions.
 *
 * \param character Pointer to the citizen structure.
 */
void move_citizen_to_home(citizen_t *character);

/**
 * \brief Moves a citizen to their workplace based on specific conditions.
 *
 * \param character Pointer to the citizen structure.
 */
void move_citizen_to_work(citizen_t *character);

/**
 * \brief Moves a citizen to the supermarket based on specific conditions.
 *
 * \param character Pointer to the citizen structure.
 */
void move_citizen_to_supermarket(citizen_t *character);

/**
 * \brief Changes the state of a citizen to a new state.
 *
 * \param c Pointer to the citizen structure.
 * \param s Pointer to the new state structure.
 */
void state_change_state(citizen_t *c, state_t *s);

/**
 * \brief Creates a new state with a specified ID and associated action.
 *
 * \param id The ID of the new state.
 * \param action Pointer to the action function for the state.
 * \return Pointer to the new state structure.
 */
state_t *new_state(int id, state_t *(*action)(citizen_t *));

/**
 * \brief Defines the behavior of citizens when resting at home during specific hours.
 *
 * \param c Pointer to the citizen structure.
 * \return Pointer to the new state structure.
 */
state_t *rest_at_home(citizen_t *c);

/**
 * \brief Defines the behavior of citizens going to their workplace.
 *
 * \param c Pointer to the citizen structure.
 * \return Pointer to the new state structure.
 */
state_t *go_to_company(citizen_t *c);

/**
 * \brief Defines the behavior of citizens while at work.
 *
 * \param c Pointer to the citizen structure.
 * \return Pointer to the new state structure.
 */
state_t *work(citizen_t *c);

/**
 * \brief Defines the behavior of citizens going to the supermarket.
 *
 * \param c Pointer to the citizen structure.
 * \return Pointer to the new state structure.
 */
state_t *go_to_supermarket(citizen_t *c);

/**
 * \brief Defines the behavior of citizens going back home from work.
 *
 * \param c Pointer to the citizen structure.
 * \return Pointer to the new state structure.
 */
state_t *go_back_home(citizen_t *c);

/**
 * \brief Defines the behavior of citizens while shopping at the supermarket.
 *
 * \param c Pointer to the citizen structure.
 * \return Pointer to the new state structure.
 */
state_t *do_some_shopping(citizen_t *c);

/**
 * \brief Calculates the current simulation time based on the internal clock.
 *
 * \param memory Pointer to the shared memory.
 * \return The current simulation time.
 */
double get_current_simulation_time(memory_t *memory);

/**
 * \brief Checks if a citizen is currently at the supermarket.
 *
 * \param character Pointer to the citizen structure.
 * \return true if the citizen is at the supermarket, false otherwise.
*/
int is_at_supermarket(citizen_t *character);

/**
 * \brief Opens an existing shared memory segment.
 *
 * Opens an existing shared memory segment with a predefined name.
 *
 * \return The shared memory structure pointer.
 */
memory_t open_shared_memory();

/**
 * \brief Initiates the beginning state for a citizen.
 *
 * \param c Pointer to the citizen structure.
 */
void *begin(citizen_t *c);

/**
 * \brief Performs a single step in the simulation for a citizen.
 *
 * \param c Pointer to the citizen structure.
 */
void *step(citizen_t *c);

/**
 * \brief Initiates the ending state for a citizen.
 *
 * \param c Pointer to the citizen structure.
 */
void *end(citizen_t *c);

/**
 * \brief Changes the state of a citizen to a new state.
 *
 * \param c Pointer to the citizen structure.
 * \param new_state Pointer to the new state structure.
 */
void *change_state(citizen_t *c, state_t *new_state);


#endif // CITIZEN_MANAGER_H