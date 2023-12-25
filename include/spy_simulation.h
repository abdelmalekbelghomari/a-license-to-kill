#ifndef SPY_SIMULATION
#define SPY_SIMULATION 

#include "memory.h"
#include "character.h"
#include <stdlib.h>
#include <time.h>
/*#include <sys/mmap.h>*/
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct memory_s memory_t;


void init_map(map_t * cityMap);
void init_citizens(citizen_t *citizens);
void init_surveillance(surveillanceNetwork_t *surveillance);
void start_simulation_process();
struct memory_s *create_shared_memory(const char *name);

#endif // SPY_SIMULATION