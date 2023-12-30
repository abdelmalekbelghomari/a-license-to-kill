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

#define NB_CITIZEN 127
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


#define CITIZENS_COUNT 132
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

typedef enum citizen_type_e {
    NORMAL,
    SPY,
    CASE_OFFICER,
    COUNTER_INTELLIGENCE_OFFICER
} citizen_type_t;

typedef enum building_type_e {
    CORPORATION,
    STORE,
    HALL
}building_type_t;

struct mq_s {
    mqd_t mq;
};
// Structure for surveillance devices on each cell
typedef struct {
    int standard_camera; // Status of the standard camera (enabled/disabled)
    int infrared_camera; // Status of the infrared camera (enabled/disabled)
    int lidar; // Status of the lidar (enabled/disabled)
} SurveillanceDevices;

// Structure for the surveillance AI
typedef struct {
    int suspicious_movement; // Indicator of suspicious movement (boolean)
} SurveillanceAI;

// Global structure for surveillance network
struct SurveillanceNetwork {
    SurveillanceDevices devices[MAX_ROWS][MAX_COLUMNS]; // 2D array covering all cells of the city
    SurveillanceAI surveillanceAI; // Surveillance AI
};
typedef struct SurveillanceNetwork surveillanceNetwork_t;

struct state_s {
    int id;

    void (*change_state)(citizen_t *, state_t *);
    state_t *(*action)(citizen_t *);
};

/**
 * The type of cells within the city map.
 */
enum cell_type_e {
    WASTELAND,            /*!< An empty cell. */
    RESIDENTIAL_BUILDING, /*!< A cell that contains a residential building. */
    CITY_HALL,            /*!< A cell that contains the City Hall. */
    COMPANY,              /*!< A cell that contains a company. */
    SUPERMARKET           /*!< A cell that contains a supermarket. */
};

typedef enum cell_type_e cell_type_t;
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
typedef struct cell_s cell_t;

typedef struct simulated_clock_s {
    int round;
    int hours;
    int minutes;
    int days;
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




struct citizen_s {
    unsigned int id;
    unsigned int position [2];
    unsigned int health ;
    citizen_type_t type;
    building_t *workplace;
    building_t *supermarket; /*a random supermarket of the city*/
    home_t *home; 

    state_t *current_state;
    state_t *next_state;
    state_t *resting_at_home;
    state_t *going_to_company;
    state_t *working;
    state_t *going_to_supermarket;
    state_t *doing_some_shopping;
    state_t *going_back_home;
    state_t *dying;

    void (*change_state)(citizen_t *, state_t *);
    void (*begin)(citizen_t *);
    void (*end)(citizen_t *);
    void (*step)(citizen_t *);
};

struct building_s {
    unsigned int position[2];
    building_type_t type;
    cell_t cell_type;
    unsigned int nb_citizen;
    unsigned int max_capacity;
    unsigned int max_workers;
    unsigned int min_workers;
    unsigned int nb_workers;
    citizen_t *citizens[CITIZENS_COUNT];
    void (*add_citizen)(building_t *, citizen_t *);
    void (*remove_citizen)(building_t *, citizen_t *);
};

struct home_s {
    unsigned int position[2];
    unsigned int nb_citizen;
    unsigned int max_capacity;
    citizen_t *citizens[CITIZENS_COUNT];
    void (*add_citizen)(home_t *, citizen_t *);
    void (*remove_citizen)(home_t *, citizen_t *);
};

/**
 * \brief Shared memory used by all processes.
 */
struct memory_s {
    int memory_has_changed;    /*!< This flag is set to 1 when the memory has changed. */
    int simulation_has_ended;  /*!< This flag is set to the following values:
                                * - 0: has not ended;
                                * - 1: the spy network has fled. It wins!
                                * - 2: the counterintelligence officer has discovered the mailbox. He wins.
                                * - 3: the counterintelligence officer did not discover the mailbox. The spy network
                                *      wins!
                                */
    map_t map;
    spy_t spies[3];
    case_officer_t case_officer;
    counterintelligence_officer_t counterintelligence_officer;
    simulated_clock_t timer;
    int end_round;
    pid_t pids[7];
    mq_t mqInfo;
    citizen_t citizens[CITIZENS_COUNT];
    int walking_citizens;
    int at_home_citizens;
    int at_work_citizens;
    surveillanceNetwork_t surveillanceNetwork;
    home_t homes[NB_HOMES];
    building_t companies[NB_WORKPLACES];

};

#endif /* MEMORY_H */