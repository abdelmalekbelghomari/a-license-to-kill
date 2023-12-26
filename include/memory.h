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


#include "cell.h"
#include "common.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <mqueue.h>


#include "cell.h"
#include "citizen_manager.h"

#define NB_CITIZEN 127 /* 127 citizens in the city */
#define CITIZENS_COUNT 127 /* 127 citizens in the city */
#define MAX_ROWS 7
#define MAX_COLUMNS 7
#define MAX_ROUNDS 2016


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

/**
 * \brief The city map.
 */
struct map_s {
    int columns;                         /*!< The number of columns of the city map. */
    int rows;                            /*!< The number of rows of the city map.*/
    cell_t cells[MAX_COLUMNS][MAX_ROWS]; /*!< Cells that constitute the city map. */
};

struct SpyInfo{
    int id;                                               /*!< The spy identification number. */
    int health_point;                                     /*!< The spy health point.*/
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
    int health_point;                                     /*!< The case_officer health point.*/
    int location_row;                                     /*!< The case_officer location row.*/
    int location_column;                                  /*!< The case_officer location column.*/
    int home_row;                                         /*!< The case_officer home row.*/
    int home_column;                                      /*!< The case_officer home column.*/  
    int mailbox_row;                                      /*!< The case_officer home row.*/
    int mailbox_column;                                   /*!< The case_officer home column.*/
};

struct CounterIntelligenceOfficer {
    int id;                                               /*!< The counterintelligence_officer identification number. */
    int health_point;                                     /*!< The counterintelligence_officer health point.*/
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
    int health_point;                                     /*!< The spy health point.*/
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
    int health_point;                                     /*!< The case_officer health point.*/
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
    int health_point;                                     /*!< The counterintelligence_officer health point.*/
    int location_row;                                     /*!< The counterintelligence_officer location row.*/
    int location_column;                                  /*!< The counterintelligence_officer location column.*/
    int city_hall_row;                                    /*!< The counterintelligence_officer home row.*/
    int city_hall_column;                                 /*!< The counterintelligence_officer home column.*/  
    int mailbox_row;                                      /*!< The counterintelligence_officer home row.*/
    int mailbox_column;                                   /*!< The counterintelligence_officer home column.*/
    int targeted_character_id;                            /*!< The targeted character id.*/
}; 

typedef struct time_s {
    int round;
    int hours;
    int minutes;
    int days;
}time_t;

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
    citizen_t citizens[NB_CITIZEN];
    int walking_citizens;
    int at_home_citizens;
    int at_work_citizens;
    surveillanceNetwork_t surveillanceNetwork;
};




#endif /* MEMORY_H */

