#include "spy_simulation.h"

int main(){
    memory_t* memory;
    pid_t pid_monitor, pid_enemy_spy_network, pid_citizen_manager, pid_enemy_country, pid_counterintelligence_officer, pid_timer;
    
    memory = create_shared_memory("/spy_simulation");

    pid_citizen_manager = start_citizen_manager();
    pid_monitor = start_monitor();
    pid_counterintelligence_officer = start_counterintelligence_officer();
    pid_enemy_country = start_enemy_country();
    pid_enemy_spy_network = start_enemy_spy_network();
    pid_timer = start_timer();

    return EXIT_SUCCESS;
}


void handle_fatal_error(const char *message){
  perror(message);
  exit(EXIT_FAILURE);
}

void init_map(map_t * cityMap){
    /* Init all cells as empty terrain*/
    int i,j;
    for (i = 0; i < MAX_ROWS; i++){
        for (j = 0; j < MAX_COLUMNS; j++){
            cityMap->cells[i][j].type = WASTELAND;
            cityMap->cells[i][j].current_capacity = 0;
            cityMap->cells[i][j].nb_of_characters = 0;
            cityMap->cells[i][j].has_mailbox = 0;
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

    /* Adding mailbox */
    int mailboxPlaced = 0;
    while (!mailboxPlaced) {
        i = rand() % MAX_ROWS;
        j = rand() % MAX_COLUMNS;
        if (cityMap->cells[i][j].type == WASTELAND){
            cityMap->cells[i][j].has_mailbox = 1;
            cityMap->mailbox_row = i;
            cityMap->mailbox_column = j;
            mailboxPlaced = 1;
        }
    }
}


void place_citizens_on_map(map_t *cityMap, Citizen *citizens){
    /* The counter intelligence officer is in the city hall */
    citizens[CI_OFFICER_INDEX].location_row = CITY_HALL_ROW;
    citizens[CI_OFFICER_INDEX].location_column = CITY_HALL_COLUMN;
    citizens[CI_OFFICER_INDEX].currentBuilding = CITY_HALL;
    citizens[CI_OFFICER_INDEX].at_home = 0;

    for (int i = 0; i < CITIZENS_COUNT; i++) {
        if (i != CI_OFFICER_INDEX) {
            int placed = 0;
            while (!placed) {
                int x = rand() % MAX_ROWS;
                int y = rand() % MAX_COLUMNS;

                if (cityMap->cells[x][y].type == RESIDENTIAL_BUILDING && !(cityMap->cells[x][y].has_mailbox)) {
                    if (!is_spy(citizens[i]) || is_within_distance(cityMap, x, y, 4)) {
                        citizens[i].location_row = x;
                        citizens[i].location_column = y;
                        citizens[i].currentBuilding = RESIDENTIAL_BUILDING;
                        citizens[i].at_home = 1;
                        cityMap->cells[x][y].nb_of_characters++;
                        placed = 1;
                    }
                }
            }
        }
    }
}

int is_spy(citizen_t citizen){
    return citizen.type == SPY;
}

int is_within_distance(map_t *cityMap, int x, int y, int max_distance){
    int mailbox_x = cityMap->mailbox_row;
    int mailbox_y = cityMap->mailbox_column;

    int distance = abs(x - mailbox_x) + abs(y - mailbox_y);

    return distance <= max_distance;
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
        handle_fatal_error("shm_open")
    }

    size_t size = sizeof(struct memory_s);

    if (ftruncate(shm_fd, size) == -1) {
        handle_fatal_error("ftruncate");
    }

    struct memory_s *shared_memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        handle_fatal_error("mmap");
    }

    close(shm_fd);

    // Initialize the shared memory as necessary
    shared_memory->memory_has_changed = 0;
    shared_memory->simulation_has_ended = 0;
    init_map(&shared_memory->cityMap);
    place_citizens_on_map(&shared_memory->cityMap, &shared_memory->citizens);
    init_surveillance(&shared_memory->surveillanceNetwork);
    shared_memory->mqInfo = init_mq();

    return shared_memory;
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

void signals(){
    struct sigaction action;

    action.sa_handler = handle_signal;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);

    /* Set signal handlers */
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGUSR2, &action, NULL);
}
