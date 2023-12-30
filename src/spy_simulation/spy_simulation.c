#include "memory.h"
// #include "citizen_manager.h"
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
            cityMap->cells[i][j].has_mailbox = 0;
        }
    }

    cityMap->cells[3][3].type = CITY_HALL;
    cityMap->cells[3][3].nb_of_characters = 20;

    place_building_randomly(cityMap, SUPERMARKET, 2, 30);
    place_building_randomly(cityMap, COMPANY, 8, 50);
    place_building_randomly(cityMap, RESIDENTIAL_BUILDING, 11, 15);
}


double distance(unsigned int pos1[2], unsigned int pos2[2]) {
    return abs((int)pos1[0] - (int)pos2[0]) + abs((int)pos1[1] - (int)pos2[1]);
}

void init_house(memory_t *memory){
    int fakeHome = rand() % NB_HOMES;
    printf("=====================================\n");
    printf("fake home %d\n", fakeHome);
    printf("La maison %p est la maison avec la boite au lettre piégee\n", &memory->homes[fakeHome]);
    printf("\n\n=====================================\n");
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
    // Assign a random house, respecting max capacity
    int house_index;
    int attempts = 0;
    while (attempts < NB_HOMES) {
        house_index = rand() % NB_HOMES;
        if (houses[house_index].nb_citizen < houses[house_index].max_capacity) {
            citizen->home = &houses[house_index];
            houses[house_index].nb_citizen++;
            break;  // Sortie de la boucle une fois qu'une maison est trouvée
        }
        attempts++;
    }
}



void assign_company_to_citizen(memory_t* memory, citizen_t* citizen){
    
    building_t *company_list = memory->companies;
    // Assign a random company, respecting max capacity
    int company_index;
    int attempts = 0;  // Compteur pour éviter une boucle infinie

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
    
    // Find nearest supermarket
    building_t supermaket_list[NB_STORE] = {memory->companies[0], memory->companies[1]};
    // Les deux premiers emplacements sont donnés aux supermarchés
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
    srand(time(NULL));

    init_house(memory);
    init_building(memory);

    for (int i = 0; i < CITIZENS_COUNT; i++) {
        citizen_t *citizen = &memory->citizens[i];

        citizen->type = NORMAL;
        citizen->health = 10;
        // // printf("ftg Haykel ton micro de merde\n");

        assign_home_to_citizen(memory, citizen);
        printf("maison du citoyen %d est la maison %p\n", i+1, citizen->home);
        assign_company_to_citizen(memory, citizen);
        printf("entreprise du citoyen %d est l'entreprise %p\n", i+1, 
                                                     citizen->workplace);
        assign_random_supermarket(memory, citizen);
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
    init_citizens(shared_memory);
    init_surveillance(&shared_memory->surveillanceNetwork);
    shared_memory->mqInfo = init_mq();

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
    pid_t pidExecutables[3];

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
