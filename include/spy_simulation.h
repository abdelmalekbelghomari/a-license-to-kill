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
#include <semaphore.h>

typedef struct memory_s memory_t;


void init_map(map_t * cityMap);
void init_citizens(memory_t *memory);
void init_surveillance(surveillanceNetwork_t *surveillance);
void start_simulation_processes();
struct memory_s *create_shared_memory(const char *name);
bool isConnected(map_t *cityMap);
bool dfs(map_t *cityMap, bool visited[MAX_ROWS][MAX_COLUMNS], int row, int col, int endRow, int endCol);
sem_t *create_semaphore(const char *name, int value);
sem_t *open_semaphore(const char *name);
void start_simulation_processes();

#endif // SPY_SIMULATION