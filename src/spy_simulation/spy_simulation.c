#include "memory.h"
#include "citizen_manager.h"
#include <time.h>
#include <stdbool.h>
#include "spy_simulation.h"

void handle_fatal_error(const char *message)
{
  perror(message);
  exit(EXIT_FAILURE);
}

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
    // pid_timer = start_timer();
    // pid_citizen_manager = start_citizen_manager();

    // Start monitor process
    // pid_monitor = start_monitor(); 

    // Wait for timer and monitor to finish
    int status;
    waitpid(pid_timer, &status, 0);
    waitpid(pid_citizen_manager, &status, 0);
    //waitpid(pid_monitor, &status, 0);
    
    // Add other child processes as needed
}


int start_citizen_manager(){
    int pid_citizen_manager = fork();

    if (pid_citizen_manager == -1) {
      handle_fatal_error("Error creating fork()\n");
    }

    if (pid_citizen_manager == 0) {
        if (execl("./bin/manager", "citizen_manager", NULL) == -1){
            handle_fatal_error("Failed to start citizen_manager\n");
        }
    }
    return pid_citizen_manager;
}

int start_monitor(){
    int pid_monitor = fork();

    if (pid_monitor == -1) {
      handle_fatal_error("Error creating fork()\n");
    }

    if (pid_monitor == 0) {
        if (execl("./bin/monitor", "monitor", NULL) == -1) {
            handle_fatal_error("Failed to start monitor\n");
        }
    }

    return pid_monitor;
}

int start_enemy_spy_network(){
    int pid_enemy_spy_network = fork();

    if (pid_enemy_spy_network == -1) {
      handle_fatal_error("Error creating fork()\n");
    }

    if (pid_enemy_spy_network == 0) {
        if (execl("./bin/enemy_spy_network", "enemy_spy_network", NULL) == -1) {
            handle_fatal_error("Failed to start enemy_spy_network\n");
        }
    }

    return pid_enemy_spy_network;
}

int start_counterintelligence_officer(){
    int pid_counterintelligence_officer = fork();

    if (pid_counterintelligence_officer == -1) {
       handle_fatal_error("Error creating fork()\n");
    }

    if (pid_counterintelligence_officer == 0) {
        if (execl("./bin/counterintelligence_officer", "counterintelligence_officer", NULL) == -1) {
            handle_fatal_error("Failed to start counterintelligence_officer\n");
        }
    }

    return pid_counterintelligence_officer;
}

int start_enemy_country(){
    int pid_enemy_country = fork();

    if (pid_enemy_country == -1) {
      handle_fatal_error("Error creating fork()\n");
    }

    if (pid_enemy_country == 0) {
        if (execl("./bin/menemy_country", "enemy_country", NULL) == -1) {
            handle_fatal_error("Failed to start enemy_country\n");
        }
    }

    return pid_enemy_country;
}

int start_timer(){
    int pid_timer = fork();

    if (pid_timer == -1) {
      handle_fatal_error("Error creating fork()\n");
    }

    if (pid_timer == 0) {
        if (execl("./bin/timer", "timer", NULL) == -1) {
            handle_fatal_error("Failed to start timer\n");
        }
    }

    return pid_timer;
}

mq_t init_mq(){
    shm_unlink("/mq");
    mq_t mqs;
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 20;
    attr.mq_msgsize = 128;
    attr.mq_curmsgs = 0;
    mqs.mq = mq_open("/mq",O_CREAT | O_RDWR,0644,&attr);
    return mqs;
}
