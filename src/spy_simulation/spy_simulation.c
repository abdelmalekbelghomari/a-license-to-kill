#include "memory.h"
#include "../../include/citizen_manager.h"


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
    srand(time(NULL));
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
    init_map(&shared_memory->cityMap);
    init_citizens(&shared_memory->citizens);
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