/**
 * @file spy_simulation.c
 * @brief Spy Simulation Control and Process Launcher.
 *
 * This file contains the code for controlling the spy simulation, including
 * map creation, process launching, and shared memory segment initialization.
 * It serves as a central component for orchestrating the entire espionage
 * simulation and intelligence operations.
 *
 */

#include "citizen_manager.h"
#include <time.h>
#include <stdbool.h>
#include "spy_simulation.h"

// extern sem_t *sem_producer_timer, *sem_consumer_timer;

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

bool is_neighbor(int row, int col, int endRow, int endCol) {
    // Vérifie si la case (row, col) est voisine de la case (endRow, endCol)
    return (abs(row - endRow) <= 1 && abs(col - endCol) <= 1);
}

bool dfs(map_t *cityMap, bool visited[MAX_ROWS][MAX_COLUMNS], int row, int col, int endRow, int endCol) {
    // Vérifier si la position actuelle est valide
    if (row < 0 || row >= MAX_ROWS || col < 0 || col >= MAX_COLUMNS || visited[col][row] || cityMap->cells[col][row].type != WASTELAND) {
        return false;
    }

    // Si une case WASTELAND voisine de la case d'arrivée est atteinte
    if (is_neighbor(row, col, endRow, endCol)) {
    
        // printf("dfs: Wasteland neighbor found at (%d, %d) for end (%d, %d)\n", row, col, endRow, endCol);
        return true;
    }

    // Marquer la case actuelle comme visitée
    visited[col][row] = true;

    // Définir les 8 directions de l'exploration (incluant les diagonales)
    int rowOffsets[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int colOffsets[] = {-1, 0, 1, 1, 1, 0, -1, -1};

    // Explorer toutes les directions possibles
    for (int k = 0; k < 8; k++) {
        int newRow = row + rowOffsets[k];
        int newCol = col + colOffsets[k];

        if (dfs(cityMap, visited, newRow, newCol, endRow, endCol)) {
            return true;
        }
    }

    // Backtracking: Désélectionner la case actuelle avant de revenir en arrière
    visited[col][row] = false;

    return false;
}

void reset_checked(bool checked[MAX_ROWS][MAX_COLUMNS]) {
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLUMNS; j++) {
            checked[j][i] = false;
        }
    }
}

bool is_path_available(map_t *cityMap, int startRow, int startCol, int endRow, int endCol, bool checked[MAX_ROWS][MAX_COLUMNS]) {
    
    reset_checked(checked);

    if (checked[endCol][endRow]) {
        // printf("is_path_available: Path already checked from (%d, %d) to (%d, %d)\n", startRow, startCol, endRow, endCol);
        return true;
    }
    // printf("is_path_available: Checking path from (%d, %d) to (%d, %d)\n", startRow, startCol, endRow, endCol);

    bool visited[MAX_COLUMNS][MAX_ROWS] = {{false}};
    int rowOffsets[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int colOffsets[] = {-1, 0, 1, 1, 1, 0, -1, -1};

    for (int k = 0; k < 8; k++) {
        int newRow = startRow + rowOffsets[k];
        int newCol = startCol + colOffsets[k];

        // Check if the neighboring cell is a WASTELAND and not visited
        if (newRow >= 0 && newRow < MAX_ROWS && newCol >= 0 && newCol < MAX_COLUMNS &&
            !visited[newCol][newRow] && cityMap->cells[newCol][newRow].type == WASTELAND) {
            if (dfs(cityMap, visited, newRow, newCol, endRow, endCol)) {
                checked[endCol][endRow] = true;
                return true;
            }
        }
    }
    // printf("no path is avaiable from (%d, %d) to (%d, %d)\n", startRow, startCol, endRow, endCol);
    checked[endCol][endRow] = false;
    return false;
}


void place_building_randomly(map_t *cityMap, int buildingType, int count, int nb_of_characters) {
    int max_attempts = 100;
    int companyCount = 0;
    for (int placed_count = 0; placed_count < count; ) {
        int attempts = 0;
        bool placed = false;

        while (!placed && attempts < max_attempts) {
            attempts++;
            int row = rand() % MAX_ROWS;
            int column = rand() % MAX_COLUMNS;

            //printf("place_building_randomly: Attempt %d to place building at (%d, %d)\n", attempts, i, j);
            if (cityMap->cells[row][column].type == WASTELAND) {
                cityMap->cells[row][column].type = buildingType;
                cityMap->cells[row][column].nb_of_characters = nb_of_characters;
                // memory->companies[companyCount].position[0] = i;
                // memory->companies[companyCount].position[1] = j;

                bool allConnected = true;
                bool checked[MAX_COLUMNS][MAX_ROWS] = {{false}};

                // Vérifier si chaque bâtiment est accessible depuis chaque autre bâtiment
                for (int m = 0; m < MAX_ROWS && allConnected; ++m) {
                    for (int n = 0; n < MAX_COLUMNS && allConnected; ++n) {
                        if (cityMap->cells[n][m].type != WASTELAND) {
                            for (int p = 0; p < MAX_ROWS && allConnected; ++p) {
                                for (int q = 0; q < MAX_COLUMNS && allConnected; ++q) {
                                    if (cityMap->cells[q][p].type != WASTELAND && (m != p || n != q)) {
                                        if (!is_path_available(cityMap, m, n, p, q, checked)) {
                                            allConnected = false;
                                            //printf("place_building_randomly: No path from (%d, %d) to (%d, %d). Retrying...\n", m, n, p, q);
                                            cityMap->cells[row][column].type = WASTELAND;
                                            cityMap->cells[row][column].nb_of_characters = 0;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (allConnected) {
                    //printf("place_building_randomly: Building placed at (%d, %d)\n", i, j);
                    placed = true;
                    placed_count++;
                }
            }
            companyCount++;
        }

        if (!placed) {
            printf("Unable to place all buildings after %d attempts. Resetting...\n", max_attempts);
            break;
        }
    }
}

void init_map(map_t *cityMap) {
    // printf("init_map: Initializing the map\n");
    // sem_wait(sem_consumer_timer);
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int column = 0; column < MAX_COLUMNS; column++) {
            cityMap->cells[row][column].type = WASTELAND;
            cityMap->cells[row][column].nb_of_characters = 0;
        }
    }

    // printf("init_map: Placing City Hall at (3, 3)\n");
    cityMap->cells[3][3].type = CITY_HALL;
    cityMap->cells[3][3].nb_of_characters = 20;

    // printf("init_map: Placing other buildings\n");
    place_building_randomly(cityMap, SUPERMARKET, 2, 30);
    place_building_randomly(cityMap, COMPANY, 8, 50);
    place_building_randomly(cityMap, RESIDENTIAL_BUILDING, 11, 15);
    // sem_post(sem_producer_timer);
}


void init_surveillance(surveillanceNetwork_t *surveillanceNetwork) {
    surveillanceNetwork->cameras.standard_camera = 0;
    surveillanceNetwork->cameras.infrared_camera = 0;
    surveillanceNetwork->surveillanceAI.suspicious_movement = 0; 
}



memory_t *create_shared_memory(const char *name) {
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    size_t size = sizeof(struct memory_s);

    if (ftruncate(shm_fd, size) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    memory_t *shared_memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(shm_fd);

    // Initialize the shared memory as necessary
    shared_memory->memory_has_changed = 0;
    shared_memory->simulation_has_ended = 0;
    shared_memory->walking_citizens = 0;
    shared_memory->at_home_citizens = 0;
    shared_memory->at_work_citizens = 0;
    init_map(&shared_memory->map);
    init_surveillance(&shared_memory->surveillanceNetwork);

    return shared_memory;
}

sem_t *create_semaphore(const char *name, int value) {
    sem_t *sem = sem_open(name, O_CREAT, 0644, value);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }
    return sem;
}

sem_t *open_semaphore(const char *name) {
    sem_t *sem = sem_open(name, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }
    return sem;
}

void start_simulation_processes(memory_t *memory){

    int num_children = 0;

    memory->pids[num_children] = fork();
    if (memory->pids[num_children] == -1) {
        perror("Error [fork()] timer: ");
        exit(EXIT_FAILURE);
    }
    if (memory->pids[num_children] == 0) {
        if (execl("./bin/timer", "timer", NULL) == -1) {
            perror("Error [execl] timer: ");
            exit(EXIT_FAILURE);
        }
    }
    num_children++;

    memory->pids[num_children] = fork();
    if (memory->pids[num_children] == -1) {
        perror("Error [fork()] citizen_manager: ");
        exit(EXIT_FAILURE);
    }
    if (memory->pids[num_children] == 0) {
        if (execl("./bin/citizen_manager", "citizen_manager", NULL) == -1) {
            perror("Error [execl] citizen_manager: ");
            exit(EXIT_FAILURE);
        }
    }
    num_children++;

    memory->pids[num_children] = fork();
    if (memory->pids[num_children] == -1) {
        perror("Error [fork()] enemy_spy_network: ");
        exit(EXIT_FAILURE);
    }
    if (memory->pids[num_children] == 0) {
        if (execl("./bin/enemy_spy_network", "enemy_spy_network", NULL) == -1) {
            perror("Error [execl] enemy_spy_network: ");
            exit(EXIT_FAILURE);
        }
    }
    num_children++;

    memory->pids[num_children] = fork();
    if (memory->pids[num_children] == -1) {
        perror("Error [fork()] enemy_country: ");
        exit(EXIT_FAILURE);
    }
    if (memory->pids[num_children] == 0) {
        if (execl("./bin/enemy_country", "enemy_country", NULL) == -1) {
            perror("Error [execl] enemy_country: ");
            exit(EXIT_FAILURE);
        }
    }
    num_children++;

    memory->pids[num_children] = fork();
    if (memory->pids[num_children] == -1) {
        perror("Error [fork()] counter_intelligence: ");
        exit(EXIT_FAILURE);
    }
    if (memory->pids[num_children] == 0) {
        if (execl("./bin/counter_intelligence", "counter_intelligence", NULL) == -1) {
            perror("Error [execl] : counter_intelligence");
            exit(EXIT_FAILURE);
        }
    }


    num_children++;

    memory->pids[num_children] = fork();
    if (memory->pids[num_children] == -1) {
        perror("Error [fork()] monitor:");
        exit(EXIT_FAILURE);
    }
    if (memory->pids[num_children] == 0) {
        if (execl("./bin/monitor", "monitor", NULL) == -1) {
            perror("Error [execl] monitor: ");
            exit(EXIT_FAILURE);
        }
        
    }
    

   
    for (int i = 0; i < num_children; i++) {
        int status;
        waitpid(memory->pids[i], &status, 0);
    }
    
    // int statusSharedMemory;

    // Replace 'file_name.txt' with the name of the file you want to delete
    // statusSharedMemory = remove("/dev/shm/SharedMemory");

    // if (statusSharedMemory == 0)
    //     printf("File deleted successfully\n");
    // else
    //     printf("Error: unable to delete the file\n");
    
}
