/**
 * \file counter_intelligence.h
 *
 * Defines structures and functions related to counter-intelligence officers.
 */

#ifndef COUNTER_INTELLIGENCE_H
#define COUNTER_INTELLIGENCE_H

#include "memory.h"

/**
 * \brief Initializes the counter-intelligence officer.
 *
 * Initializes the counter-intelligence officer's attributes and state.
 */
void init_counter_intelligence_officer();

/**
 * \brief Handles the monitoring state of the counter-intelligence officer.
 *
 * This function represents the behavior of the counter-intelligence officer when monitoring.
 *
 * \param officer A pointer to the counter-intelligence officer.
 * \return A pointer to the next state for the counter-intelligence officer.
 */
state_t *monitor(counter_intelligence_officer_t *officer);

/**
 * \brief Handles the state when the counter-intelligence officer goes to a suspect place.
 *
 * This function represents the behavior of the counter-intelligence officer when going to a suspect place.
 *
 * \param officer A pointer to the counter-intelligence officer.
 * \return A pointer to the next state for the counter-intelligence officer.
 */
state_t *go_to_suspect_place(counter_intelligence_officer_t *officer);

/**
 * \brief Handles the state when the counter-intelligence officer hides.
 *
 * This function represents the behavior of the counter-intelligence officer when hiding.
 *
 * \param officer A pointer to the counter-intelligence officer.
 * \return A pointer to the next state for the counter-intelligence officer.
 */
state_t *hide(counter_intelligence_officer_t *officer);

/**
 * \brief Handles the state when the counter-intelligence officer goes back to monitoring.
 *
 * This function represents the behavior of the counter-intelligence officer when going back to monitoring.
 *
 * \param officer A pointer to the counter-intelligence officer.
 * \return A pointer to the next state for the counter-intelligence officer.
 */
state_t *go_back_to_monitor(counter_intelligence_officer_t *officer);

/**
 * \brief Handles the state when the counter-intelligence officer waits for a spy to steal.
 *
 * This function represents the behavior of the counter-intelligence officer when waiting for a spy to steal.
 *
 * \param officer A pointer to the counter-intelligence officer.
 * \return A pointer to the next state for the counter-intelligence officer.
 */
state_t *wait_for_spy_to_steal(counter_intelligence_officer_t *officer);

/**
 * \brief Handles the state when the counter-intelligence officer follows a spy.
 *
 * This function represents the behavior of the counter-intelligence officer when following a spy.
 *
 * \param officer A pointer to the counter-intelligence officer.
 * \return A pointer to the next state for the counter-intelligence officer.
 */
state_t *follow_spy(counter_intelligence_officer_t *officer);

/**
 * \brief Handles the state when the counter-intelligence officer waits for a spy to send a message.
 *
 * This function represents the behavior of the counter-intelligence officer when waiting for a spy to send a message.
 *
 * \param officer A pointer to the counter-intelligence officer.
 * \return A pointer to the next state for the counter-intelligence officer.
 */
state_t *wait_for_spy_to_send_message(counter_intelligence_officer_t *officer);

/**
 * \brief Handles the state when the counter-intelligence officer searches for a mailbox.
 *
 * This function represents the behavior of the counter-intelligence officer when searching for a mailbox.
 *
 * \param officer A pointer to the counter-intelligence officer.
 * \return A pointer to the next state for the counter-intelligence officer.
 */
state_t *search_for_mailbox(counter_intelligence_officer_t *officer);

/**
 * \brief Handles the state when the counter-intelligence officer recovers a message.
 *
 * This function represents the behavior of the counter-intelligence officer when recovering a message.
 *
 * \param officer A pointer to the counter-intelligence officer.
 * \return A pointer to the next state for the counter-intelligence officer.
 */
state_t *recover_message(counter_intelligence_officer_t *officer);

/**
 * \brief Handles the state when the counter-intelligence officer goes to search for a mailbox.
 *
 * This function represents the behavior of the counter-intelligence officer when going to search for a mailbox.
 *
 * \param officer A pointer to the counter-intelligence officer.
 * \return A pointer to the next state for the counter-intelligence officer.
 */
state_t *go_to_search_for_mailbox(counter_intelligence_officer_t *officer);

/**
 * \brief Creates a new state for the counter-intelligence officer.
 *
 * Creates a new state for the counter-intelligence officer with the given ID and action function.
 *
 * \param id The ID of the state.
 * \param action A function pointer representing the action for the state.
 * \return A pointer to the newly created state.
 */
state_t *new_state(int id, state_t *(*action)(counter_intelligence_officer_t *officer));

/**
 * \brief Detects a suspicious person using monitoring data.
 *
 * This function detects a suspicious person using monitoring data from cameras.
 *
 * \param memory A pointer to the shared memory structure.
 */
void detect_suspicious_person(memory_t *memory);

/**
 * \brief Checks if a movement is suspicious.
 *
 * This function checks if a movement is suspicious based on monitoring data from cameras.
 *
 * \param movement A pointer to the movement data.
 * \param memory A pointer to the shared memory structure.
 * \return `true` if the movement is suspicious, `false` otherwise.
 */
bool is_movement_suspicious(characterMovement *movement, memory_t *memory);

/**
 * \brief Updates the status of a suspicious person based on monitoring data.
 *
 * This function updates the status of a suspicious person based on monitoring data from cameras.
 *
 * \param movement A pointer to the movement data.
 * \param memory A pointer to the shared memory structure.
 */
void update_suspicious_person(characterMovement *movement, memory_t *memory);

/**
 * \brief Checks if the counter-intelligence officer is at the monitoring location.
 *
 * Checks if the counter-intelligence officer is at the monitoring location.
 *
 * \param officer A pointer to the counter-intelligence officer.
 * \return `1` if the officer is at the monitoring location, `0` otherwise.
 */
int is_at_monitor(counter_intelligence_officer_t *officer);

/**
 * \brief Checks if the counter-intelligence officer is at a suspect place.
 *
 * Checks if the counter-intelligence officer is at a suspect place.
 *
 * \param officer A pointer to the counter-intelligence officer.
 * \return `1` if the officer is at a suspect place, `0` otherwise.
 */
int at_suspect_place(counter_intelligence_officer_t *officer);

/**
 * \brief Checks if the counter-intelligence officer is at a mailbox.
 *
 * Checks if the counter-intelligence officer is at a mailbox.
 *
 * \param officer A pointer to the counter-intelligence officer.
 * \return `1` if the officer is at a mailbox, `0` otherwise.
 */
int is_at_mailbox(counter_intelligence_officer_t *officer);

#endif // COUNTER_INTELLIGENCE_H

