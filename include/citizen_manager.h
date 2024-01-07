/**
 * @file citizen_manager.h
 * @brief Header for citizen management in the espionage simulation project.
 *
 * Declares the functions and structures necessary for managing citizens
 * in the context of an espionage simulation, including assigning homes,
 * workplaces, and managing the states of citizens.
 */

#ifndef CITIZEN_MANAGER_H
#define CITIZEN_MANAGER_H
#include "memory.h"

/**
 * @brief Assign a home to a citizen.
 *
 * Randomly assigns a home to a citizen while respecting the maximum
 * capacity of homes.
 *
 * @param memory Pointer to the shared memory structure.
 * @param citizen Pointer to the citizen to whom a home is to be assigned.
 */
void assign_home_to_citizen(memory_t* memory, citizen_t* citizen);

/**
 * @brief Assign a workplace to a citizen.
 *
 * Randomly assigns a company to a citizen while respecting the maximum
 * and minimum capacity of companies.
 *
 * @param memory Pointer to the shared memory structure.
 * @param citizen Pointer to the citizen to whom a workplace is to be assigned.
 */
void assign_company_to_citizen(memory_t* memory, citizen_t* citizen);

/**
 * @brief Assign a supermarket to a citizen.
 *
 * Randomly assigns a supermarket to a citizen.
 *
 * @param memory Pointer to the shared memory structure.
 * @param citizen Pointer to the citizen to whom a supermarket is to be assigned.
 */
void assign_random_supermarket(memory_t* memory, citizen_t* citizen);

/**
 * @brief Initialize the citizens.
 *
 * Initializes citizens with default data, assigns homes, workplaces,
 * and sets initial paths.
 *
 * @param memory Pointer to the shared memory structure.
 */
void init_citizens(memory_t *memory);

/**
 * @brief Initialize the houses in the simulation.
 *
 * Creates and configures houses for citizens, including setting their capacity,
 * position, and other necessary properties.
 *
 * @param memory Pointer to the shared memory structure.
 */
void init_house(memory_t *memory);

/**
 * @brief Initialize the buildings (companies and supermarkets) in the simulation.
 *
 * Configures buildings by setting their type, capacity, and position.
 *
 * @param memory Pointer to the shared memory structure.
 */
void init_building(memory_t *memory);

/**
 * @brief Create a new state for a citizen.
 *
 * Allocates and initializes a new state with an identifier and an action function.
 *
 * @param id Identifier of the state.
 * @param action Function to execute when the state is active.
 * @return state_t* Pointer to the newly created state.
 */
state_t *new_state(int id, state_t *(*action)(citizen_t *));

/**
 * @brief Action for the "resting at home" state.
 *
 * Manages the logic for a citizen resting at home. Changes state if necessary.
 *
 * @param c Pointer to the citizen.
 * @return state_t* Pointer to the next state.
 */
state_t *rest_at_home(citizen_t *c);

/**
 * @brief Action for the "going to work" state.
 *
 * Manages the logic of a citizen moving to their workplace.
 *
 * @param c Pointer to the citizen.
 * @return state_t* Pointer to the next state.
 */
state_t *go_to_company(citizen_t *c);

/**
 * @brief Action for the "working" state.
 *
 * Manages the working logic of a citizen at their company.
 *
 * @param c Pointer to the citizen.
 * @return state_t* Pointer to the next state.
 */
state_t *work(citizen_t *c);

/**
 * @brief Action for the "going to supermarket" state.
 *
 * Manages the logic of a citizen moving to a supermarket.
 *
 * @param c Pointer to the citizen.
 * @return state_t* Pointer to the next state.
 */
state_t *go_to_supermarket(citizen_t *c);

/**
 * @brief Action for the "going back home" state.
 *
 * Manages the logic of a citizen moving back to their home.
 *
 * @param c Pointer to the citizen.
 * @return state_t* Pointer to the next state.
 */
state_t *go_back_home(citizen_t *c);

/**
 * @brief Action for the "doing some shopping" state.
 *
 * Manages the logic of a citizen doing shopping in a supermarket.
 *
 * @param c Pointer to the citizen.
 * @return state_t* Pointer to the next state.
 */
state_t *do_some_shopping(citizen_t *c);

/**
 * @brief Action for the "dying" state.
 *
 * Manages the end-of-life logic for a citizen.
 *
 * @param c Pointer to the citizen.
 * @return state_t* Pointer to the death state.
 */
state_t *dying(citizen_t *c);

/**
 * @brief Action for the "finished" state.
 *
 * Marks the end of the life cycle of a citizen.
 *
 * @param c Pointer to the citizen.
 * @return state_t* Pointer to the state indicating completion.
 */
state_t *finished(citizen_t *c);

/**
 * @brief Calculate the current simulation time.
 *
 * Computes and returns the current time in the simulation in hours and minutes.
 *
 * @param memory Pointer to the shared memory structure.
 * @return double The current time in the simulation.
 */
double get_current_simulation_time(memory_t *memory);

/**
 * @brief Open shared memory.
 *
 * Opens and maps the shared memory used for storing shared data structures
 * among different processes in the simulation.
 *
 * @return memory_t Data structure of the shared memory.
 */
memory_t open_shared_memory();

#endif // CITIZEN_MANAGER_H
