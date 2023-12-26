#ifndef SPY_SIMULATION
#define SPY_SIMULATION 

#include "memory.h"
#include <stdlib.h>
#include <time.h>
/*#include <sys/mmap.h>*/
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct memory_s memory_t;

typedef struct memory_s memory_t;

#define CI_OFFICER_INDEX 4

void handle_fatal_error(const char *message);
void init_map(map_t * cityMap);
void place_citizens_on_map(map_t *cityMap, Citizen *citizens);
int is_spy(citizen_t citizen);
int is_within_distance(map_t *cityMap, int x, int y, int max_distance);
void init_surveillance(surveillanceNetwork_t *surveillance);
void start_simulation_processes();
struct memory_s *create_shared_memory(const char *name);
int start_citizen_manager();
int start_monitor();
int start_enemy_spy_network();
int start_counterintelligence_officer();
int start_enemy_country();
int start_timer();

#endif // SPY_SIMULATION