/*
 * The License to Kill Project
 *
 * Copyright (C) 1995-2022 Alain Lebret <alain.lebret [at] ensicaen [dot] fr>
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

/**
 * \file memory.h
 *
 * Defines structures and functions used to manipulate shared memory and related data structures.
 *
 * This file provides the necessary data structures and functions for managing shared memory
 * and various components used in a simulation environment, such as map data, characters,
 * surveillance networks, and more.
 *
 */


#ifndef MEMORY_H
#define MEMORY_H

#include "common.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <mqueue.h>
#include <semaphore.h>
#include <stdbool.h>

#define NB_PROCESS 7


#define CITIZENS_COUNT 127 /* 127 citizens in the city */
#define MAX_ROWS 7
#define MAX_COLUMNS 7
#define MAX_ROUNDS 2016
#define NUM_CITIZENS 127
#define SPIES_COUNT 3
#define MAX_MESSAGE_SIZE 128
#define MAX_MESSAGES 10000
#define SHIFT 3
#define NUM_DIRECTIONS 8

#define NB_CITIZEN_HALL 10
#define NB_CITIZEN_STORE 6
#define NB_CITIZEN_COMPANY 111
#define NB_HOMES 11
#define NB_COMPANY 8
#define NB_STORE 2
#define NB_WORKPLACES 11 // 8 companies + 1 city hall + 2 supermarkets
#define NB_HALL 1
#define MAX_IN_BUILDING 15
#define MAX_IN_HALL 20
#define MAX_IN_STORE 30
#define MAX_IN_COMPANY 50
#define NB_CITIZEN_STATES 6

#define CITY_HALL_ROW 3
#define CITY_HALL_COLUMN 3

/**
 * \file memory.h
 *
 * Defines structures and functions used to manipulate our shared memory and 
 * also the all the structures we will need in multiple files.
 */

typedef struct map_s map_t;
typedef struct mq_s mq_t;
typedef struct memory_s memory_t;
typedef struct spy_s spy_t;
typedef struct case_officer_s case_officer_t;
typedef struct counter_intelligence_officer_s counter_intelligence_officer_t;
typedef struct state_s state_t;
typedef struct building_s building_t;
typedef struct home_s home_t;
typedef struct citizen_s citizen_t;
typedef struct mailbox_s mailbox_t;
typedef struct leaving_time_s leaving_time_t;
typedef struct Node Node;


/**
 * \typedef SpyMessage
 * \brief Structure representing a message from a spy.
 */
typedef struct {
    int priority; /*!< Priority of the message. */
    char content[MAX_MESSAGE_SIZE]; /*!< Content of the message. */
} SpyMessage;

/**
 * \enum citizen_type_t
 * \brief Enumeration for different types of citizens.
 * 
 * Possible values:
 * - NORMAL: Normal citizen.
 * - SPY: Spy citizen.
 * - CASE_OFFICER: Case officer citizen.
 * - COUNTER_INTELLIGENCE_OFFICER: Counterintelligence officer citizen.
 */
typedef enum citizen_type_e {
    NORMAL,
    SPY,
    CASE_OFFICER,
    COUNTER_INTELLIGENCE_OFFICER
} citizen_type_t;

/**
 * \enum building_type_t
 * \brief Enumeration for different types of buildings.
 * 
 * Possible values:
 * - CORPORATION: Corporation building.
 * - STORE: Store building.
 * - HALL: Hall building.
 */
typedef enum building_type_e {
    CORPORATION,
    STORE,
    HALL
} building_type_t;

/**
 * \struct mq_s
 * \brief Structure representing a message queue.
 */
struct mq_s {
    mqd_t mq; /*!< Message queue descriptor. */
};


/**
 * \typedef characterMovement
 * \brief Structure representing the movement of a character.
 */
typedef struct {
    int currentRow, currentColumn; /*!< Current position. */
    int previousRow, previousColumn; /*!< Previous position. */
    int historyRow[7], historyColumn[7]; /*!< Historical positions. */
} characterMovement;

/**
 * \typedef surveillanceCameras
 * \brief Structure representing surveillance cameras.
 */
typedef struct {
    int standard_camera; /*!< 0 = désactivée, 1 = activée for standard camera. */
    int infrared_camera; /*!< 0 = désactivée, 1 = activée for infrared camera. */
} surveillanceCameras;

/**
 * \typedef cellData
 * \brief Structure representing data for a cell.
 */
typedef struct {
    int numberOfPeople; /*!< Number of people in the cell. */
} cellData;

/**
 * \enum suspect_type_t
 * \brief Enumeration for different types of suspects.
 * 
 * Possible values:
 * - SUSPECT_NONE: No suspect.
 * - SUSPECT_CITIZEN: Suspect is a citizen.
 * - SUSPECT_SPY: Suspect is a spy.
 * - SUSPECT_CASE_OFFICER: Suspect is a case officer.
 */
typedef enum {
    SUSPECT_NONE,
    SUSPECT_CITIZEN,
    SUSPECT_SPY,
    SUSPECT_CASE_OFFICER
} suspect_type_t;

/**
 * \union suspect_t
 * \brief Union representing a suspect that can be a citizen, spy, or case officer.
 * 
 * It can be:
 * - citizen: A pointer to a citizen.
 * - spy: A pointer to a spy.
 * - case_officer: A pointer to a case officer.
 */
typedef union {
    citizen_t *citizen; /*!< A pointer to a citizen. */
    spy_t *spy; /*!< A pointer to a spy. */
    case_officer_t *case_officer; /*!< A pointer to a case officer. */
} suspect_t;

/**
 * \typedef surveillance_AI
 * \brief Structure representing surveillance AI.
 */
typedef struct {
    bool suspicious_movement; /*!< Indicates suspicious movement. */
    suspect_t suspect; /*!< The suspect. */
    suspect_type_t suspect_type; /*!< The type of suspect. */
} surveillance_AI;

/**
 * \typedef surveillanceNetwork_t
 * \brief Structure representing a surveillance network.
 */
typedef struct {
    cellData cells[MAX_COLUMNS][MAX_ROWS]; /*!< Data for each cell. */
    surveillanceCameras cameras; /*!< Surveillance cameras for the entire network. */
    surveillance_AI surveillanceAI; /*!< AI for surveillance. */
} surveillanceNetwork_t;


/**
 * \struct state_s
 * \brief Structure representing a state for a citizen.
 */
struct state_s {
    int id; /*!< Unique identifier for the state. */
    char *description; /*!< Description of the state. */

    /**
     * \brief Function pointer to change the state of a citizen.
     * 
     * This function changes the state of a citizen based on certain conditions.
     * 
     * \param citizen A pointer to the citizen whose state is being changed.
     * \param state A pointer to the new state.
     */
    void (*change_state)(citizen_t *, state_t *);

    /**
     * \brief Function pointer to perform an action for a citizen's state.
     * 
     * This function performs an action for a citizen's state.
     * 
     * \param citizen A pointer to the citizen for which the action is performed.
     * \return A pointer to the next state of the citizen.
     */
    state_t *(*action)(citizen_t *);
};

/**
 * \enum cell_type_e
 * \brief Enumeration for different types of cells within the city map.
 */
enum cell_type_e {
    WASTELAND,            /*!< An empty cell. */
    RESIDENTIAL_BUILDING, /*!< A cell that contains a residential building. */
    CITY_HALL,            /*!< A cell that contains the City Hall. */
    COMPANY,              /*!< A cell that contains a company. */
    SUPERMARKET           /*!< A cell that contains a supermarket. */
};

/**
 * \typedef cell_type_t
 * \brief Typedef for the cell type enumeration.
 */
typedef enum cell_type_e cell_type_t;

/**
 * \struct cell_s
 * \brief A cell within the map of the city.
 */
struct cell_s {
    int column;           /*!< Column of the cell. */
    int row;              /*!< Row of the cell. */
    cell_type_t type;     /*!< Type of the cell (@see \enum e_cell_type). */
    int current_capacity; /*!< Current capacity of the cell. */
    int nb_of_characters; /*!< Max. number of characters on the cell. */
    citizen_t *occupants; /*!< Array of occupants in the cell. */
};

/**
 * \typedef cell_t
 * \brief Typedef for the cell structure.
 */
typedef struct cell_s cell_t;

/**
 * \struct simulated_clock_s
 * \brief Structure representing a simulated clock.
 */
typedef struct simulated_clock_s {
    int round;   /*!< Current round. */
    int hours;   /*!< Current hours. */
    int minutes; /*!< Current minutes. */
    int days;    /*!< Current days. */
} simulated_clock_t;

/**
 * \struct leaving_time_s
 * \brief Structure representing leaving time.
 */
struct leaving_time_s {
    int leaving_hour;   /*!< The hour of leaving. */
    int leaving_minute; /*!< The minute of leaving. */
};

/**
 * \brief The city map.
 */
struct map_s {
    int columns;                         /*!< The number of columns of the city map. */
    int rows;                            /*!< The number of rows of the city map.*/
    cell_t cells[MAX_COLUMNS][MAX_ROWS]; /*!< Cells that constitute the city map. */
    int mailbox_row;                     /*!< The mailbox row. */
    int mailbox_column;                  /*!< The mailbox column. */
};

/**
 * \struct spy_s
 * \brief The spy information.
 */
struct spy_s {
    int id;                                               /*!< The spy identification number. */
    int health_point;                                     /*!< The spy health_point point.*/
    int location_row;                                     /*!< The spy location row.*/
    int location_column;                                  /*!< The spy location column.*/
    int home_row;                                         /*!< The spy home row.*/
    int home_column;                                      /*!< The spy home column.*/  
    int nb_of_stolen_companies;                           /*!< The number of stolen companies stolen by the spy.*/
    int has_license_to_kill;                              /*!< The spy's authorization to kill.*/
    char stolen_message_content[MAX_LENGTH_OF_MESSAGE];   /*!< The content of stolen message.*/
    cell_t allowed_company[8];                            /*!< The allowed cells around a target company */
    leaving_time_t leaving_time;                          /*!< The time the spy leaves a location.*/
    int turns_spent_scouting;                             /*!< The number of turns spent scouting.*/
    int turns_spent_spotting;                             /*!< The number of turns spent spotting.*/
    int turns_spent_stealing;                             /*!< The number of turns spent stealing.*/
    int turns_spent_shopping;                             /*!< The number of turns spent shopping.*/
    int turns_spent_waiting;                              /*!< The number of turns spent waiting.*/
    bool has_targeted_company;                            /*!< Indicates if the spy has a targeted company.*/
    building_t *targeted_company;                         /*!< Pointer to the targeted company.*/
    int row_in_front_of_targeted_company;                 /*!< Row in front of the targeted company.*/
    int column_in_front_of_targeted_company;              /*!< Column in front of the targeted company.*/
    Node* random_neighbour;                               /*!< Pointer to a random neighboring node.*/
    int row_supermarket;                                  /*!< Row of the supermarket.*/
    int column_supermarket;                               /*!< Column of the supermarket.*/
    bool has_a_message;                                   /*!< Indicates if the spy has a message.*/
    bool has_a_fake_message;                              /*!< Indicates if the spy has a fake message.*/
    characterMovement movement;                           /*!< Information about the spy's movement.*/

    char description[40];                                 /*!< Description of the spy.*/
    state_t *current_state;                               /*!< Current state of the spy.*/
    state_t *resting_at_home;                             /*!< State when the spy is resting at home.*/
    state_t *going_to_spot;                               /*!< State when the spy is going to a spot.*/
    state_t *spotting;                                    /*!< State when the spy is spotting.*/
    state_t *stealing;                                    /*!< State when the spy is stealing.*/
    state_t *scouting;                                    /*!< State when the spy is scouting.*/
    state_t *going_to_send_fake_message;                  /*!< State when the spy is going to send a fake message.*/
    state_t *going_back_home;                             /*!< State when the spy is going back home.*/
    state_t *going_to_send_message;                       /*!< State when the spy is going to send a message.*/
    state_t *sending_message;                            /*!< State when the spy is sending a message.*/
    state_t *resting_at_home_before_going_to_send_message; /*!< State when the spy is resting at home before sending a message.*/
    state_t *waiting_for_residence_to_be_clear;            /*!< State when the spy is waiting for the residence to be clear.*/
    state_t *going_to_supermarket;                         /*!< State when the spy is going to the supermarket.*/
    state_t *doing_some_shopping;                         /*!< State when the spy is doing some shopping.*/
    state_t *arriving_at_mailbox;                         /*!< State when the spy is arriving at the mailbox.*/
    state_t *is_hurt;                                     /*!< State when the spy is hurt.*/
    state_t *is_free;                                     /*!< State when the spy is free.*/
    state_t *riposte;                                     /*!< State when the spy is riposte.*/
    state_t *is_in_conflict;                              /*!< State when the spy is in conflict.*/
    state_t *dying;                                       /*!< State when the spy is dying.*/
    state_t *finished;                                    /*!< State when the spy is finished.*/
};

/**
 * \struct case_officer_s
 * \brief The case_officer information.
 */
struct case_officer_s {
    int id;                                               /*!< The case_officer identification number. */
    int health_point;                                     /*!< The case_officer health_point point.*/
    int location_row;                                     /*!< The case_officer location row.*/
    int location_column;                                  /*!< The case_officer location column.*/
    int home_row;                                         /*!< The case_officer home row.*/
    int home_column;                                      /*!< The case_officer home column.*/  
    int mailbox_row;                                      /*!< The case_officer mailbox row.*/
    int mailbox_column;                                   /*!< The case_officer mailbox column.*/
    int nb_of_outing;                                     /*!< The number of outings of case_officer */
    int row_supermarket;                                  /*!< Row of the supermarket.*/
    int column_supermarket;                               /*!< Column of the supermarket.*/
    leaving_time_t first_leaving_time;                     /*!< The first leaving time of the case officer.*/
    leaving_time_t second_leaving_time;                    /*!< The second leaving time of the case officer.*/
    leaving_time_t shopping_time;                         /*!< The shopping time of the case officer.*/
    int turns_spent_shopping;                             /*!< The number of turns spent shopping.*/
    leaving_time_t messaging_time;                        /*!< The messaging time of the case officer.*/
    char messages[MAX_MESSAGES][MAX_MESSAGE_SIZE];        /*!< Array of messages stored by the case officer.*/
    int message_count;                                    /*!< The count of stored messages.*/
    characterMovement movement;                           /*!< Information about the case officer's movement.*/

    char description[30];                                 /*!< Description of the case officer.*/
    state_t *current_state;                               /*!< Current state of the case officer.*/
    state_t *resting_at_home;                             /*!< State when the case officer is resting at home.*/
    state_t *going_to_supermarket;                        /*!< State when the case officer is going to the supermarket.*/
    state_t *doing_some_shopping;                         /*!< State when the case officer is doing some shopping.*/
    state_t *going_back_home;                             /*!< State when the case officer is going back home.*/
    state_t *going_to_mailbox;                            /*!< State when the case officer is going to the mailbox.*/
    state_t *sending_messages;                            /*!< State when the case officer is sending messages.*/
    state_t *recovering_messages;                         /*!< State when the case officer is recovering messages.*/
};

/**
 * \struct counter_intelligence_officer_s
 * \brief The counterintelligence_officer information.
 */
struct counter_intelligence_officer_s {
    int id;                                               /*!< The counterintelligence_officer identification number. */
    int health_point;                                     /*!< The counterintelligence_officer health_point point.*/
    int location_row;                                     /*!< The counterintelligence_officer location row.*/
    int location_column;                                  /*!< The counterintelligence_officer location column.*/
    int city_hall_row;                                    /*!< The counterintelligence_officer home row.*/
    int city_hall_column;                                 /*!< The counterintelligence_officer home column.*/  
    int mailbox_row;                                      /*!< The counterintelligence_officer mailbox row.*/
    int mailbox_column;                                   /*!< The counterintelligence_officer mailbox column.*/
    int targeted_character_id;                            /*!< The targeted character id.*/
    int targeted_character_row;                           /*!< The targeted character row.*/
    int targeted_character_column;                        /*!< The targeted character column.*/
    char description[40];                                 /*!< Description of the counterintelligence officer.*/
    state_t *monitoring;                                  /*!< State when the counterintelligence officer is monitoring.*/
    state_t *going_to_suspect_place;                      /*!< State when the counterintelligence officer is going to a suspect place.*/
    state_t *hiding;                                      /*!< State when the counterintelligence officer is hiding.*/
    state_t *going_back_to_monitor;                       /*!< State when the counterintelligence officer is going back to monitoring.*/
    state_t *waiting_for_spy_to_steal;                    /*!< State when the counterintelligence officer is waiting for the spy to steal.*/
    state_t *following_spy;                               /*!< State when the counterintelligence officer is following the spy.*/
    state_t *waiting_for_spy_to_send_message;             /*!< State when the counterintelligence officer is waiting for the spy to send a message.*/
    state_t *searching_for_mailbox;                       /*!< State when the counterintelligence officer is searching for a mailbox.*/
    state_t *recovering_messages;                         /*!< State when the counterintelligence officer is recovering messages.*/
    state_t *current_state;                               /*!< Current state of the counterintelligence officer.*/
    state_t *going_to_search_for_mailbox;                 /*!< State when the counterintelligence officer is going to search for a mailbox.*/
    bool has_found_mailbox;                               /*!< Indicates if the counterintelligence officer has found a mailbox.*/
    bool has_found_mailbox_location;                      /*!< Indicates if the counterintelligence officer has found the mailbox location.*/
    bool new_day;                                         /*!< Indicates if it's a new day for the counterintelligence officer.*/
    leaving_time_t leaving_time;                          /*!< The leaving time of the counterintelligence officer.*/
};


/**
 * @brief A structure representing a node in a grid.
 * 
 * This structure is used for pathfinding and representing positions in a grid.
 */
struct Node {
    int position[2]; ///< Position of the node in the grid.
    double g;   ///< Cost of the path from the start node to this node.
    double h;   ///< Heuristic cost estimate to reach the goal from this node.
    double f;   ///< Total cost (f = g + h).
    struct Node* parent;  ///< Parent node in the path.
};

/**
 * @brief A structure representing a path in a grid.
 * 
 * This structure is used to store the sequence of nodes representing a path.
 */
typedef struct Path {
    Node **nodes; ///< Array of nodes representing the path.
    int length;   ///< Length of the path.
} Path;

/**
 * @brief A structure representing a citizen in the simulation.
 * 
 * This structure stores information about a citizen, including their state, position, and properties.
 */
struct citizen_s {
    unsigned int id; ///< Unique identifier for the citizen.
    int position[2]; ///< Position of the citizen in the grid.
    unsigned int health; ///< Health points of the citizen.
    int current_step; ///< Current step in the path (if any).
    int is_coming_from_company; ///< Flag indicating if the citizen is returning from a company.
    citizen_type_t type; ///< Type of the citizen (e.g., normal, spy, officer).
    building_t *workplace; ///< Pointer to the citizen's workplace.
    building_t *supermarket; ///< Pointer to a random supermarket in the city.
    int time_spent_shopping; ///< Time spent shopping, up to 30 minutes.
    home_t *home; ///< Pointer to the citizen's home.
    int visited_cells[MAX_COLUMNS][MAX_ROWS]; ///< Grid to track visited cells.

    Path *path_to_work; ///< Path to the workplace.
    Path *path_to_supermarket; ///< Path to the supermarket.
    Path *path_from_supermarket_to_home; ///< Path from the supermarket to home.

    state_t *current_state; ///< Current state of the citizen.
    state_t *resting_at_home; ///< State when resting at home.
    state_t *going_to_company; ///< State when going to the company.
    state_t *working; ///< State when working.
    state_t *going_to_supermarket; ///< State when going to the supermarket.
    state_t *doing_some_shopping; ///< State when doing shopping.
    state_t *going_back_home; ///< State when going back home.
    state_t *dying; ///< State when dying.
    state_t *finished; ///< State when finished.
    characterMovement movement; ///< Movement behavior of the citizen.
};

/**
 * @brief A structure representing a mailbox in the simulation.
 * 
 * This structure stores information about a mailbox, including its position and stored messages.
 */
struct mailbox_s {
    bool is_occupied; ///< Flag indicating if the mailbox is occupied.
    int row; ///< Row position of the mailbox.
    int column; ///< Column position of the mailbox.
    int row_in_front; ///< Row in front of the mailbox.
    int column_in_front; ///< Column in front of the mailbox.
    char messages[MAX_MESSAGES][MAX_MESSAGE_SIZE]; ///< Array of messages stored in the mailbox.
    int message_count; ///< Number of messages in the mailbox.
    int priority[MAX_MESSAGES]; ///< Priority of each message.
};

/**
 * @brief A structure representing a building in the simulation.
 * 
 * This structure stores information about a building, such as its type, position, and associated citizens.
 */
struct building_s {
    int position[2]; ///< Position of the building.
    building_type_t type; ///< Type of the building (e.g., company, supermarket).
    cell_type_t cell_type; ///< Cell type of the building.
    unsigned int nb_citizen; ///< Number of citizens associated with this building.
    unsigned int max_capacity; ///< Maximum capacity of the building.
    unsigned int max_workers; ///< Maximum number of workers for a company building.
    unsigned int min_workers; ///< Minimum number of workers for a company building.
    unsigned int nb_workers; ///< Current number of workers in the building.
    citizen_t *citizens; ///< Array of pointers to citizens associated with this building.
};

/**
 * @brief A structure representing a home in the simulation.
 * 
 * This structure stores information about a home, including its position, capacity, and associated citizens.
 */
struct home_s {
    int position[2]; ///< Position of the home.
    int nb_citizen; ///< Number of citizens living in the home.
    int max_capacity; ///< Maximum capacity of the home.
    bool has_mailbox; ///< Flag indicating if the home has a mailbox.
    citizen_t *citizens; ///< Array of pointers to citizens living in this home.
    mailbox_t mailbox; ///< Mailbox associated with the home.
};

/**
 * @brief Structure representing the shared memory used by all processes in the simulation.
 * 
 * This structure includes various components of the simulation, such as the map, citizens, spies, and more.
 */
struct memory_s {
    int memory_has_changed;    /*!< Flag set to 1 when the memory is modified. */
    int simulation_has_ended;  /*!< Flag indicating the end state of the simulation:
                                * - 0: not ended;
                                * - 1: spy network has fled (wins);
                                * - 2: counterintelligence officer discovered the mailbox (wins);
                                * - 3: counterintelligence officer did not discover mailbox (spy network wins).
                                */
    map_t map; ///< Map of the simulation.
    spy_t spies[SPIES_COUNT]; ///< Array of spies in the simulation.
    case_officer_t case_officer; ///< Case officer in the simulation.
    counter_intelligence_officer_t counter_intelligence_officer; ///< Counterintelligence officer in the simulation.
    simulated_clock_t timer; ///< Clock keeping track of the simulation time.
    char messages[MAX_MESSAGES][MAX_MESSAGE_SIZE]; ///< Array of messages in the simulation.
    int message_count; ///< Count of messages in the simulation.
    int end_round; ///< Round number marking the end of the simulation.
    pid_t pids[NB_PROCESS - 1]; ///< Array of process IDs.
    mq_t mqInfo; ///< Message queue information.
    citizen_t citizens[CITIZENS_COUNT]; ///< Array of citizens in the simulation.
    int walking_citizens; ///< Count of citizens currently walking.
    int at_home_citizens; ///< Count of citizens currently at home.
    int at_work_citizens; ///< Count of citizens currently at work.
    surveillanceNetwork_t surveillanceNetwork; ///< Surveillance network in the simulation.
    home_t homes[NB_HOMES]; ///< Array of homes in the simulation.
    building_t companies[NB_WORKPLACES]; ///< Array of workplaces in the simulation.
};


#endif /* MEMORY_H */