#ifndef ENEMY_SPY_NETWORK_H
#define ENEMY_SPY_NETWORK_H

#include <pthread.h>

#define MAX_AGENTS 4
#define MAX_BUILDINGS
#define MAX_CITIZENS


// Define the structure for agents (spies and officer)
struct Agent {
    int id;                 // Unique identifier for the agent
    int type;               // 0 for citizen, 1 for officer traitant, 2 for agent source
    int hasLicense;         // 1 if the agent has a license to kill, 0 otherwise
    int position[2]         // coordinates in the city grid
    int health;             // Current health points
    int maxHealth;          // Maximum health points
    int state;              // 0 for healthy, 1 for injured, 2 for dead
    int remainingRest;      // Remaining rest rounds needed for recovery
    int currentBuilding;    // ID of the current building (if inside), -1 otherwise
    // Other necessary fields according to the requirements
};

// Define the structure for other inhabitants (citizens)
struct Citizen {
    int id;                 // Unique identifier for the citizen
    int positionX;          // X-coordinate in the city grid
    int positionY;          // Y-coordinate in the city grid
    int workplace;          // ID of the workplace (Hôtel de Ville, Supermarket, or Company)
    // Other necessary fields according to the requirements
};

// Define the structure for buildings
struct Building {
    int type;               // Type of the building (Hôtel de Ville, Supermarket, Company, etc.)
    int capacity;           // Maximum number of characters the building can hold
    // Other necessary fields according to the requirements
};

// Define the structure for the city
struct City {
    int sizeX;              // Size of the city grid in the X-direction
    int sizeY;              // Size of the city grid in the Y-direction
    // Other necessary fields according to the requirements
};

// Define the structure for shared memory
struct SharedMemory {
    struct Agent agents[MAX_AGENTS];          // Array of agents
    struct Citizen citizens[MAX_CITIZENS];    // Array of citizens
    struct Building buildings[MAX_BUILDINGS]; // Array of buildings
    struct City city;                         // City information
    // Other necessary fields according to the requirements
};



// Function prototypes
void* agentSourceThread(void* arg);
void* officerTraitantThread(void* arg);

#endif // ENEMY_SPY_NETWORK_H
