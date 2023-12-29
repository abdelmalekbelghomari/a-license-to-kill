#include "memory.h"
#include "../../include/citizen_manager.h"
#include <time.h>
#include <stdbool.h>
#include <semaphore.h>
#define MAX_MESSAGE_SIZE 256


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

mqd_t init_message_queue() {
    struct mq_attr attr;
    attr.mq_flags = 0; // Blocage ou non-blocage
    attr.mq_maxmsg = 10; // Nombre maximal de messages dans la queue
    attr.mq_msgsize = MAX_MESSAGE_SIZE; // Taille maximale d'un message
    attr.mq_curmsgs = 0; // Nombre initial de messages

    mqd_t mq = mq_open("/messageQueue", O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open error");
        exit(EXIT_FAILURE);
        
    }
    return mq; 
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
            surveillanceNetwork->devices[i][j].lidar = 1; // Lidars enabled by default
        }
    }
    // Initialization of the AI state
    surveillanceNetwork->surveillanceAI.suspicious_movement = 0; // No suspicious movement detected initially
}

sem_t* init_sem() {
    // Créer un nouveau sémaphore ou ouvrir un existant
    sem_t *sem = sem_open("/Mysemaphore", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    printf("\nla sémaphore est crée et fonctionne!\n");

    return sem;
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
    shared_memory->mqInfo.mq = init_message_queue();
    init_sem();
    

    return shared_memory;
}

void start_simulation_processes() {
    pid_t pid_timer, pid_citizen_manager, pid_enemy_spy_network, pid_enemy_country;

    // Start timer process
    // pid_timer = fork();
    // if (pid_timer == 0) {
    //     execl("./bin/timer", "timer", NULL);
    //     perror("Error [execl] timer: ");
    //     exit(EXIT_FAILURE);
    // } else if (pid_timer < 0) {
    //     perror("Error [fork()] timer: ");
    //     exit(EXIT_FAILURE);
    // }

    // Start citizen manager process
    /*pid_citizen_manager = fork();
    if (pid_citizen_manager == 0) {
        execl("./bin/citizen_manager", "citizen_manager", NULL);
        perror("Error [execl] citizen_manager: ");
        exit(EXIT_FAILURE);
    } else if (pid_citizen_manager < 0) {
        perror("Error [fork()] citizen_manager: ");
        exit(EXIT_FAILURE);
    }*/

    // Start enemy spy network process
    pid_enemy_spy_network = fork();
    if (pid_enemy_spy_network == 0) {
        execl("./bin/enemy_spy_network", "enemy_spy_network", NULL);
        perror("Error [execl] enemy_spy_network: ");
        exit(EXIT_FAILURE);
    } else if (pid_enemy_spy_network < 0) {
        perror("Error [fork()] enemy_spy_network: ");
        exit(EXIT_FAILURE);
    }

    // Start enemy country process
    pid_enemy_country = fork();
    if (pid_enemy_country == 0) {
        execl("./bin/enemy_country", "enemy_country", NULL);
        perror("Error [execl] enemy_country: ");
        exit(EXIT_FAILURE);
    } else if (pid_enemy_country < 0) {
        perror("Error [fork()] enemy_country: ");
        exit(EXIT_FAILURE);
    }

    // Wait for processes to finish
    int status;
    waitpid(pid_timer, &status, 0);
    waitpid(pid_citizen_manager, &status, 0);
    waitpid(pid_enemy_spy_network, &status, 0);
    waitpid(pid_enemy_country, &status, 0);
}

