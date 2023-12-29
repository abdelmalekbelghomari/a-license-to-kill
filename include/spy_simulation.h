#ifndef SPY_SIMULATION
#define SPY_SIMULATION 

#include "memory.h"
#include <stdlib.h>
#include <time.h>
/*#include <sys/mmap.h>*/
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct memory_s memory_t;


void init_map(map_t * cityMap);
void init_citizens(citizen_t *citizens);
void init_surveillance(surveillanceNetwork_t *surveillance);
void start_simulation_processes();
struct memory_s *create_shared_memory(const char *name);
bool isConnected(map_t *cityMap);
void dfs(map_t *cityMap, int x, int y, bool visited[MAX_ROWS][MAX_COLUMNS]);
mqd_t init_message_queue();
sem_t* init_sem();

#endif // SPY_SIMULATION