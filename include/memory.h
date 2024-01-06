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
#include <semaphore.h>
#include <stdbool.h>


#define CITIZENS_COUNT 127 /* 127 citizens in the city */
#define MAX_ROWS 7
#define MAX_COLUMNS 7
#define MAX_ROUNDS 2016
#define NUM_CITIZENS 127
#define SPIES_COUNT 3
#define MAX_MESSAGE_SIZE 128
#define MAX_MESSAGES 10000
#define SHIFT 3

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
 * Defines structures and functions used to manipulate our shared memory.
 */

typedef struct map_s map_t;
typedef struct mq_s mq_t;
typedef struct memory_s memory_t;
typedef struct spy_s spy_t;
typedef struct case_officer_s case_officer_t;
typedef struct counter_intelligence_officer_s counter_intelligence_officer_t;
typedef struct SpyInfo spyInfo;
typedef struct CaseOfficerInfo caseOfficerInfo;
typedef struct CounterIntelligenceOfficer counterIntelligenceOfficer;
typedef struct state_s state_t;
typedef struct building_s building_t;
typedef struct home_s home_t;
typedef struct citizen_s citizen_t;
typedef struct mailbox_s mailbox_t;
typedef struct leaving_time_s leaving_time_t;
typedef struct Node Node;





typedef struct {
    int priority;
    char content[MAX_MESSAGE_SIZE];
} SpyMessage;



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

typedef struct {
    int currentX, currentY; // Position actuelle
    int previousX, previousY; // Position précédente
    int historyX[7], historyY[7];
} characterMovement;

typedef struct {
    int standard_camera; // 0 = désactivée, 1 = activée
    int infrared_camera; // 0 = désactivée, 1 = activée
} surveillanceCameras;

typedef struct {
    int numberOfPeople; // Nombre de personnes dans la case
} cellData;

typedef enum {
    SUSPECT_NONE,
    SUSPECT_CITIZEN,
    SUSPECT_SPY,
    SUSPECT_CASE_OFFICER
} suspect_type_t;

typedef union {
    citizen_t *citizen;
    spy_t *spy;
    case_officer_t *case_officer;
} suspect_t;

typedef struct {
    bool suspicious_movement;
    suspect_t suspect;
    suspect_type_t suspect_type;
} surveillance_AI;

typedef struct {
    cellData cells[MAX_COLUMNS][MAX_ROWS]; // Données pour chaque cellule
    surveillanceCameras cameras; // Caméras de surveillance pour l'ensemble du réseau
    surveillance_AI surveillanceAI; // IA de surveillance
} surveillanceNetwork_t;

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

struct leaving_time_s {
    int leaving_hour;
    int leaving_minute;
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
    leaving_time_t leaving_time;
    int turns_spent_scouting;
    int turns_spent_spotting;
    int turns_spent_stealing;
    int turns_spent_shopping;
    int turns_spent_waiting;
    building_t *targeted_company;
    int x_in_front_of_targeted_company;
    int y_in_front_of_targeted_company;
    Node* random_neighbour;
    int x_supermarket;
    int y_supermarket;
    bool has_a_message;
    bool has_a_fake_message;
    characterMovement movement;


    state_t *current_state;
    state_t *resting_at_home;
    state_t *going_to_spot;
    state_t *spotting;
    state_t *stealing;
    state_t *scouting;
    state_t *going_to_send_fake_message;
    state_t *going_back_home;
    state_t *going_to_send_message;
    state_t *sending_message;
    state_t *resting_at_home_before_going_to_send_message;
    state_t *waiting_for_residence_to_be_clear;
    state_t *going_to_supermarket;
    state_t *doing_some_shopping;
    state_t *arriving_at_mailbox;
    state_t *is_hurt;
    state_t *is_free;
    state_t *riposte;
    state_t *is_in_conflict;
    state_t *dying;
    state_t *finished;
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
    int x_supermarket;
    int y_supermarket;
    leaving_time_t first_leaving_time;
    leaving_time_t second_leaving_time;
    leaving_time_t shopping_time;
    int turns_spent_shopping;
    leaving_time_t messaging_time;
    char messages[MAX_MESSAGES][MAX_MESSAGE_SIZE];
    int message_count;
    characterMovement movement;

    state_t *current_state;
    state_t *resting_at_home;
    state_t *going_to_supermarket;
    state_t *doing_some_shopping;
    state_t *going_back_home;
    state_t *going_to_mailbox;
    state_t *sending_messages;
    state_t *recovering_messages;
};

/**
 * \brief The counterintelligence_officer information.
 */
struct counter_intelligence_officer_s {
    int id;                                               /*!< The counterintelligence_officer identification number. */
    int health_point;                                     /*!< The counterintelligence_officer health_point point.*/
    int location_row;                                     /*!< The counterintelligence_officer location row.*/
    int location_column;                                  /*!< The counterintelligence_officer location column.*/
    int city_hall_row;                                    /*!< The counterintelligence_officer home row.*/
    int city_hall_column;                                 /*!< The counterintelligence_officer home column.*/  
    int mailbox_row;                                      /*!< The counterintelligence_officer home row.*/
    int mailbox_column;                                   /*!< The counterintelligence_officer home column.*/
    int targeted_character_id;                            /*!< The targeted character id.*/

    state_t *monitoring;
    state_t *going_to_suspect_place;
    state_t *hiding;
    state_t *going_back_to_monitor;
    state_t *waiting_for_spy_to_steal;
    state_t *following_spy;
    state_t *waiting_for_spy_to_send_message;
    state_t *searching_for_mailbox;
    state_t *recovering_messages;
    state_t *current_state;
    state_t *going_to_search_for_mailbox;

    bool has_found_mailbox;
    bool has_found_mailbox_location;
    bool new_day;
    leaving_time_t leaving_time;



}; 

struct Node {
    int position[2]; // Position du nœud dans la grille
    double g;   // Coût du chemin 
    double h;   // Coût heuristique 
    double f;   // Score total (f = g + h)
    struct Node* parent;  // Parent du nœud dans le chemin
};

typedef struct Path {
    Node **nodes; // Chemin 
    int length;   // Longueur du chemin 
} Path;

struct citizen_s {
    unsigned int id;
    int position [2];
    unsigned int health;
    int current_step;
    int is_coming_from_company;
    citizen_type_t type;
    building_t *workplace;
    building_t *supermarket; /*a random supermarket of the city*/
    int time_spent_shopping; /* let's say no more than 30 minutes*/
    home_t *home;
    int visited_cells[MAX_COLUMNS][MAX_ROWS];

    Path *path_to_work;
    Path *path_to_supermarket;
    Path *path_from_supermarket_to_home;

    state_t *current_state;
    state_t *resting_at_home;
    state_t *going_to_company;
    state_t *working;
    state_t *going_to_supermarket;
    state_t *doing_some_shopping;
    state_t *going_back_home;
    state_t *dying;
    state_t *finished;
    characterMovement movement;
    // state_t change_state[DAILY_CITIZEN_STATES]

    // void (*change_state)(citizen_t *, state_t *);
    // void (*begin)(citizen_t *);
    // void (*end)(citizen_t *);
    // void (*step)(citizen_t *);
};

struct mailbox_s{
    bool is_occupied;
    int x;
    int y;
    int x_in_front;
    int y_in_front;
    char messages[MAX_MESSAGES][MAX_MESSAGE_SIZE];
    int message_count;
    int priority[MAX_MESSAGES];

};

struct building_s {
    int position[2];
    building_type_t type;
    cell_type_t cell_type;
    unsigned int nb_citizen;
    unsigned int max_capacity;
    unsigned int max_workers;
    unsigned int min_workers;
    unsigned int nb_workers;
    citizen_t *citizens;
    // void (*add_citizen)(building_t *, citizen_t *);
    // void (*remove_citizen)(building_t *, citizen_t *);
};

struct home_s {
    int position[2];
    int nb_citizen;
    int max_capacity;
    bool has_mailbox;
    citizen_t *citizens;
    mailbox_t mailbox;
    // void (*add_citizen)(home_t *, citizen_t *);
    // void (*remove_citizen)(home_t *, citizen_t *);
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
    counter_intelligence_officer_t counter_intelligence_officer;
    simulated_clock_t timer;
    char messages[MAX_MESSAGES][MAX_MESSAGE_SIZE];
    int message_count;
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