#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>

#include "enemy_spy_network.h"

void handle_fatal_error(const char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

memory_t* memory;

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
  if(memory->map.cells[row][column].last_thief >= memory->map.cells[row][column].last_thief + 1296 ){
    memory->map.cells[row][column].allowed_thief = 1;
    memory->map.cells[row][column].allowed_thief = memory->map.cells[row][column].allowed_thief + 1296;
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


spyInfo* get_info_spy(int is_killer, int id){
  spyInfo* data = (spyInfo*)malloc(sizeof(spyInfo));
  int random = rand()%11;

  data->id                     = id ;
  data->health_point           = 10 ;
  data->location_row           = 1  ;
  data->location_column        = 0  ;
  data->home_row               = 1  ;
  data->home_column            = 0  ;
  data->nb_of_stolen_companies = 0  ;
  data->has_license_to_kill = is_killer;
  strcpy(data->stolen_message_content, "...");


  return data;
}

caseOfficerInfo* get_info_case_officer(){
  caseOfficerInfo* data = (caseOfficerInfo*)malloc(sizeof(caseOfficerInfo));

  data->id                     = 4  ;
  data->health_point           = 10 ;
  data->location_row           = 4  ;
  data->location_column        = 1  ;
  data->home_row               = 4  ;
  data->home_column            = 1  ;
  data->mailbox_row            = 6  ;
  data->mailbox_column         = 6  ;

  return data;
}

void fill_company_spy(memory_t* memory, int n){
  int i, j, k;

  k = 0;
  for(i = 0; i < 7; i++){
    for(j = 0; j < 7; j++){
      if(memory->map.cells[i][j].type == COMPANY){
        memory->spies[n].allowed_company[k] = memory->map.cells[i][j];
        k++;
      }
    }
  }
}

void go_to_mail_box(memory_t* memory, int choice){
  int row, column;
  int move_row, move_column;
  cell_t mail_box;
  mail_box = memory->map.cells[6][6];
  
  if(choice == 0 || choice == 1 || choice == 2){
    row = memory->spies[choice].location_row;
    column = memory->spies[choice].location_column;
  }else if(choice == 3){
    row = memory->case_officer.location_row;
    column = memory->case_officer.location_column;
  }else if(choice == 4){
    row = memory->counterintelligence_officer.location_row;
    column = memory->counterintelligence_officer.location_column;
  }else{
    return;
  }

  while(!memory->map.cells[column][row].is_mailbox){
    move_row = mail_box.row - row;
    move_column = mail_box.column - column;

    if(move_row != 0){
      row = row + move_row/abs(move_row);
    }
    if(move_column != 0){
      column = column + move_column/abs(move_column);
    }

    if(choice == 0 || choice == 1 || choice == 2){
      memory->spies[choice].location_row = row;
      memory->spies[choice].location_column = column;
    }else if(choice == 3){
      memory->case_officer.location_row = row;
      memory->case_officer.location_column = column;
    }else if(choice == 4){
      memory->counterintelligence_officer.location_row = row;
      memory->counterintelligence_officer.location_column = column;
    }
  }
}


int* is around(int row, int column, int cell_type) {
  int i,j ;
  int pos_row, pos_column;
  int pos[2];
  pos[0] = 0;
  pos[1] = 0;

  for(i = -1; i <= 1; i++) {
    for(j = -1; j <= 1; j++) {
      pos_row = column + i;
      pos_column = row + j;
      if(!(0 <= pos_row && pos_row <= 6)){
        pos_row = row;
      }
      if(!(0 <= pos_column && pos_column <= 6)){
        pos_column = pos_column;
      }
      if(memory->map.cells[pos_column][pos_row].type == cell_type) {
        pos[0] = pos_column;
        pos[1] = pos_row;
        return pos;
      }
    }
  }
  return pos;
}

void case_officer_go_shopping(case_officer_t* case_officer){
  int row, column;
  int move_row, move_column;
  cell_t market = memory->map.cells[3][4];
  
  row = case_officer->location_row;
  column = case_officer->location_column;

  while(memory->map.cells[column][row].type != SUPERMARKET){
    move_row = market.row - row;
    move_column = market.column - column;

    if(move_row != 0){
      row = row + move_row/abs(move_row);
    }
    if(move_column != 0){
      column = column + move_column/abs(move_column);
    }
    case_officer->location_row = row;
    case_officer->location_column = column;
  }
}

void case_officer_go_home(case_officer_t* case_officer){
  int row, column;
  int move_row, move_column;
  
  row = case_officer->location_row;
  column = case_officer->location_column;

  while(!(column == case_officer->home_column && row == case_officer->home_row)){
    move_row = case_officer->home_row - row;
    move_column = case_officer->home_column - column;

    if(move_row != 0){
      row = row + move_row/abs(move_row);
    }
    if(move_column != 0){
      column = column + move_column/abs(move_column);
    }
    case_officer->location_row = row;
    case_officer->location_column = column;
  }
}

void* spy_life(void* thread) {
  spyInfo* thread_data = (spyInfo*) thread;
  /* INIT */
  int spy_row, spy_column, theft_round;
  int *pos, tmp[2];
  sem_t *sem = sem_open (MY_SEM, O_CREAT, 0666, 1);

  if(sem == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  pos = (int*)malloc(2*sizeof(int));
  tmp[0] = 0;
  tmp[1] = 0;

  sem_wait(&sem);
  memory->spies[thread_data->id].id                         = thread_data->id;
  memory->spies[thread_data->id].health_point               = thread_data->health_point;
  memory->spies[thread_data->id].location_row               = thread_data->location_row;
  memory->spies[thread_data->id].location_column            = thread_data->location_column;
  memory->spies[thread_data->id].home_row                   = thread_data->home_row;
  memory->spies[thread_data->id].home_column                = thread_data->home_column;
  memory->spies[thread_data->id].nb_of_stolen_companies     = thread_data->nb_of_stolen_companies;
  memory->spies[thread_data->id].has_license_to_kill        = thread_data->has_license_to_kill;
  strcpy(memory->spies[thread_data->id].stolen_message_content, thread_data->stolen_message_content);
  fill_company_spy(memory, thread_data->id);
  memory->map.cells[thread_data->location_column][thread_data->home_row].characters++;
  sem_post(&sem);

  while(!memory->simulation_has_ended){
    sem_wait(&sem);
    spy_row = memory->spies[thread_data->id].location_row;
    spy_column = memory->spies[thread_data->id].location_column;
    sem_post(&sem);
    pos = is_around(spy_row, spy_column, COMPANY); 
    if(pos != tmp){
      theft_round = memory->timer.round%144;
      if(possible_theft(memory->spies[thread_data->id], theft_round)){
        if(consider_theft(&memory->spies[thread_data->id], pos)){
          sem_wait(&sem);
          theft(&memory->spies[thread_data->id]);
          go_to_mail_box(memory, thread_data->id);
          /* Depose là un message dans la mail box */
          sem_post(&sem);
          }
      }else{
        sem_wait(&sem);
        move_spy(&memory->spies[thread_data->id]);
        sem_post(&sem);
      }
    }else{
      sem_wait(&sem);
      move_spy(&memory->spies[thread_data->id]);
      sem_post(&sem);
    }
    sem_wait(&sem);
    memory->memory_has_changed = 1;
    sem_post(&sem);
  }
  return NULL;

}

