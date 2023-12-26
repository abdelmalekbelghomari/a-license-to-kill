#include "memory.h"
#include "../../include/citizen_manager.h"
#include <time.h>


/*A utiliser dans citizen manager ou dans les .c correspondant
aux protagonistes et antagonistes pour la gestion des blessures*/
void signal_handler(int signal, memory_t *shared_memory) {
    if (signal == SIGUSR1) {
        shared_memory->memory_has_changed = 1;
    } else if (signal == SIGUSR2) {
        shared_memory->simulation_has_ended = 1;
        /*TO DO*/
        /*Print info*/
    }
}


void init_map(map_t *cityMap) {
    int i, j, attempt, success;
    int placements[MAX_ROWS][MAX_COLUMNS] = {0};

    srand(time(NULL));

    // Initialize all cells as wasteland
    for (i = 0; i < MAX_ROWS; ++i) {
        for (j = 0; j < MAX_COLUMNS; ++j) {
            cityMap->cells[i][j].type = WASTELAND;
            cityMap->cells[i][j].current_capacity = 0;
            cityMap->cells[i][j].nb_of_characters = 0;
        }
    }

    // Place City Hall
    cityMap->cells[3][3].type = CITY_HALL;
    cityMap->cells[3][3].nb_of_characters = 20;
    placements[3][3] = CITY_HALL;

    // Place Supermarkets
    cityMap->cells[1][2].type = SUPERMARKET;
    cityMap->cells[1][2].nb_of_characters = 30;
    placements[1][2] = SUPERMARKET;

    cityMap->cells[4][3].type = SUPERMARKET;
    cityMap->cells[4][3].nb_of_characters = 30;
    placements[4][3] = SUPERMARKET;

    // Place other buildings with CSP check for connectivity
    int required_companies = 8;
    int required_buildings = 11;
    int buildings_placed = 0;
    int companies_placed = 0;
    for (attempt = 0; attempt < 1000 && (buildings_placed < required_buildings || companies_placed < required_companies); ++attempt) {
        i = rand() % MAX_ROWS;
        j = rand() % MAX_COLUMNS;

        // Skip if the cell is already occupied
        if (placements[i][j] != WASTELAND) continue;

        // Check if at least one neighboring cell is not WASTELAND
        success = 0;
        for (int di = -1; di <= 1; ++di) {
            for (int dj = -1; dj <= 1; ++dj) {
                int ni = i + di;
                int nj = j + dj;
                if (ni >= 0 && ni < MAX_ROWS && nj >= 0 && nj < MAX_COLUMNS) {
                    if (placements[ni][nj] != WASTELAND) {
                        success = 1;
                    }
                }
            }
        }

        // Place a company or a residential building if the cell is connected
        if (success) {
            if (companies_placed < required_companies) {
                cityMap->cells[i][j].type = COMPANY;
                cityMap->cells[i][j].nb_of_characters = 50;
                placements[i][j] = COMPANY;
                companies_placed++;
            } else {
                cityMap->cells[i][j].type = RESIDENTIAL_BUILDING;
                cityMap->cells[i][j].nb_of_characters = 15;
                placements[i][j] = RESIDENTIAL_BUILDING;
                buildings_placed++;
            }
        }
    }

    // Check if all buildings were placed successfully
    if (buildings_placed < required_buildings || companies_placed < required_companies) {
        fprintf(stderr, "Failed to place all buildings with connectivity constraints.\n");
    }
}


void init_citizens(citizen_t *citizens){
    int i;
    int k = rand() % MAX_ROWS;
    int j = rand() % MAX_COLUMNS;
    for (i = 0; i < CITIZENS_COUNT; i++){
        citizens[i].type = NORMAL;
        citizens[i].health = 10;
        citizens[i].position[0] = k;
        citizens[i].position[1] = j;
    }

    /* Counter intelligence officer is in the city Hall */
    citizens[0].type = COUNTER_INTELLIGENCE_OFFICER;
    citizens[0].position[0] = 3;
    citizens[0].position[1] = 3;
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



memory_t *create_shared_memory(const char *name) {
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open error");
        exit(EXIT_FAILURE);
    }

    // Calculate the size from the structure size
    size_t size = sizeof(struct memory_s);

    if (ftruncate(shm_fd, size) == -1) {
        perror("ftruncate error");
        exit(EXIT_FAILURE);
    }

    memory_t *shared_memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap error");
        exit(EXIT_FAILURE);
    }

    close(shm_fd);

    // Initialize the shared memory as necessary
    shared_memory->memory_has_changed = 0;
    shared_memory->simulation_has_ended = 0;
    init_map(&shared_memory->map);
    init_citizens(&shared_memory->citizens);
    init_surveillance(&shared_memory->surveillanceNetwork);

    return shared_memory;
}

void start_simulation_processes(){
    pid_t pid_monitor, pid_enemy_spy_network, pid_citizen_manager, pid_enemy_country,
    pid_counterintelligence_officer, pid_timer;

    pid_monitor = fork();
    if (pid_monitor == -1) {
        perror("Error [fork()] monitor:");
        exit(EXIT_FAILURE);
    }
    if (pid_monitor == 0) {
        if (execl("./bin/monitor", "monitor", NULL) == -1) {
            perror("Error [execl] monitor: ");
            exit(EXIT_FAILURE);
        }
    }
    
    /*pid_citizen_manager = fork();
    if (pid_citizen_manager == -1) {
        perror("Error [fork()] citizen_manager: ");
        exit(EXIT_FAILURE);
    }
    if (pid_citizen_manager == 0) {
        if (execl("./bin/citizen_manager", "citizen_manager", NULL) == -1) {
            perror("Error [execl] citizen_manager: ");
            exit(EXIT_FAILURE);
        }
    }*/

    /*pid_counterintelligence_officer = fork();
    if (pid_counterintelligence_officer == -1) {
        perror("Error [fork()] counterintelligence_officer: ");
        exit(EXIT_FAILURE);
    }
    if (pid_counterintelligence_officer == 0) {
        if (execl("./bin/counterintelligence_officer", "counterintelligence_officer", NULL) == -1) {
            perror("Error [execl] counterintelligence_officer: ");
            exit(EXIT_FAILURE);
        }
    }*/

    /*pid_enemy_country = fork();
    if (pid_enemy_country == -1) {
        perror("Error [fork()] enemy_country: ");
        exit(EXIT_FAILURE);
    }
    if (pid_enemy_country == 0) {
        if (execl("./bin/enemy_country", "enemy_country", NULL) == -1) {
            perror("Error [execl] enemy_country: ");
            exit(EXIT_FAILURE);
        }
    }*/

    /*pid_enemy_spy_network = fork();
    if (pid_enemy_spy_network == -1) {
        perror("Error [fork()] enemy_spy_network: ");
        exit(EXIT_FAILURE);
    }
    if (pid_enemy_spy_network == 0) {
        if (execl("./bin/enemy_spy_network", "enemy_spy_network", NULL) == -1) {
            perror("Error [execl] enemy_spy_network: ");
            exit(EXIT_FAILURE);
        }
    }*/
    
    /*pid_timer = fork();
    if (pid_timer == -1) {
        perror("Error [fork()] timer: ");
        exit(EXIT_FAILURE);
    }
    if (pid_timer == 0) {
        if (execl("./bin/timer", "timer", NULL) == -1) {
            perror("Error [execl] timer: ");
            exit(EXIT_FAILURE);
        }
    }*/
}