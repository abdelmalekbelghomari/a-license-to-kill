#ifndef ENEMY_SPY_NETWORK_H
#define ENEMY_SPY_NETWORK_H

#include "memory.h"
#include "monitor.h"
#include "enemy_spy_network.h"

/**
 * \brief Handles fatal errors by displaying an error message and exiting the program.
 *
 * \param message The error message to be displayed.
 */
void handle_fatal_error(const char *message);

/**
 * \brief Determines the choice of action for a spy based on random probabilities.
 *
 * \param spy The spy for which the choice is made.
 * \return The choice representing the action (SUPERMARKET, -1 for going back home, WASTELAND).
 */
int spy_choice(spy_t spy);

/**
 * \brief Checks if the spy has already stolen a specific company.
 *
 * \param spy The spy to check for stolen companies.
 * \return 1 if the spy has already stolen this company, 0 otherwise.
 */
int already_stolen(spy_t spy);

/**
 * \brief Checks if someone else has already stolen a specific company.
 *
 * \param row The row of the company.
 * \param column The column of the company.
 * \return 1 if someone else has already stolen this company, 0 otherwise.
 */
int someone_stolen(int row, int column);

/**
 * \brief Checks if it is possible for the spy to steal a company based on time probabilities.
 *
 * \param spy The spy attempting to steal.
 * \param t The current time.
 * \return 1 if the spy can steal the company, 0 otherwise.
 */
int possible_theft(spy_t spy, int t);

/**
 * \brief Checks and corrects the position of a spy or officer within the map boundaries.
 *
 * \param memory Pointer to the shared memory.
 * \param choice The choice representing the character (0-2 for spies, 3 for case officer, 4 for counterintelligence officer).
 */
void check_position(memory_t* memory, int choice);

/**
 * \brief Moves a spy around a specific company's position.
 *
 * \param spy Pointer to the spy.
 * \param pos The position of the company.
 */
void spy_move_around_company(spy_t* spy, cell_t pos);

/**
 * \brief Determines if a spy wants to steal a specific company based on certain conditions.
 *
 * \param spy Pointer to the spy.
 * \param pos The position of the company.
 * \return 1 if the spy wants to steal the company, 0 otherwise.
 */
int consider_theft(spy_t* spy, int* pos);

/**
 * \brief Handles the theft action for a spy, updating the stolen companies and message content.
 *
 * \param spy Pointer to the spy.
 */
void theft(spy_t* spy);

/**
 * \brief Moves a spy randomly within the map boundaries.
 *
 * \param spy Pointer to the spy.
 */
void move_spy(spy_t* spy);

/**
 * \brief Retrieves information about a spy for initialization.
 *
 * \param is_killer Indicates whether the spy has a license to kill.
 * \param id The ID of the spy.
 * \return The spy information.
 */
spyInfo* get_info_spy(int is_killer, int id);

/**
 * \brief Retrieves information about a case officer for initialization.
 *
 * \return The caseOfficer information.
 */
caseOfficerInfo* get_info_case_officer();

/**
 * \brief Fills the allowed companies for a spy based on the company types in the map.
 *
 * \param memory Pointer to the shared memory.
 * \param n The ID of the spy.
 */
void fill_company_spy(memory_t* memory, int n);

/**
 * \brief Moves a character (spy, case officer, or counterintelligence officer) to the mailbox.
 *
 * \param memory Pointer to the shared memory.
 * \param choice The choice representing the character (0-2 for spies, 3 for case officer, 4 for counterintelligence officer).
 */
void go_to_mail_box(memory_t* memory, int choice);

/**
 * \brief Checks if a certain cell type is present around a specific position.
 *
 * \param row The row of the position.
 * \param column The column of the position.
 * \param cell_type The type of the cell to check for.
 * \return Array containing the row and column of the found cell, or {0, 0} if not found.
 */
int* is_around(int row, int column, int cell_type);

/**
 * \brief Handles the case officer going shopping action.
 *
 * \param case_officer Pointer to the case officer structure.
 */
void case_officer_go_shopping(case_officer_t* case_officer);

/**
 * \brief Handles the case officer going home action.
 *
 * \param case_officer Pointer to the case officer structure.
 */
void case_officer_go_home(case_officer_t* case_officer);

/**
 * \brief The main function representing the life of a spy. Handles actions such as theft and movement.
 *
 * \param thread Pointer to the spyInfo structure representing the spy.
 */
void* spy_life(void* thread);
  

#endif /* ENEMY_SPY_NETWORK_H */


