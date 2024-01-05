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


#define CITIZENS_COUNT 127 /* 127 citizens in the city */
#define MAX_ROWS 7
#define MAX_COLUMNS 7
#define MAX_ROUNDS 2016

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

#define CITY_HALL_ROW 3
#define CITY_HALL_COLUMN 3

/**
 * \file memory.h
 *
 * Defines structures and functions used to manipulate our shared memory.
 */

typedef struct map_s map_t;
typedef struct mq_s mq_t;
typedef struct memory_s memory_t;
typedef struct spy_s spy_t;
typedef struct case_officer_s case_officer_t;
typedef struct counterintelligence_officer_s counterintelligence_officer_t;
typedef struct SpyInfo spyInfo;
typedef struct CaseOfficerInfo caseOfficerInfo;
typedef struct CounterIntelligenceOfficer counterIntelligenceOfficer;
typedef struct state_s state_t;
typedef struct state_s state_t;
typedef struct building_s building_t;
typedef struct home_s home_t;
typedef struct citizen_s citizen_t;
typedef struct SurveillanceNetwork surveillanceNetwork_t;
typedef enum cell_type_e cell_type_t;
typedef struct cell_s cell_t;


/**
 * \brief The type of citizens within the city.
 */
typedef enum citizen_type_e {
    NORMAL,                        /*!< A normal citizen. */
    SPY,                           /*!< A spy. */
    CASE_OFFICER,                  /*!< A case officer. */
    COUNTER_INTELLIGENCE_OFFICER  /*!< A counterintelligence officer. */
} citizen_type_t;

/**
 * \brief The type of buildings within the city map.
 */
typedef enum building_type_e {
    CORPORATION, /*!< A corporation building. */
    STORE,       /*!< A store building. */
    HALL         /*!< A hall building. */
} building_type_t;

/**
 * \brief Message Queue structure for inter-process communication.
 */
struct mq_s {
    mqd_t mq; /*!< Message Queue descriptor. */
};

/**
 * \brief Structure for surveillance devices on each cell.
 */
typedef struct {
    int standard_camera; /*!< Status of the standard camera (enabled/disabled). */
    int infrared_camera; /*!< Status of the infrared camera (enabled/disabled). */
    int lidar;           /*!< Status of the lidar (enabled/disabled). */
} SurveillanceDevices;

/**
 * \brief Structure for the surveillance AI.
 */
typedef struct {
    int suspicious_movement; /*!< Indicator of suspicious movement (boolean). */
} SurveillanceAI;

/**
 * \brief Global structure for the surveillance network covering the entire city.
 */
struct SurveillanceNetwork {
    SurveillanceDevices devices[MAX_ROWS][MAX_COLUMNS]; /*!< 2D array covering all cells of the city. */
    SurveillanceAI surveillanceAI;                     /*!< Surveillance AI. */
};

/**
 * \brief The type of states a citizen can have.
 */
struct state_s {
    int id;

    /*!< Function pointer to change the state of a citizen. */
    void (*change_state)(citizen_t *, state_t *);

    /*!< Function pointer to perform an action based on the citizen's state. */
    state_t *(*action)(citizen_t *);
};

/**
 * \brief The type of cells within the city map.
 */
enum cell_type_e {
    WASTELAND,            /*!< An empty cell. */
    RESIDENTIAL_BUILDING, /*!< A cell that contains a residential building. */
    CITY_HALL,            /*!< A cell that contains the City Hall. */
    COMPANY,              /*!< A cell that contains a company. */
    SUPERMARKET           /*!< A cell that contains a supermarket. */
};

/**
 * \brief A cell within the map of the city.
 */
struct cell_s {
    int column;           /*!< Column of the cell. */
    int row;              /*!< Row of the cell. */
    cell_type_t type;     /*!< Type of the cell (@see \enum e_cell_type). */
    int current_capacity;
    int nb_of_characters; /*!< Max. number of characters on the cell. */
    citizen_t *occupants;
};

/**
 * \brief Structure representing the simulated clock.
 */
typedef struct simulated_clock_s {
    int round;    /*!< Current round of the simulation. */
    int hours;    /*!< Current hours in the simulation. */
    int minutes;  /*!< Current minutes in the simulation. */
    int days;     /*!< Current days in the simulation. */
} simulated_clock_t;

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

struct SpyInfo{
    int id;                                               /*!< The spy identification number. */
    int health_point;                                     /*!< The spy health_point point.*/
    int location_row;                                     /*!< The spy location row.*/
    int location_column;                                  /*!< The spy location column.*/
    int home_row;                                         /*!< The spy home row.*/
    int home_column;                                      /*!< The spy home column.*/  
    int nb_of_stolen_companies;                           /*!< The number of stolen companies stolen by the spy.*/
    int has_license_to_kill;                              /*!< The spy's authorization to kill.*/
    char stolen_message_content[MAX_LENGTH_OF_MESSAGE];   /*!< The content of stolen message.*/
    cell_t allowed_company[8];
};


struct CaseOfficerInfo {
    int id;                                               /*!< The case_officer identification number. */
    int health_point;                                     /*!< The case_officer health_point point.*/
    int location_row;                                     /*!< The case_officer location row.*/
    int location_column;                                  /*!< The case_officer location column.*/
    int home_row;                                         /*!< The case_officer home row.*/
    int home_column;                                      /*!< The case_officer home column.*/  
    int mailbox_row;                                      /*!< The case_officer home row.*/
    int mailbox_column;                                   /*!< The case_officer home column.*/
};

struct CounterIntelligenceOfficer {
    int id;                                               /*!< The counterintelligence_officer identification number. */
    int health_point;                                     /*!< The counterintelligence_officer health_point point.*/
    int location_row;                                     /*!< The counterintelligence_officer location row.*/
    int location_column;                                  /*!< The counterintelligence_officer location column.*/
    int city_hall_row;                                    /*!< The counterintelligence_officer home row.*/
    int city_hall_column;                                 /*!< The counterintelligence_officer home column.*/  
    int mailbox_row;                                      /*!< The counterintelligence_officer home row.*/
    int mailbox_column;                                   /*!< The counterintelligence_officer home column.*/
    int targeted_character_id;                           /*!< The targeted character id.*/
}; 

/**
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
    cell_t allowed_company[8];                            /*!< The allowed cells around a targer company */
};

/**
 * \brief The case_officer information.
 */
struct case_officer_s {
    int id;                                               /*!< The case_officer identification number. */
    int health_point;                                     /*!< The case_officer health_point point.*/
    int location_row;                                     /*!< The case_officer location row.*/
    int location_column;                                  /*!< The case_officer location column.*/
    int home_row;                                         /*!< The case_officer home row.*/
    int home_column;                                      /*!< The case_officer home column.*/  
    int mailbox_row;                                      /*!< The case_officer home row.*/
    int mailbox_column;                                   /*!< The case_officer home column.*/
    int nb_of_outing;                                     /*!< The number of outing of case_officer */
};

/**
 * \brief The counterintelligence_officer information.
 */
struct counterintelligence_officer_s {
    int id;                                               /*!< The counterintelligence_officer identification number. */
    int health_point;                                     /*!< The counterintelligence_officer health_point point.*/
    int location_row;                                     /*!< The counterintelligence_officer location row.*/
    int location_column;                                  /*!< The counterintelligence_officer location column.*/
    int city_hall_row;                                    /*!< The counterintelligence_officer home row.*/
    int city_hall_column;                                 /*!< The counterintelligence_officer home column.*/  
    int mailbox_row;                                      /*!< The counterintelligence_officer home row.*/
    int mailbox_column;                                   /*!< The counterintelligence_officer home column.*/
    int targeted_character_id;                            /*!< The targeted character id.*/
}; 



/**
 * \brief Structure representing a citizen.
 */
struct citizen_s {
    unsigned int id;                  /*!< Unique identifier for the citizen. */
    unsigned int position[2];         /*!< Current position of the citizen [x, y]. */
    unsigned int health;              /*!< Health status of the citizen. */
    citizen_type_t type;              /*!< Type of the citizen. */
    building_t *workplace;            /*!< Workplace of the citizen. */
    building_t *supermarket;          /*!< A random supermarket of the city. */
    home_t *home;                     /*!< Home of the citizen. */

    state_t *current_state;           /*!< Current state of the citizen. */
    state_t *next_state;              /*!< Next state of the citizen. */
    state_t *resting_at_home;         /*!< State when the citizen is resting at home. */
    state_t *going_to_company;        /*!< State when the citizen is going to the workplace. */
    state_t *working;                 /*!< State when the citizen is working. */
    state_t *going_to_supermarket;    /*!< State when the citizen is going to the supermarket. */
    state_t *doing_some_shopping;     /*!< State when the citizen is doing some shopping. */
    state_t *going_back_home;         /*!< State when the citizen is going back home. */
    state_t *dying;                   /*!< State when the citizen is dying. */

    void (*change_state)(citizen_t *, state_t *); /*!< Function pointer to change the state of a citizen. */
    void (*begin)(citizen_t *);                   /*!< Function pointer for the beginning of a citizen's action. */
    void (*end)(citizen_t *);                     /*!< Function pointer for the end of a citizen's action. */
    void (*step)(citizen_t *);                    /*!< Function pointer for a step in a citizen's action. */
};

/**
 * \brief Structure representing a building.
 */
struct building_s {
    unsigned int position[2];       /*!< Current position of the building [x, y]. */
    building_type_t type;           /*!< Type of the building (CORPORATION, STORE, etc.). */
    cell_t cell_type;               /*!< Type of the cell within the building. */
    unsigned int nb_citizen;        /*!< Number of citizens in the building. */
    unsigned int max_capacity;      /*!< Maximum capacity of the building. */
    unsigned int max_workers;       /*!< Maximum number of workers in the building. */
    unsigned int min_workers;       /*!< Minimum number of workers required in the building. */
    unsigned int nb_workers;        /*!< Current number of workers in the building. */
    citizen_t *citizens;            /*!< Array of citizens in the building. */
    void (*add_citizen)(building_t *, citizen_t *);     /*!< Function pointer to add a citizen to the building. */
    void (*remove_citizen)(building_t *, citizen_t *);  /*!< Function pointer to remove a citizen from the building. */
};

/**
 * \brief Structure representing a home.
 */
struct home_s {
    unsigned int position[2];       /*!< Current position of the home [x, y]. */
    unsigned int nb_citizen;        /*!< Number of citizens in the home. */
    unsigned int max_capacity;      /*!< Maximum capacity of the home. */
    citizen_t *citizens;            /*!< Array of citizens in the home. */
    void (*add_citizen)(home_t *, citizen_t *);      /*!< Function pointer to add a citizen to the home. */
    void (*remove_citizen)(home_t *, citizen_t *);   /*!< Function pointer to remove a citizen from the home. */
};

/**
 * \brief Shared memory used by all processes.
 */
struct memory_s {
    int memory_has_changed;                    /*!< This flag is set to 1 when the memory has changed. */
    int simulation_has_ended;                  /*!< This flag is set to the following values:
                                                * - 0: has not ended;
                                                * - 1: the spy network has fled. It wins!
                                                * - 2: the counterintelligence officer has discovered the mailbox. He wins.
                                                * - 3: the counterintelligence officer did not discover the mailbox. The spy network
                                                *      wins!
                                                */
    map_t map;                                 /*!< The map of the city. */
    spy_t spies[3];                            /*!< Array of spies in the simulation (up to 3 spies). */
    case_officer_t case_officer;               /*!< The case officer. */
    counterintelligence_officer_t counterintelligence_officer; /*!< The counterintelligence officer. */
    simulated_clock_t timer;                   /*!< The simulated clock used for tracking time in the simulation. */
    int end_round;                             /*!< The ending round of the simulation. */
    pid_t pids[7];                             /*!< Array of process IDs for communication between processes. */
    mq_t mqInfo;                               /*!< The Message Queue information for inter-process communication. */
    citizen_t citizens[CITIZENS_COUNT];        /*!< Array of citizens in the simulation. */
    int walking_citizens;                      /*!< Number of citizens currently walking in the city. */
    int at_home_citizens;                      /*!< Number of citizens currently at their homes. */
    int at_work_citizens;                      /*!< Number of citizens currently at their workplaces. */
    surveillanceNetwork_t surveillanceNetwork; /*!< The surveillance network structure. */
    home_t homes[NB_HOMES];                    /*!< Array of homes. */
    building_t companies[NB_WORKPLACES];       /*!< Array of companies. */

};

#endif /* MEMORY_H */