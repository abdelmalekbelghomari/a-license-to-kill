#ifndef SPY_SIMULATION
#define SPY_SIMULATION

#include "memory.h"


enum citizen_type_e {
    NORMAL,
    SPY,
    CASE_OFFICER,
    COUNTER_INTELLIGENCE_OFFICER
};

typedef enum citizen_type_e citizen_type_t;

struct Citizen {
    citizen_type_t type;
    int health;
    int positionX;
    int positionY;
    cell_type_t workPlace;
};

typedef struct Citizen citizen_t;


void init_map();
void init_citizens();


#endif