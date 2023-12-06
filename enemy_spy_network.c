#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>

#include "enemy_spy_network.h"
#define NUM_AGENTS 3


void handle_fatal_error(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

memory_t* memory

int spy_choice(spy_t spy){
  if(rand()%100 < 10){
    return SUPERMARKET;
  }else if(rand()%100 < 30){
    return -1; /* Go back home */
  }else{
    return WASTELAND;
  }
}

/* Return 1 if the spy has already stolen this company */
int already_stolen(spy_t spy){
  int i, j, k;

  i = spy.location_row;
  j = spy.location_column;

  for(k = 0; k < 8; k++){
    if(memory->map.cells[i][j].row == spy.allowed_company[k].row && memory->map.cells[i][j].column == spy.allowed_company[k].column){
      return 1;
    }
  }
  return 0;

}

/* Return 1 if someone else (another thief) has already stolen this company */
int someone_stolen(int row, int column){

  if(memory->map.cells[row][column].allowed_thief){
    return 0;
  }
  if(memory->map.cells[row][column].last_thief >= memory->map.cells[row][column].last_thief +"???" ){
    memory->map.cells[row][column].allowed_thief = 1;
    memory->map.cells[row][column].allowed_thief = memory->map.cells[row][column].allowed_thief + "???";
    return 1;
  }
  return 1;
}

/* Return 1 if the spy achieves to steal the company */
int possible_theft(spy_t spy, int t){
  
  if(!already_stolen(spy)){

    if(102 <= t && t < 120){             /* 17h - 20h => 2%  */
      if(rand()%100 < 2){
        return 1;
      } else if(120 <= t && t < 144) {   /* 20h - 00h => 10% */
            if (rand()%100 < 10) {
                return 1;
            }
        } else if(0 <= t && t < 18) {    /* 00h - 3h  => 76% */
            if (rand()%100 < 76) {
                return 1;
            }
        } else if(18 <= t && t < 30){    /* 03h - 05h => 10%*/
            if(rand()%100 < 10){
                return 1;
            }
        } else if(30 <= t && t < 42){    /* 05h - 08h => 2%*/
           if(rand()%100 < 2){
                return 1;
            }
        }
    }
  }
  return 0;
}

void check_position(memory_t* memory, int choice){
  int row, column;

  if(choice == 0 || choice == 1 || choice == 2){
    column = memory->spies[choice].location_column;
    row = memory->spies[choice].location_row;
  }else if(choice == 3){
    column = memory->case_officer.location_column;
    row = memory->case_officer.location_row;
  }else if(choice == 4){
    column = memory->counterintelligence_officer.location_column;
    row = memory->counterintelligence_officer.location_row;
  }else{
    return;
  }

  if(column < 0){
    column = 0;
  }else if(column > 6){
    column = 6;
  }

  if(row < 0){
    row = 0;
  }else if(row > 6){
    row = 6;
  }

  if(choice == 0 || choice == 1 || choice == 2){
    memory->spies[choice].location_column = column;
    memory->spies[choice].location_row = row;
  }else if(choice == 3){
    memory->case_officer.location_column = column;
    memory->case_officer.location_row = row;
  }else if(choice == 4){
    memory->counterintelligence_officer.location_column = column;
    memory->counterintelligence_officer.location_row = row;
  }

}


void spy_move_around_company(spy_t* spy, cell_t pos){
  int move_col, move_row;
  int random = rand()%2;
  int r = rand()%2;

  move_col = spy->location_column - pos.column;
  move_row = spy->location_row - pos.row;

  if(move_col > 0 && move_row > 0){       
    if(random){
      spy->location_row = spy->location_row - 1;
    }else{
      spy->location_column = spy->location_column - 1;  
    }
  }else if(move_col > 0 && move_row < 0){ 
    if(random){
      spy->location_row = spy->location_row + 1;
    }else{
      spy->location_column = spy->location_column - 1;  
    }
  }else if(move_col < 0 && move_row > 0){ 
    if(random){
      spy->location_row = spy->location_row - 1;
    }else{
      spy->location_column = spy->location_column + 1;  
    }
  }else if(move_col < 0 && move_row < 0){ 
    if(random){
      spy->location_row = spy->location_row + 1;
    }else{
      spy->location_column = spy->location_column + 1;  
    }
  }else if(move_row == 0){ 
    spy->location_row = spy->location_row + pow(-1, r)*random;
  }else if(move_col == 0 ){ 
    spy->location_column = spy->location_column + pow(-1, r)*random;
  }
  check_position(memory, spy->id);
  memory->memory_has_changed = 1;
}

/* Return 1 if the spy wants to steal the company */
int consider_theft(spy_t* spy, int* pos){
  cell_t company_pos;

  company_pos = memory->map.cells[pos[0]][pos[1]];
  while(memory->turns%18 < 12){
    spy_move_around_company(spy, company_pos);
  }

  if(memory->turns%18 > 12){
    if(rand()%100 < 85){
      return 1;
    }
  }
  return 0;
}


/* theft */
void theft(spy_t* spy){
  if(rand()%100 < 90){
    spy->allowed_company[spy->nb_of_stolen_companies] = memory->map.cells[spy->location_row][spy->location_column];
    spy->nb_of_stolen_companies++;
    strcpy(spy->stolen_message_content, "This is not a fake message");      
  }
}



void move_spy(spy_t* spy){

  spy->location_column = spy->location_column + (pow(-1, rand()%4))*(rand()%2);
  spy->location_row = spy->location_row + (pow(-1, rand()%4))*(rand()%2);
  check_position(memory, spy->id);
}