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

#define CITIZENS_COUNT 131

/**
 * \file memory.h
 *
 * Defines structures and functions used to manipulate our shared memory.
 */

typedef struct map_s map_t;
typedef struct memory_s memory_t;

/**
 * \brief The city map.
 */
struct map_s {
    int columns;                         /*!< The number of columns of the city map. */
    int rows;                            /*!< The number of rows of the city map.*/
    cell_t cells[MAX_COLUMNS][MAX_ROWS]; /*!< Cells that constitute the city map. */
};


enum citizen_type_e {
    NORMAL,
    SPY,
    CASE_OFFICER,
    COUNTER_INTELLIGENCE_OFFICER
};

typedef enum citizen_type_e citizen_type_t;

struct Citizen {
    int id;
    citizen_type_t type;
    int health;
    int positionX;
    int positionY;
    cell_type_t currentBuilding;
    state_t state;
};

typedef struct Citizen citizen_t;

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
                               
    map_t cityMap; 
    citizen_t citizens[CITIZENS_COUNT];
    surveillanceNetwork_t surveillanceNetwork;
    

};

#endif /* MEMORY_H */
