/**
 * \file enemy_spy_network.h
 * \brief Header file for the Enemy Spy Network executable.
 *
 * The Enemy Spy Network executable manages the spy and case officer threads in
 * the espionage network. Their goal is to steal information and send it to the
 * enemy country, place the information in a mailbox, and have the case officer
 * retrieve the encrypted letters and send them to the enemy country.
 */

#ifndef ENEMY_SPY_NETWORK_H
#define ENEMY_SPY_NETWORK_H

#include "memory.h"
#include "monitor.h"

#define SPY_COUNT 3

/**
 * \struct TimeRange
 * \brief Structure representing a time range.
 *
 * It includes:
 * - start_hour: The start hour of the range.
 * - end_hour: The end hour of the range.
 * - probability: The probability associated with the time range.
 */
typedef struct {
    int start_hour; /*!< The start hour of the range. */
    int end_hour;   /*!< The end hour of the range. */
    float probability; /*!< The probability associated with the time range. */
} TimeRange;

/**
 * \brief Initializes the spy agents.
 */
void init_spies();

/**
 * \brief Handles the spy resting at home state.
 *
 * This function represents the behavior of a spy when resting at home.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *rest_at_home(spy_t *spy);

/**
 * \brief Handles the spy going to a spot state.
 *
 * This function represents the behavior of a spy when going to a spot.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *go_to_spot(spy_t *spy);

/**
 * \brief Handles the spy spotting state.
 *
 * This function represents the behavior of a spy when spotting.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *spot(spy_t *spy);

/**
 * \brief Handles the spy stealing state.
 *
 * This function represents the behavior of a spy when stealing.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *steal(spy_t *spy);

/**
 * \brief Handles the spy going to send a fake message state.
 *
 * This function represents the behavior of a spy when going to send a fake message.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *go_to_send_fake_message(spy_t *spy);

/**
 * \brief Handles the spy sending a message state.
 *
 * This function represents the behavior of a spy when sending a message.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *send_message(spy_t *spy);

/**
 * \brief Handles the spy arriving at the mailbox state.
 *
 * This function represents the behavior of a spy when arriving at the mailbox.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *arrived_at_mailbox(spy_t *spy);

/**
 * \brief Handles the spy going back home state.
 *
 * This function represents the behavior of a spy when going back home.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *going_back_home(spy_t *spy);

/**
 * \brief Handles the spy going to send a message state.
 *
 * This function represents the behavior of a spy when going to send a message.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *go_to_send_message(spy_t *spy);

/**
 * \brief Handles the spy going to the supermarket state.
 *
 * This function represents the behavior of a spy when going to the supermarket.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *go_to_supermarket(spy_t *spy);

/**
 * \brief Handles the spy doing some shopping state.
 *
 * This function represents the behavior of a spy when doing some shopping.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *do_some_shopping(spy_t *spy);

/**
 * \brief Handles the spy waiting for the residence to be clear state.
 *
 * This function represents the behavior of a spy when waiting for the residence to be clear.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *wait_for_residence_to_be_clear(spy_t *spy);

/**
 * \brief Handles the spy scouting state.
 *
 * This function represents the behavior of a spy when scouting.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *scout(spy_t *spy);

/**
 * \brief Handles the spy being hurt state.
 *
 * This function represents the behavior of a spy when hurt.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *is_hurt(spy_t *spy);

/**
 * \brief Handles the spy being free state.
 *
 * This function represents the behavior of a spy when free.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *is_free(spy_t *spy);

/**
 * \brief Handles the spy riposte state.
 *
 * This function represents the behavior of a spy when in riposte.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *riposte(spy_t *spy);

/**
 * \brief Handles the spy being in conflict state.
 *
 * This function represents the behavior of a spy when in conflict.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *is_in_conflict(spy_t *spy);

/**
 * \brief Handles the spy dying state.
 *
 * This function represents the behavior of a spy when dying.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *dying(spy_t *spy);

/**
 * \brief Handles the spy finished state.
 *
 * This function represents the behavior of a spy when finished.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *finished(spy_t *spy);

/**
 * \brief Handles the spy performing an action state.
 *
 * This function represents the behavior of a spy when performing an action.
 *
 * \param spy A pointer to the spy.
 * \return A pointer to the next state for the spy.
 */
state_t *do_something(spy_t *spy);


/**
 * \brief Creates a new state for a spy agent.
 *
 * Creates a new state for a spy agent with the specified `id` and action.
 *
 * \param id The identifier of the spy.
 * \param action The action function for the spy state.
 * \return A pointer to the new spy state.
 */
state_t *new_state_spy(int id, state_t *(*action)(spy_t *));

/**
 * \brief Assigns leaving time to a spy agent.
 *
 * Assigns leaving time to a spy agent based on time ranges and probabilities.
 *
 * \param spy A pointer to the spy agent.
 */
void assign_leaving_time(spy_t *spy);

/**
 * \brief Assigns a home location to a spy agent.
 *
 * Assigns a home location to a spy agent using shared memory.
 *
 * \param memory A pointer to the shared memory structure.
 * \param spy A pointer to the spy agent.
 */
void assign_home_to_spy(memory_t *memory, spy_t *spy);

/**
 * \brief Checks if a spy agent is at home.
 *
 * Checks if a spy agent is at their home location.
 *
 * \param spy A pointer to the spy agent.
 * \return `1` if the spy is at home, `0` otherwise.
 */
int is_at_home(spy_t *spy);

/**
 * \brief Checks if a spy agent is at the mailbox.
 *
 * Checks if a spy agent is at the mailbox location.
 *
 * \param spy A pointer to the spy agent.
 * \return `1` if the spy is at the mailbox, `0` otherwise.
 */
int is_at_mailbox(spy_t *spy);

/**
 * \brief Checks if a spy agent is at the supermarket.
 *
 * Checks if a spy agent is at the supermarket location.
 *
 * \param spy A pointer to the spy agent.
 * \return `1` if the spy is at the supermarket, `0` otherwise.
 */
int is_at_supermarket(spy_t *spy);

/**
 * \brief Initializes the case officer.
 */
void init_officer();

/**
 * \brief Handles the case officer resting at home state.
 *
 * This function represents the behavior of a case officer when resting at home.
 *
 * \param officer A pointer to the case officer.
 * \return A pointer to the next state for the case officer.
 */
state_t *rest_at_home_officer(case_officer_t *officer);

/**
 * \brief Handles the case officer sending messages state.
 *
 * This function represents the behavior of a case officer when sending messages.
 *
 * \param officer A pointer to the case officer.
 * \return A pointer to the next state for the case officer.
 */
state_t *send_messages(case_officer_t *officer);

/**
 * \brief Handles the case officer going back home state.
 *
 * This function represents the behavior of a case officer when going back home.
 *
 * \param officer A pointer to the case officer.
 * \return A pointer to the next state for the case officer.
 */
state_t *go_back_home_officer(case_officer_t *officer);

/**
 * \brief Handles the case officer going to the supermarket state.
 *
 * This function represents the behavior of a case officer when going to the supermarket.
 *
 * \param officer A pointer to the case officer.
 * \return A pointer to the next state for the case officer.
 */
state_t *go_to_supermarket_officer(case_officer_t *officer);

/**
 * \brief Handles the case officer doing some shopping state.
 *
 * This function represents the behavior of a case officer when doing some shopping.
 *
 * \param officer A pointer to the case officer.
 * \return A pointer to the next state for the case officer.
 */
state_t *do_some_shopping_officer(case_officer_t *officer);

/**
 * \brief Handles the case officer going to the mailbox state.
 *
 * This function represents the behavior of a case officer when going to the mailbox.
 *
 * \param officer A pointer to the case officer.
 * \return A pointer to the next state for the case officer.
 */
state_t *go_to_mailbox(case_officer_t *officer);

/**
 * \brief Handles the case officer recovering messages state.
 *
 * This function represents the behavior of a case officer when recovering messages.
 *
 * \param officer A pointer to the case officer.
 * \return A pointer to the next state for the case officer.
 */
state_t *recover_messages(case_officer_t *officer);


/**
 * \brief Creates a new state for a case officer.
 *
 * Creates a new state for a case officer with the specified `id` and action.
 *
 * \param id The identifier of the case officer.
 * \param action_officer The action function for the case officer state.
 * \return A pointer to the new case officer state.
 */
state_t *new_state_officer(int id, state_t *(*action_officer)(case_officer_t *));

/**
 * \brief Assigns times to a case officer for state transitions.
 *
 * Assigns times to a case officer for state transitions based on shared memory.
 *
 * \param officer A pointer to the case officer.
 */
void assign_officer_times(case_officer_t *officer);

/**
 * \brief Checks if one leaving time is less than two hours ahead of another.
 *
 * Checks if `time1` is less than two hours ahead of `time2`.
 *
 * \param time1 The first leaving time.
 * \param time2 The second leaving time.
 * \return `true` if `time1` is less than two hours ahead of `time2`, `false` otherwise.
 */
bool less_than_two_hours(leaving_time_t time1, leaving_time_t time2);

/**
 * \brief Performs Caesar cipher encryption on a message.
 *
 * Performs Caesar cipher encryption on a message by shifting characters.
 *
 * \param message A pointer to the message to be encrypted.
 */
void caesar_cipher(char *message);

/**
 * \brief Retrieves the priority of a crypted message.
 *
 * Retrieves the priority of a crypted message for processing.
 *
 * \param message A pointer to the crypted message.
 * \return The priority of the crypted message.
 */
unsigned int get_crypted_message_priority(const char *message);

/**
 * \brief Assigns a home location to a case officer.
 *
 * Assigns a home location to a case officer using shared memory.
 *
 * \param memory A pointer to the shared memory structure.
 * \param officer A pointer to the case officer.
 */
void assign_home_to_officer(memory_t *memory, case_officer_t *officer);

/**
 * \brief Checks if a case officer is at home.
 *
 * Checks if a case officer is at their home location.
 *
 * \param officer A pointer to the case officer.
 * \return `1` if the case officer is at home, `0` otherwise.
 */
int is_at_home_officer(case_officer_t *officer);

/**
 * \brief Checks if a case officer is at the mailbox.
 *
 * Checks if a case officer is at the mailbox location.
 *
 * \param officer A pointer to the case officer.
 * \return `1` if the case officer is at the mailbox, `0` otherwise.
 */
int is_at_mailbox_officer(case_officer_t *officer);

/**
 * \brief Checks if a case officer is at the supermarket.
 *
 * Checks if a case officer is at the supermarket location.
 *
 * \param officer A pointer to the case officer.
 * \return `1` if the case officer is at the supermarket, `0` otherwise.
 */
int is_at_supermarket_officer(case_officer_t *officer);

#endif /* ENEMY_SPY_NETWORK_H */
