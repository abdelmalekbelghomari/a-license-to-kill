#include "spy_simulation.h"


void init_map(map_t * cityMap){
    /* Init all cells as empty terrain*/
    int i,j;
    for (i = 0; i < MAX_ROWS; i++){
        for (j = 0; j < MAX_COLUMNS; j++){
            cityMap->cells[i][j].type = WASTELAND;
            cityMap->cells[i][j].current_capacity = 0;
            cityMap->cells[i][j].nb_of_characters = 0;
        }
    }

    /* City hall */
    cityMap->cells[3][3].type = CITY_HALL;
    cityMap->cells[3][3].nb_of_characters = 20;

    /* 2 Supermarkets */

    cityMap->cells[1][2].type = SUPERMARKET;
    cityMap->cells[1][2].nb_of_characters = 30;

    cityMap->cells[4][3].type = SUPERMARKET;
    cityMap->cells[4][3].nb_of_characters = 30;

    /* 8 Companies */
    int companies_count = 0;
    while (companies_count != 8) {
        i = rand() % MAX_ROWS;
        j = rand() % MAX_COLUMNS;
        if (cityMap->cells[i][j].type == WASTELAND){
            cityMap->cells[i][j].type = COMPANY;
            cityMap->cells[i][j].nb_of_characters = 50;
            companies_count++;
        }
    }

    /* 11 residential buildings */
    int buildings_count = 0;
    while (buildings_count != 11) {
        i = rand() % MAX_ROWS;
        j = rand() % MAX_COLUMNS;
        if (cityMap->cells[i][j].type == WASTELAND){
            cityMap->cells[i][j].type = RESIDENTIAL_BUILDING;
            cityMap->cells[i][j].nb_of_characters = 15;
            buildings_count++;
        }
    }
}


void init_citizens(citizen_t *citizens){
    int i;
    for (i = 0; i < CITIZENS_COUNT; i++){
        citizens[i].type = NORMAL;
        citizens[i].health = 10;
        citizens[i].positionX = 0;
        citizens[i].positionY = 0;
        citizens[i].currentBuilding = WASTELAND;
    }

    /* Counter intelligence officer is in the city Hall */
    citizens[0].type = COUNTER_INTELLIGENCE_OFFICER;
    citizens[0].positionX = 3;
    citizens[0].positionY = 3;
    citizens[0].currentBuilding = CITY_HALL;
}


void init_surveillance(surveillanceNetwork_t *surveillanceNetwork) {
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLUMNS; ++j) {
            surveillanceNetwork->devices[i][j].standard_camera = 1; // Standard cameras enabled by default
            surveillanceNetwork->devices[i][j].infrared_camera = 1; // Infrared cameras enabled by default
            surveillanceNetwork->devices[i][j].lidar = 1; // Lidars enabled by default
        }
    }
    // Initialization of the AI state
    surveillanceNetwork->surveillanceAI.suspicious_movement = 0; // No suspicious movement detected initially
}


struct memory_s *create_shared_memory(const char *name) {
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Calculate the size from the structure size
    size_t size = sizeof(struct memory_s);

    if (ftruncate(shm_fd, size) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    struct memory_s *shared_memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(shm_fd);

    // Initialize the shared memory as necessary
    shared_memory->memory_has_changed = 0;
    shared_memory->simulation_has_ended = 0;
    init_map(&shared_memory->cityMap);
    init_citizens(&shared_memory->citizens);
    init_surveillance(&shared_memory->surveillanceNetwork);

    return shared_memory;
}
