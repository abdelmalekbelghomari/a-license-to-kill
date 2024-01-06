#include "memory.h"
// #include "citizen_manager.h"
#include <time.h>
#include <stdbool.h>
#include "spy_simulation.h"


void handle_fatal_error(const char *message){
  perror(message);
  exit(EXIT_FAILURE);
}

/* To be used in the citizen manager or in the corresponding .c files 
for both protagonists and antagonists for injury management. */
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
    /* Checks if the cell (row, col) is neighbor to the cell (endRow, endCol) */
    return (abs(row - endRow) <= 1 && abs(col - endCol) <= 1);
}

bool dfs(map_t *cityMap, bool visited[MAX_ROWS][MAX_COLUMNS], int row, int col, int endRow, int endCol) {
    /* Checks if the actual position is valid */
    if (row < 0 || row >= MAX_ROWS || col < 0 || col >= MAX_COLUMNS || visited[row][col] || cityMap->cells[row][col].type != WASTELAND) {
        return false;
    }

    /* If a WASTELAND cell adjacent to the destination cell is reached */
    if (is_neighbor(row, col, endRow, endCol)) {
    
        /* printf("dfs: Wasteland neighbor found at (%d, %d) for end (%d, %d)\n", row, col, endRow, endCol); */
        return true;
    }

    /* Mark the current cell as visited.*/
    visited[row][col] = true;

    /* Define the 8 directions of exploration (including diagonals) */
    int rowOffsets[] = {-1, -1, -1, 0, 1, 1, 1, 0};
    int colOffsets[] = {-1, 0, 1, 1, 1, 0, -1, -1};

    /* Explore all possible directions */
    for (int k = 0; k < 8; k++) {
        int newRow = row + rowOffsets[k];
        int newCol = col + colOffsets[k];

        if (dfs(cityMap, visited, newRow, newCol, endRow, endCol)) {
            return true;
        }
    }

    /* Backtracking: Unselect the current cell before backtracking */
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

        /* Check if the neighboring cell is a WASTELAND and not visited */
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
    for (int placed_count = 0; placed_count < count; ) {
        int attempts = 0;
        bool placed = false;

        while (!placed && attempts < max_attempts) {
            attempts++;
            int i = rand() % MAX_ROWS;
            int j = rand() % MAX_COLUMNS;

            printf("place_building_randomly: Attempt %d to place building at (%d, %d)\n", attempts, i, j);
            if (cityMap->cells[i][j].type == WASTELAND) {
                cityMap->cells[i][j].type = buildingType;
                cityMap->cells[i][j].nb_of_characters = nb_of_characters;

                bool allConnected = true;
                bool checked[MAX_ROWS][MAX_COLUMNS] = {{false}};

                /* Check if each building is accessible from every other building */
                for (int m = 0; m < MAX_ROWS && allConnected; ++m) {
                    for (int n = 0; n < MAX_COLUMNS && allConnected; ++n) {
                        if (cityMap->cells[m][n].type != WASTELAND) {
                            for (int p = 0; p < MAX_ROWS && allConnected; ++p) {
                                for (int q = 0; q < MAX_COLUMNS && allConnected; ++q) {
                                    if (cityMap->cells[p][q].type != WASTELAND && (m != p || n != q)) {
                                        if (!is_path_available(cityMap, m, n, p, q, checked)) {
                                            allConnected = false;
                                            printf("place_building_randomly: No path from (%d, %d) to (%d, %d). Retrying...\n", m, n, p, q);
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
                    printf("place_building_randomly: Building placed at (%d, %d)\n", i, j);
                    placed = true;
                    placed_count++;
                }
            }
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

double distance(unsigned int pos1[2], unsigned int pos2[2]) {
    return abs((int)pos1[0] - (int)pos2[0]) + abs((int)pos1[1] - (int)pos2[1]);
}

void init_house(memory_t *memory){
    int fakeHome = rand() % NB_HOMES;
    // printf("=====================================\n");
    // printf("fake home %d\n", fakeHome);
    // printf("La maison %p est la maison avec la boite au lettre piégee\n", &memory->homes[fakeHome]);
    // printf("\n\n=====================================\n");
    for(int i = 0; i < NB_HOMES; i++){
        if (i == fakeHome){
            memory->homes[i].max_capacity = 0;
        }
        else{
            memory->homes[i].max_capacity = 15;
        }
    }
}

void init_building(memory_t *memory){
    for(int i = 0; i < NB_WORKPLACES; i++){
        if(i < NB_STORE){
            memory->companies[i].type = STORE;
            memory->companies[i].max_workers = 3;
            memory->companies[i].min_workers = 3;
        } else if (i == NB_STORE) {
            memory->companies[i].min_workers = 10;
            memory->companies[i].max_workers = 10;
            memory->companies[i].type = CITY_HALL;
        } else {
            memory->companies[i].min_workers = 5;
            memory->companies[i].max_workers = 50;
            memory->companies[i].type = CORPORATION;
        }
    }
}

void assign_home_to_citizen(memory_t* memory, citizen_t* citizen){

    home_t *houses = memory->homes;
    /* Assign a random house, respecting max capacity */
    int house_index;
    int attempts = 0;
    while (attempts < NB_HOMES) {
        house_index = rand() % NB_HOMES;
        if (houses[house_index].nb_citizen < houses[house_index].max_capacity) {
            citizen->home = &houses[house_index];
            houses[house_index].nb_citizen++;
            break; 
        }
        attempts++;
    }
}



void assign_company_to_citizen(memory_t* memory, citizen_t* citizen){
    
    building_t *company_list = memory->companies;
    /* Assign a random company, respecting max capacity */
    int company_index;
    int attempts = 0;  /* Counter to avoid an infinite loop */

    if(company_list[0].nb_workers < company_list[0].max_workers){
        citizen->workplace = &company_list[0];
        company_list[0].nb_workers++;

    } else if (company_list[1].nb_workers < company_list[1].max_workers){
        citizen->workplace = &company_list[1];
        company_list[1].nb_workers++;

    } else if (company_list[2].nb_workers < company_list[2].max_workers){
        citizen->workplace = &company_list[2];
        company_list[2].nb_workers++;

    } else {
        while (attempts < NB_COMPANY) {
            company_index = 3 + rand() % NB_COMPANY;
            building_t *company = &company_list[company_index];
            if (company->nb_workers < company->max_workers 
            && company->nb_workers < company->min_workers) {
                citizen->workplace = &company;
                company->nb_workers++;
                break;
            }else{
                attempts++;
            }
        }
    }

//     while (1) {
//         company_index = rand() % NB_WORKPLACES;
//         building_t *company = &company_list[company_index];

//         if (company->type == STORE && company->nb_workers < company->max_workers) {
//             citizen->workplace = company;
//             company->nb_workers++;
//             break;
//         if (company->type == CITY_HALL && company->nb_workers < company->max_workers) {
//             citizen->workplace = company;
//             company->nb_workers++;
//             break;
//         } else if (company->nb_workers < company->max_workers ||
//                    company->nb_workers < company->min_workers) {
//             citizen->workplace = company;
//             company->nb_workers++;
//             break;
//         }
//     }
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

void assign_random_supermarket(memory_t* memory, citizen_t* citizen){
    
    /* Find nearest supermarket */
    building_t supermaket_list[NB_STORE] = {memory->companies[0], memory->companies[1]};
    /* The first two locations are reserved for supermarkets */
    double dist1 = distance(supermaket_list[0].position, citizen->workplace->position);
    double dist2 = distance(supermaket_list[1].position, citizen->workplace->position);
    int supermaketChoice = rand() % NB_STORE;
    if(supermaketChoice == 0){
        citizen->supermarket = &supermaket_list[0];
    } else {
        citizen->supermarket = &supermaket_list[1];
    }
    
}



void init_citizens(memory_t *memory) {

    init_house(memory);
    init_building(memory);

    for (int i = 0; i < CITIZENS_COUNT; i++) {
        citizen_t *citizen = &memory->citizens[i];

        citizen->type = NORMAL;
        citizen->health = 10;

        assign_home_to_citizen(memory, citizen);
        //printf("maison du citoyen %d est la maison %p\n", i+1, citizen->home);
        assign_company_to_citizen(memory, citizen);
        // printf("entreprise du citoyen %d est l'entreprise %p\n", i+1, citizen->workplace);
        assign_random_supermarket(memory, citizen);
        // printf("Le supermarché le plus proche du citoyen %d est %p\n", i+1, citizen->supermarket);
    }
        
}


void init_surveillance(surveillanceNetwork_t *surveillanceNetwork) {
    for (int i = 0; i < MAX_ROWS; ++i) {
        for (int j = 0; j < MAX_COLUMNS; ++j) {
            /* Standard cameras enabled by default */
            surveillanceNetwork->devices[i][j].standard_camera = 1;
            /* Lidars enabled by default */
            surveillanceNetwork->devices[i][j].lidar = 1;
        }
    }
    /* Initialization of the AI state */
    surveillanceNetwork->surveillanceAI.suspicious_movement = 0; // No suspicious movement detected initially
}



memory_t *create_shared_memory(const char *name) {
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        handle_fatal_error("shm_open");
    }

    size_t size = sizeof(struct memory_s);

    if (ftruncate(shm_fd, size) == -1) {
        handle_fatal_error("ftruncate");
    }

    memory_t *shared_memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        handle_fatal_error("mmap");
    }

    close(shm_fd);

    printf("avant de faire les init");

    /* Initialize the shared memory */
    shared_memory->memory_has_changed = 0;
    shared_memory->simulation_has_ended = 0;
    init_map(&shared_memory->map);
    init_citizens(shared_memory);
    init_surveillance(&shared_memory->surveillanceNetwork);

    return shared_memory;
}

sem_t *create_semaphore(const char *name, int value) {
    sem_t *sem = sem_open(name, O_CREAT, 0644, value);
    if (sem == SEM_FAILED) {
        handle_fatal_error("sem_open failed");
    }
    return sem;
}

sem_t *open_semaphore(const char *name) {
    sem_t *sem = sem_open(name, 0);
    if (sem == SEM_FAILED) {
        handle_fatal_error("sem_open failed");
    }
    return sem;
}

void start_simulation_processes(){
    // pid_t pid_monitor, pid_enemy_spy_network, pid_citizen_manager, pid_enemy_country,
    // pid_counterintelligence_officer, pid_timer;
    int num_children =0;
    pid_t pidExecutables[3];

    pidExecutables[num_children] = fork();
    if (pidExecutables[num_children] == -1) {
        handle_fatal_error("Error [fork()] monitor:");
    }
    if (pidExecutables[num_children] == 0) {
        if (execl("./bin/monitor", "monitor", NULL) == -1) {
            handle_fatal_error("Error [execl] monitor: ");
        }
        
    }
    num_children++;

    pidExecutables[num_children] = fork();
    if (pidExecutables[num_children] == -1) {
        handle_fatal_error("Error [fork()] timer: ");
    }
    if (pidExecutables[num_children] == 0) {
        if (execl("./bin/timer", "timer", NULL) == -1) {
            handle_fatal_error("Error [execl] timer: ");
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
    
   
    for (int i = 0; i < num_children; i++) {
        int status;
        waitpid(pidExecutables[i], &status, 0);
    }
    
    int statusSharedMemory;

    /* Replace 'file_name.txt' with the name of the file you want to delete */
    statusSharedMemory = remove("/dev/shm/SharedMemory");

    if (statusSharedMemory == 0)
        printf("File deleted successfully\n");
    else
        printf("Error: unable to delete the file\n");

    return 0;
    

    //pid_counterintelligence_officer = start_counterintelligence_officer();

    //pid_enemy_country = start_enemy_country();

    //pid_enemy_spy_network = start_enemy_spy_network();

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

