#include "citizen_manager.h"
#include <time.h>
#include <stdbool.h>
#include "spy_simulation.h"


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
    if (row < 0 || row >= MAX_ROWS || col < 0 || col >= MAX_COLUMNS || visited[row][col] || cityMap->cells[row][col].type != WASTELAND) {
        return false;
    }

    // Si une case WASTELAND voisine de la case d'arrivée est atteinte
    if (is_neighbor(row, col, endRow, endCol)) {
    
        // printf("dfs: Wasteland neighbor found at (%d, %d) for end (%d, %d)\n", row, col, endRow, endCol);
        return true;
    }

    // Marquer la case actuelle comme visitée
    visited[row][col] = true;

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
    visited[row][col] = false;

    return false;
}

bool is_path_available(map_t *cityMap, int startRow, int startCol, int endRow, int endCol, bool checked[MAX_ROWS][MAX_COLUMNS]) {
    if (checked[endRow][endCol]) {
        // printf("is_path_available: Path already checked from (%d, %d) to (%d, %d)\n", startRow, startCol, endRow, endCol);
        return true;
    }
    // printf("is_path_available: Checking path from (%d, %d) to (%d, %d)\n", startRow, startCol, endRow, endCol);

    bool visited[MAX_ROWS][MAX_COLUMNS] = {{false}};
    int rowOffsets[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int colOffsets[] = {-1, 0, 1, 1, 1, 0, -1, -1};

    for (int k = 0; k < 8; k++) {
        int newRow = startRow + rowOffsets[k];
        int newCol = startCol + colOffsets[k];

        // Check if the neighboring cell is a WASTELAND and not visited
        if (newRow >= 0 && newRow < MAX_ROWS && newCol >= 0 && newCol < MAX_COLUMNS &&
            !visited[newRow][newCol] && cityMap->cells[newRow][newCol].type == WASTELAND) {
            if (dfs(cityMap, visited, newRow, newCol, endRow, endCol)) {
                checked[endRow][endCol] = true;
                return true;
            }
        }
    }
    // printf("no path is avaiable from (%d, %d) to (%d, %d)\n", startRow, startCol, endRow, endCol);
    checked[endRow][endCol] = false;
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
            int i = rand() % MAX_ROWS;
            int j = rand() % MAX_COLUMNS;

            //printf("place_building_randomly: Attempt %d to place building at (%d, %d)\n", attempts, i, j);
            if (cityMap->cells[i][j].type == WASTELAND) {
                cityMap->cells[i][j].type = buildingType;
                cityMap->cells[i][j].nb_of_characters = nb_of_characters;
                // memory->companies[companyCount].position[0] = i;
                // memory->companies[companyCount].position[1] = j;

                bool allConnected = true;
                bool checked[MAX_ROWS][MAX_COLUMNS] = {{false}};

                // Vérifier si chaque bâtiment est accessible depuis chaque autre bâtiment
                for (int m = 0; m < MAX_ROWS && allConnected; ++m) {
                    for (int n = 0; n < MAX_COLUMNS && allConnected; ++n) {
                        if (cityMap->cells[m][n].type != WASTELAND) {
                            for (int p = 0; p < MAX_ROWS && allConnected; ++p) {
                                for (int q = 0; q < MAX_COLUMNS && allConnected; ++q) {
                                    if (cityMap->cells[p][q].type != WASTELAND && (m != p || n != q)) {
                                        if (!is_path_available(cityMap, m, n, p, q, checked)) {
                                            allConnected = false;
                                            //printf("place_building_randomly: No path from (%d, %d) to (%d, %d). Retrying...\n", m, n, p, q);
                                            cityMap->cells[i][j].type = WASTELAND;
                                            cityMap->cells[i][j].nb_of_characters = 0;
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
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLUMNS; j++) {
            cityMap->cells[i][j].type = WASTELAND;
            cityMap->cells[i][j].nb_of_characters = 0;
        }
    }

    // printf("init_map: Placing City Hall at (3, 3)\n");
    cityMap->cells[3][3].type = CITY_HALL;
    cityMap->cells[3][3].nb_of_characters = 20;

    // printf("init_map: Placing other buildings\n");
    place_building_randomly(cityMap, SUPERMARKET, 2, 30);
    place_building_randomly(cityMap, COMPANY, 8, 50);
    place_building_randomly(cityMap, RESIDENTIAL_BUILDING, 11, 15);
}










// void assign_company_to_citizen(memory_t* memory, citizen_t* citizen) {
//     building_t *buildings = memory->companies;
//     int company_index;
//     int found = 0;  // Indicateur pour savoir si une entreprise a été trouvée

//     for (int attempts = 0; attempts < NB_WORKPLACES; attempts++) {
//         company_index = rand() % NB_WORKPLACES;

//         if (buildings[company_index].nb_workers < buildings[company_index].max_workers &&
//             buildings[company_index].nb_workers < buildings[company_index].min_workers) {
//             citizen->workplace = &buildings[company_index];
//             buildings[company_index].nb_workers++;
//             found = 1; 
//             break;
//         }
//     }

//     if (!found) {
//         printf("No company found for citizen %d\n", citizen->id);
//     }
// }


// void assing_company_to_citizen(memory_t* memory, citizen_t* citizen){
    
//     building_t *buildings = memory->companies;
//     // Assign a random company, respecting max capacity
//     int company_index;
//     do {
//         company_index = rand() % NB_WORKPLACES;
//     } while (buildings[company_index].nb_workers >= buildings[company_index].max_workers 
//             && buildings[company_index].nb_workers <= buildings[company_index].min_workers);
//     citizen->workplace = &buildings[company_index];
//     buildings[company_index].nb_workers++;
// }



// void init_states(citizen_t *citizen) {
//     state_t *current_state;
//     state_t *next_state;
//     state_t *resting_at_home;
//     state_t *going_to_company;
//     state_t *working;
//     state_t *going_to_supermarket;
//     state_t *doing_some_shopping;
//     state_t *going_back_home;
//     state_t *dying;
//     state_t *finished;

//     current_state = new_state(citizen->id, citizen->current_state->action);
//     next_state = new_state(citizen->id, citizen->next_state->action);
//     resting_at_home = new_state(citizen->id, citizen->resting_at_home->action);
//     going_to_company = new_state(citizen->id, citizen->going_to_company->action);
//     working = new_state(citizen->id, citizen->working->action);
//     going_to_supermarket = new_state(citizen->id, citizen->going_to_supermarket->action);
//     doing_some_shopping = new_state(citizen->id, citizen->doing_some_shopping->action);
//     going_back_home = new_state(citizen->id, citizen->going_back_home->action);
//     dying = new_state(citizen->id, citizen->dying->action);
//     finished = new_state(citizen->id, citizen->finished->action);

// }






// void citizen_begin(citizen_t *citizen) {
//     citizen->change_state(citizen, citizen->resting_at_home);
// }

// void citizen_step(citizen_t *citizen) {
//     citizen->change_state(citizen, citizen->next_state);
// }

// void citizen_end(citizen_t *citizen) {
//     citizen->change_state(citizen, citizen->resting_at_home);
// }

// state_t *rest_at_home(citizen_t *citizen) {
//     return citizen->going_to_company;
// }

// state_t *go_to_company(citizen_t *citizen) {
//     return citizen->working;
// }

// state_t *work(citizen_t *citizen) {
//     return citizen->going_to_supermarket;
// }

void init_surveillance(surveillanceNetwork_t *surveillanceNetwork) {
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLUMNS; ++j) {
            surveillanceNetwork->devices[i][j].standard_camera = 1; // Standard cameras enabled by default
            surveillanceNetwork->devices[i][j].lidar = 1; // Lidars enabled by default
        }
    }
    // Initialization of the AI state
    surveillanceNetwork->surveillanceAI.suspicious_movement = 0; // No suspicious movement detected initially
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

    printf("avant de faire les init");

    // Initialize the shared memory as necessary
    shared_memory->memory_has_changed = 0;
    shared_memory->simulation_has_ended = 0;
    init_map(&shared_memory->map);
    init_surveillance(&shared_memory->surveillanceNetwork);
    //shared_memory->mqInfo = init_mq();

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

void start_simulation_processes(){
    // pid_t pid_monitor, pid_enemy_spy_network, pid_citizen_manager, pid_enemy_country,
    // pid_counterintelligence_officer, pid_timer;
    int num_children =0;
    pid_t pidExecutables[5];

    pidExecutables[num_children] = fork();
    if (pidExecutables[num_children] == -1) {
        perror("Error [fork()] monitor:");
        exit(EXIT_FAILURE);
    }
    if (pidExecutables[num_children] == 0) {
        if (execl("./bin/monitor", "monitor", NULL) == -1) {
            perror("Error [execl] monitor: ");
            exit(EXIT_FAILURE);
        }
        
    }
    num_children++;

    pidExecutables[num_children] = fork();
    if (pidExecutables[num_children] == -1) {
        perror("Error [fork()] timer: ");
        exit(EXIT_FAILURE);
    }
    if (pidExecutables[num_children] == 0) {
        if (execl("./bin/timer", "timer", NULL) == -1) {
            perror("Error [execl] timer: ");
            exit(EXIT_FAILURE);
        }
    }
    num_children++;

    // pidExecutables[num_children] = fork();
    // if (pidExecutables[num_children] == -1) {
    //     perror("Error [fork()] citizen_manager: ");
    //     exit(EXIT_FAILURE);
    // }
    // if (pidExecutables[num_children] == 0) {
    //     if (execl("./bin/citizen_manager", "citizen_manager", NULL) == -1) {
    //         perror("Error [execl] citizen_manager: ");
    //         exit(EXIT_FAILURE);
    //     }
    // }
    num_children++;

    pidExecutables[num_children] = fork();
    if (pidExecutables[num_children] == -1) {
        perror("Error [fork()] enemy_spy_network: ");
        exit(EXIT_FAILURE);
    }
    if (pidExecutables[num_children] == 0) {
        if (execl("./bin/enemy_spy_network", "enemy_spy_network", NULL) == -1) {
            perror("Error [execl] enemy_spy_network: ");
            exit(EXIT_FAILURE);
        }
    }
    num_children++;

    pidExecutables[num_children] = fork();
    if (pidExecutables[num_children] == -1) {
        perror("Error [fork()] enemy_country: ");
        exit(EXIT_FAILURE);
    }
    if (pidExecutables[num_children] == 0) {
        if (execl("./bin/enemy_country", "enemy_country", NULL) == -1) {
            perror("Error [execl] enemy_country: ");
            exit(EXIT_FAILURE);
        }
    }
    num_children++;

    pidExecutables[num_children] = fork();
    if (pidExecutables[num_children] == -1) {
        perror("Error [fork()] counter_intelligence: ");
        exit(EXIT_FAILURE);
    }
    if (pidExecutables[num_children] == 0) {
        if (execl("./bin/counter_intelligence", "counter_intelligence", NULL) == -1) {
            perror("Error [execl] : counter_intelligence");
            exit(EXIT_FAILURE);
        }
    }

    
   
    for (int i = 0; i < num_children; i++) {
        int status;
        waitpid(pidExecutables[i], &status, 0);
    }
    
    int statusSharedMemory;

    // Replace 'file_name.txt' with the name of the file you want to delete
    statusSharedMemory = remove("/dev/shm/SharedMemory");

    if (statusSharedMemory == 0)
        printf("File deleted successfully\n");
    else
        printf("Error: unable to delete the file\n");

    return 0;
    

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
    
    
}
