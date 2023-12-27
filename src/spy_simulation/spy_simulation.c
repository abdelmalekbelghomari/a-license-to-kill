#include "memory.h"
#include "citizen_manager.h"
#include <time.h>
#include <stdbool.h>


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

bool is_valid_move(map_t *cityMap, bool visited[MAX_ROWS][MAX_COLUMNS], int row, int col) {
    return (row >= 0) && (row < MAX_ROWS) && (col >= 0) && (col < MAX_COLUMNS) 
        && (cityMap->cells[row][col].type == WASTELAND || cityMap->cells[row][col].type == RESIDENTIAL_BUILDING) 
        && !visited[row][col];
}

bool dfs(map_t *cityMap, bool visited[MAX_ROWS][MAX_COLUMNS], int row, int col, int endRow, int endCol) {
    if (row == endRow && col == endCol) return true;

    visited[row][col] = true;

    int rowOffsets[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int colOffsets[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    for (int k = 0; k < 8; k++) {
        int newRow = row + rowOffsets[k];
        int newCol = col + colOffsets[k];
        if (is_valid_move(cityMap, visited, newRow, newCol) && dfs(cityMap, visited, newRow, newCol, endRow, endCol)) {
            return true;
        }
    }

    return false;
}

bool is_path_available(map_t *cityMap, int startRow, int startCol, int endRow, int endCol) {
    bool visited[MAX_ROWS][MAX_COLUMNS] = {{false}};
    return dfs(cityMap, visited, startRow, startCol, endRow, endCol);
}
void place_building_randomly(map_t *cityMap, int buildingType, int count, int nb_of_characters) {
    int placed_count = 0;
    int i, j;
    while (placed_count != count) {
        i = rand() % MAX_ROWS;
        j = rand() % MAX_COLUMNS;
        if (cityMap->cells[i][j].type == WASTELAND){
            cityMap->cells[i][j].type = buildingType;
            cityMap->cells[i][j].nb_of_characters = nb_of_characters;
            placed_count++;
        }
    }
}

void init_map(map_t *cityMap){
    int i, j;
    for (i = 0; i < MAX_ROWS; i++){
        for (j = 0; j < MAX_COLUMNS; j++){
            cityMap->cells[i][j].type = WASTELAND;
            cityMap->cells[i][j].current_capacity = 0;
            cityMap->cells[i][j].nb_of_characters = 0;
        }
    }

    cityMap->cells[3][3].type = CITY_HALL;
    cityMap->cells[3][3].nb_of_characters = 20;

    place_building_randomly(cityMap, SUPERMARKET, 2, 30);
    place_building_randomly(cityMap, COMPANY, 8, 50);
    place_building_randomly(cityMap, RESIDENTIAL_BUILDING, 11, 15);
}

void add_citizen(home_t *house, citizen_t *citizen) {
    if (house->nb_citizen < house->max_capacity) {
        // Ajoutez le citoyen à la maison
        house->citizens[house->nb_citizen] = citizen;
        house->nb_citizen++; // Incrémentez le nombre de citoyens

        // Vous pouvez également mettre à jour d'autres attributs du citoyen si nécessaire
        // par exemple, la position du citoyen, etc.
    } else {
        fprintf(stderr, "Erreur : la maison est pleine.\n");
        // Gérer la situation où la maison est pleine
    }
}

double distance(unsigned int pos1[2], unsigned int pos2[2]) {
    return abs((int)pos1[0] - (int)pos2[0]) + abs((int)pos1[1] - (int)pos2[1]);
}

void assign_home_to_citizen(memory_t* memory, citizen_t* citizen){
    

    home_t *houses = memory->homes;
    for(int i = 0; i < NB_HOMES; i++){
        houses[i].max_capacity = 15;
    }
    // Assign a random house, respecting max capacity
    int house_index;
    do {
        house_index = rand() % NB_HOMES;
    } while (houses[house_index].nb_citizen >= houses[house_index].max_capacity);
    citizen->home = &houses[house_index];
    houses[house_index].nb_citizen++;
}

void assing_company_to_citizen(memory_t* memory, citizen_t* citizen){
    
    building_t *buildings = memory->companies;
    
    for(int i = 0; i < NB_BUISNESSES; i++){
        if(i < 2){
            buildings[i].type = STORE;
            buildings[i].max_workers = 3;
            buildings[i].min_workers = 3;
        } else {
            buildings[i].min_workers = 5;
            buildings[i].max_workers = 50;
            buildings[i].type = CORPORATION;
        }
    }
    // Assign a random company, respecting max capacity
    int company_index;
    do {
        company_index = rand() % NB_BUISNESSES;
    } while (buildings[company_index].nb_workers >= buildings[company_index].max_workers 
            && buildings[company_index].nb_workers <= buildings[company_index].min_workers);
    citizen->workplace = &buildings[company_index];
    buildings[company_index].nb_workers++;
}

void assing_nearest_supermarket(memory_t* memory, citizen_t* citizen){
    
    // Find nearest supermarket
    building_t *buildings = memory->companies;
    // Les deux premiers emplacements sont donnés aux supermarchés
    double dist1 = distance(buildings[0].position, citizen->workplace->position);
    double dist2 = distance(buildings[1].position, citizen->workplace->position);
    citizen->supermarket = dist1 > dist1 ? &buildings[0] : &buildings[1];
}

void init_citizens(memory_t *memory) {
    srand(time(NULL));


    for (int i = 0; i < CITIZENS_COUNT; i++) {
        citizen_t *citizen = &memory->citizens[i];

        citizen->type = NORMAL;
        citizen->health = 10;
        // printf("ftg Haykel ton micro de merde\n");

        assign_home_to_citizen(memory, citizen);
        printf("maison du citoyen %d est la maison %p\n", i+1, citizen->home);
        assing_company_to_citizen(memory, citizen);
        printf("entreprise du citoyen %d est l'entreprise %p\n", i+1, 
                                                            citizen->workplace);
        assing_nearest_supermarket(memory, citizen);
        printf("Le supermarché le plus proche du citoyen %d est %p\n", i+1, citizen->supermarket);
    }
        
}


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

    printf("avant de faire les init");

    // Initialize the shared memory as necessary
    shared_memory->memory_has_changed = 0;
    shared_memory->simulation_has_ended = 0;
    init_map(&shared_memory->map);
    init_citizens(shared_memory);
    init_surveillance(&shared_memory->surveillanceNetwork);

    return shared_memory;
}

void start_simulation_processes() {
    pid_t pid_monitor, pid_timer, pid_citizen_manager;

    // Start timer process first
    // pid_timer = fork();
    // if (pid_timer == 0) {
    //     execl("./bin/timer", "timer", NULL);
    //     perror("Error [execl] timer: ");
    //     exit(EXIT_FAILURE);
    // } else if (pid_timer < 0) {
    //     perror("Error [fork()] timer: ");
    //     exit(EXIT_FAILURE);
    // }
    // pid_timer = fork();
    // if (pid_citizen_manager == 0) {
    //     execl("./bin/citizen_manager", "citizen_manager", NULL);
    //     perror("Error [execl] timer: ");
    //     exit(EXIT_FAILURE);
    // } else if (pid_citizen_manager < 0) {
    //     perror("Error [fork()] timer: ");
    //     exit(EXIT_FAILURE);
    // }

    // Start monitor process
    /*pid_monitor = fork();
    if (pid_monitor == 0) {
        execl("./bin/monitor", "monitor", NULL);
        perror("Error [execl] monitor: ");
        exit(EXIT_FAILURE);
    } else if (pid_monitor < 0) {
        perror("Error [fork()] monitor:");
        exit(EXIT_FAILURE);
    }*/

    // Wait for timer and monitor to finish
    int status;
    waitpid(pid_timer, &status, 0);
    waitpid(pid_citizen_manager, &status, 0);
    //waitpid(pid_monitor, &status, 0);
    
    // Add other child processes as needed
}
