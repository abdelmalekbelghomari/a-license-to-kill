#ifndef SPY_SIMULATION
#define SPY_SIMULATION

#include "memory.h"
#include "character.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
/*#include <sys/mmap.h>*/ /* i m working on Windows so this line is given me an error
                             but if u re working on Linux i guess that's not gonna be a problem */
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void init_map(map_t * cityMap);
void init_citizens(citizen_t *citizens);
void init_surveillance(surveillanceNetwork_t *surveillance);
struct memory_s *create_shared_memory(const char *name);

#endif