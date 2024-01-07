/*
 * The License to Kill Project
 *
 * Copyright (C) 2021 Alain Lebret <alain.lebret [at] ensicaen [dot] fr>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <string.h>
#include <ncurses.h>


#include "monitor.h"
#include "monitor_common.h"
#include "memory.h"
#include "timer.h"

WINDOW *main_window;
WINDOW *city_window;
WINDOW *character_window;
WINDOW *mailbox_content_window;
WINDOW *enemy_country_monitor;
extern sem_t *sem_spy_producer, *sem_spy_consumer, *sem_memory;


int old_cursor;
int cell_type_colors[5];
int colored_text[8];
int color_pair_black;

void detect_mouse()
{
    mmask_t mask;

    if (NCURSES_MOUSE_VERSION > 0) {
        addstr("This version of NCurses supports the mouse.\n");
    } else {
        addstr("This version of NCurses does not support the mouse.\n");
    }
    refresh();
    mask = mousemask(ALL_MOUSE_EVENTS, NULL);
    if (mask == 0) {
        addstr("Unable to access the mouse on this terminal.\n");
    } else {
        addstr("Mouse events can be captured.\n");
    }

    getch();
}

void create_color_pairs()
{
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_WHITE, COLOR_GREEN);
    init_pair(4, COLOR_WHITE, COLOR_YELLOW);
    init_pair(5, COLOR_WHITE, COLOR_BLUE);
    init_pair(6, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(7, COLOR_WHITE, COLOR_CYAN);
    init_pair(8, COLOR_WHITE, COLOR_WHITE);

    cell_type_colors[RESIDENTIAL_BUILDING] = COLOR_PAIR(5);
    cell_type_colors[WASTELAND] = COLOR_PAIR(3);
    cell_type_colors[COMPANY] = COLOR_PAIR(2);
    cell_type_colors[SUPERMARKET] = COLOR_PAIR(4);
    cell_type_colors[CITY_HALL] = COLOR_PAIR(6);

    colored_text[COLOR_BLACK] = COLOR_PAIR(1);
    colored_text[COLOR_RED] = COLOR_PAIR(2);
    colored_text[COLOR_GREEN] = COLOR_PAIR(3);
    colored_text[COLOR_YELLOW] = COLOR_PAIR(4);
    colored_text[COLOR_BLUE] = COLOR_PAIR(5);
    colored_text[COLOR_MAGENTA] = COLOR_PAIR(6);
    colored_text[COLOR_CYAN] = COLOR_PAIR(7);
    colored_text[COLOR_WHITE] = COLOR_PAIR(8);

    init_pair(9, COLOR_BLACK, COLOR_BLACK);
    color_pair_black = COLOR_PAIR(9);
}

void init_monitor_elements(WINDOW *window, memory_t *mem, int rows, int columns)
{
    /* --------------------------------------------------------------------- */
    /*                 Get information from mem to get the map               */
	//map_t map;
   /* ---------------------------------------------------------------------- */

    set_monitor_title(window, "LICENSE TO KILL (v. 0.2)");

    city_window = subwin(window, rows / 2 + 8, columns / 2, 2, 0);                   /* Top left */
    character_window = subwin(window, rows / 2 + 8, 0, 2, columns / 2);              /* Top right */
    mailbox_content_window = subwin(window, 0, columns / 2, rows / 2 + 10, 0);        /* Bottom left */
    enemy_country_monitor = subwin(window, 0, columns / 2, rows / 2 + 10, columns / 2);  /* Bottom right */

    box(city_window, 0, 0);
    box(character_window, 0, 0);
    box(mailbox_content_window, 0, 0);
    box(enemy_country_monitor, 0, 0);

    show_general_information(city_window);
    // display_city(city_window, mem->map, rows, columns , mem);
    display_character_information(character_window, mem);
    display_mailbox_content(mailbox_content_window, mem);
    display_enemy_country_monitor(enemy_country_monitor, mem);
}

void set_monitor_title(WINDOW *window, const char *title)
{
    int title_column;

    title_column = main_window->_maxx / 2 - strlen(title) / 2;
    wattron(window, A_BOLD);
    mvwprintw(main_window, 0, title_column, "%s", title);
    wattroff(window, A_BOLD);
}

void set_city_legend(WINDOW *window, int row, int col)
{
    wattron(window, A_BOLD);
    mvwprintw(window, row, col, "City map caption");
    wattroff(window, A_BOLD);

    set_cell_color(window, cell_type_colors[CITY_HALL], row + 2, col);
    mvwprintw(window, row + 2, col + 4, "City Hall");

    set_cell_color(window, cell_type_colors[RESIDENTIAL_BUILDING], row + 4, col);
    mvwprintw(window, row + 4, col + 4, "Residential building");

    set_cell_color(window, cell_type_colors[COMPANY], row + 6, col);
    mvwprintw(window, row + 6, col + 4, "Company");

    set_cell_color(window, cell_type_colors[SUPERMARKET], row + 8, col);
    mvwprintw(window, row + 8, col + 4, "Supermarket");

    set_cell_color(window, cell_type_colors[WASTELAND], row + 10, col);
    mvwprintw(window, row + 10, col + 4, "Wasteland");

    mvwprintw(window, row + 16, col, "Characters:");
    mvwprintw(window, row + 17, col + 2, "S - Spy");
    mvwprintw(window, row + 18, col + 2, "* - Citizen");
    mvwprintw(window, row + 19, col + 2, "I - Counter Intelligence Officer");
    mvwprintw(window, row + 20, col + 2, "O - Case Officer");
    mvwprintw(window, row + 21, col + 2, "M - Mailbox");
}


void display_city(WINDOW *window, map_t map, int rows, int columns, memory_t *memory) {
    int map_columns = 7; 
    int map_rows = 7;
    int type;

    int row_offset, col_offset;

    set_city_legend(window, 6, 2);

    for (int i = 0; i < map_columns; i++) {
        for (int j = 0; j < map_rows; j++) {
            row_offset = 8 + j;
            col_offset = 40 + (i * 3);
            type = map.cells[i][j].type;

            switch (type) {
                case SUPERMARKET:
                    wattron(window, colored_text[COLOR_YELLOW]);
                    mvwaddstr(window, row_offset, col_offset, " s ");
                    wattroff(window, colored_text[COLOR_YELLOW]);
                    break;
                case RESIDENTIAL_BUILDING:
                    wattron(window, colored_text[COLOR_BLUE]);
                    mvwaddstr(window, row_offset, col_offset, " r ");
                    wattroff(window, colored_text[COLOR_BLUE]);
                    break;
                case COMPANY:
                    wattron(window, colored_text[COLOR_RED]);
                    mvwaddstr(window, row_offset, col_offset, " c ");
                    wattroff(window, colored_text[COLOR_RED]);
                    break;
                case CITY_HALL:
                    wattron(window, colored_text[COLOR_MAGENTA]);
                    mvwaddstr(window, row_offset, col_offset, " h ");
                    wattroff(window, colored_text[COLOR_MAGENTA]);
                    break;
                default:
                    wattron(window, colored_text[COLOR_GREEN]);
                    mvwaddstr(window, row_offset, col_offset, " . ");
                    wattroff(window, colored_text[COLOR_GREEN]);
                    break;
            }

            if(memory->map.mailbox_column == j && memory->map.mailbox_row == i){
                wattron(window, colored_text[COLOR_WHITE]);
                mvwaddstr(window, row_offset, col_offset+2, "M");
                wattroff(window, colored_text[COLOR_WHITE]);
            }
            // les boucles for ici faisaient beuguer la map
            if (memory->citizens[52].position[1] == j && memory->citizens[52].position[0] == i){
                mvwaddstr(window, row_offset, col_offset, "*"); // Symbole pour le citoyen
            }
            if (memory->citizens[1].position[1] == j && memory->citizens[1].position[0] == i){
                mvwaddstr(window, row_offset, col_offset, "*"); // Symbole pour le citoyen
            }
            if (memory->citizens[10].position[1] == j && memory->citizens[10].position[0] == i){
                mvwaddstr(window, row_offset, col_offset, "*"); // Symbole pour le citoyen
            }
            if (memory->citizens[69].position[1] == j && memory->citizens[69].position[0] == i){
                mvwaddstr(window, row_offset, col_offset, "*"); // Symbole pour le citoyen
            }

            // // Affichage des espions
            if (memory->spies[0].location_row == i && memory->spies[0].location_column == j) {
                mvwaddstr(window, row_offset, col_offset, "S"); // Afficher l'espion
            }
            if (memory->spies[1].location_row == i && memory->spies[1].location_column == j) {
                    mvwaddstr(window, row_offset, col_offset, "S"); // Afficher l'espion
            }
            if (memory->spies[2].location_row == i && memory->spies[2].location_column == j) {
                    mvwaddstr(window, row_offset, col_offset, "S"); // Afficher l'espion
            }

            
            // // Affichage de l'officier du contre-espionnage
            if (memory->counter_intelligence_officer.location_row == i && memory->counter_intelligence_officer.location_column == j) {
                mvwaddstr(window, row_offset, col_offset, "I"); // Afficher l'officier du contre-espionnage
            }

            // // Affichage de l'officier traitant
            if (memory->case_officer.location_row == i && memory->case_officer.location_column == j) {
                mvwaddstr(window, row_offset, col_offset, "O"); // Afficher l'officier traitant
            }

        }
    }

    wrefresh(window);
}
void set_cell_color(WINDOW *window, int color, int row, int col)
{
    wattron(window, color);
    mvwprintw(window, row, col, "  ");
    wattroff(window, color);
}

void show_general_information(WINDOW *window)
{
    int title_column;
    char *title = "CITY MAP AND GENERAL INFORMATION";

    title_column = window->_maxx / 2 - strlen(title) / 2;

    wattron(window, A_BOLD | A_UNDERLINE);
    mvwprintw(window, 1, title_column, "%s", title);
    wattroff(window, A_BOLD | A_UNDERLINE);

    mvwprintw(window, 20, 2, "Step: ");
    mvwprintw(window, 20, 20, "Time: ");
    wrefresh(window);
}

void display_general_information_values(WINDOW *window, memory_t *mem)
{
    /* --------------------------------------------------------------------- */
    /*                 Get information from mem about simulation    
    */
    int simulation_has_ended;
    int hour;
    int minutes;
    double elapsed_time;
    char *result;

    // sem_wait(sem_producer_timer);
    simulation_has_ended = mem->simulation_has_ended;
    hour = mem->timer.hours;
    minutes = mem->timer.minutes;
    elapsed_time = (double)mem->timer.round;
    result = NULL;
    // sem_post(sem_consumer_timer);
    
   /* ---------------------------------------------------------------------- */

    mvwprintw(window, 20, 8, "%.f", elapsed_time);
    mvwprintw(window, 20, 26, "       ");
    mvwprintw(window, 20, 26, "%.2d h %d", hour, minutes);
    if (simulation_has_ended > 0) {
        switch (simulation_has_ended) {
            case 1:
                result = "Spies have won! The spy network has fled!";
                break;
            case 2:
                result = "Counter officer has discovered the mailbox!";
                break;
            case 3:
                result = "Spies have won! Counter officer did not find the mailbox!";
                break;
            default:
                break;
        }

        mvwprintw(window, 25, 2, "End of simulation: %s", result);
    }
    wrefresh(window);
}

void display_citizen_information(WINDOW *window, memory_t *mem, int row, int column)
{
    /* --------------------------------------------------------------------- */
    /*                 Get information from mem about citizens               */
    int number_of_citizens_at_home;
    int number_of_citizens_at_work;
    int number_of_citizens_walking;


    number_of_citizens_at_home = mem->at_home_citizens; //127;
    number_of_citizens_at_work = mem->at_work_citizens; //0;
    number_of_citizens_walking = mem->walking_citizens; //0;
   /* ---------------------------------------------------------------------- */

    wattron(window, A_BOLD);
    mvwprintw(window, row, column, "Citizens");
    wattroff(window, A_BOLD);
    mvwprintw(window, row + 1, column, "  At home: %.3d", number_of_citizens_at_home);
    mvwprintw(window, row + 2, column, "  At work: %.3d", number_of_citizens_at_work);
    mvwprintw(window, row + 3, column, "  Walking: %.3d", number_of_citizens_walking);
    wrefresh(window);
}

void display_spy_information(WINDOW *window, memory_t *mem, int row, int column, int number)
{
    /* --------------------------------------------------------------------- */
    /*     Get information from mem about the spy with the given number      */
    // sem_wait(sem_spy_producer);
    spy_t * spy = &mem->spies[number];

    int id;
    int health_points;
    int location_row;
    int location_column;
    int home_row;
    int home_column;
    int nb_of_stolen_companies;
    int has_license_to_kill;
    char stolen_message_content[MAX_LENGTH_OF_MESSAGE];
    char state[40];
    

    id                     = spy->id;
    health_points          = spy->health_point;
    location_row           = spy->location_column;
    location_column        = spy->location_row;
    home_row               = spy->home_column;
    home_column            = spy->home_row;
    nb_of_stolen_companies = spy->nb_of_stolen_companies;
    has_license_to_kill    = spy->has_license_to_kill;
    strcpy(state, spy->description);

    if (spy->id == 1){
        id = 7;         // Petite référence à James Bond
    }

	
   /* ---------------------------------------------------------------------- */
   
    wattron(window, A_BOLD);
    mvwprintw(window, row, column, "Spy n°%d", number);
    wattroff(window, A_BOLD);
    if (spy->id == 1){
        mvwprintw(window, row + 1, column, "  Id Code: %.3d (James Bond)", id);
    } else if (spy->id == 0){
        mvwprintw(window, row + 1, column, "  Id Code: %d (Gru)", id);
    } else {
        mvwprintw(window, row + 1, column, "  Id Code: %d (Une Totally Spy !)", id);
    }
    mvwprintw(window, row + 2, column, "  Health: %d", health_points);
    mvwprintw(window, row + 3, column, "  Position: (%d,%d)", location_row, location_column);
    mvwprintw(window, row + 4, column, "  Home pos: (%d,%d)", home_row, home_column);
    mvwprintw(window, row + 5, column, "  Stolen companies: %d", nb_of_stolen_companies);
    if (!(spy->has_a_message)) {
        mvwprintw(window, row + 6, column, "  Message stolen: none");
    } else {
        mvwprintw(window, row + 6, column, "  Message stolen: yes ");
    }
    if (has_license_to_kill) {
        mvwprintw(window, row + 7, column, "  License to kill: yes");
    } else {
        mvwprintw(window, row + 7, column, "  License to kill: no ");
    }
    mvwprintw(window, row + 8, column, "  State: %s", state);


    wmove(window, row + 8, column);
    wclrtoeol(window);

    // Print the state based on the value of state_id
    // if (state_id == 0) {
    //     mvwprintw(window, row + 8, column, "  State: %s", "resting at home");
    // } else {
    //     mvwprintw(window, row + 8, column, "  State: %s", "resting at home");
    // }

    // Refresh the window to display the changes
    wrefresh(window);

    // switch (spy->current_state->id) {
    //     case 0: mvwprintw(window, row + 8, column + 9, "Resting at Home"); break;
    wrefresh(window);
    // sem_post(sem_spy_producer);
}

// char* get_state_of_officer(memory_t *memory){
//     static char state_description[100]; // Taille arbitraire, assez grande pour la plupart des états
//     strncpy(state_description, memory->case_officer.current_state->description, sizeof(state_description) - 1);
//     state_description[sizeof(state_description) - 1] = '\0'; // Assurez-vous que la chaîne est terminée par '\0'
//     return state_description;
// }

char* get_state_of_officer(memory_t *memory) {
    // Taille arbitraire, assurez-vous qu'elle est suffisante pour votre cas d'utilisation
    static char local_state_description[100]; 

    // Verrouillez l'accès à la mémoire partagée
    sem_wait(&sem_memory);

    // Copiez la chaîne dans le buffer local
    strncpy(local_state_description, memory->case_officer.current_state->description, sizeof(local_state_description) - 1);

    // Assurez-vous que la chaîne est terminée par '\0'
    local_state_description[sizeof(local_state_description) - 1] = '\0';

    // Déverrouillez l'accès à la mémoire partagée
    sem_post(&sem_memory);

    // Retournez la copie locale
    return local_state_description;
}

void display_case_officer_information(WINDOW *window, memory_t *mem, int row, int column)
{
    /* --------------------------------------------------------------------- */
    /*           Get information from mem about the case officer             */

    // sem_wait(sem_spy_producer);
    case_officer_t * officer = &mem->case_officer;

    int id;
    int health_points;
    int location_row;
    int location_column;
    int home_row;
    int home_column;
    int mailbox_row;
    int mailbox_column;
    char state_description[30];

    id              = officer->id;
    health_points   = officer->health_point;
    location_row    = officer->location_column;
    location_column = officer->location_row;
    home_row        = officer->home_column;
    home_column     = officer->home_row;
    mailbox_row     = mem->map.mailbox_column;
    mailbox_column  = mem->map.mailbox_row;
    strcpy(state_description,officer->description);

   /* ---------------------------------------------------------------------- */

    wattron(window, A_BOLD);
    mvwprintw(window, row, column, "Case Officer");
    wattroff(window, A_BOLD);
    mvwprintw(window, row + 6, column, "%50s", "");
    mvwprintw(window, row + 1, column, "  Id: %d",id);
    mvwprintw(window, row + 2, column, "  Health: %d", health_points);
    mvwprintw(window, row + 3, column, "  Position: (%d,%d)", location_row, location_column);
    mvwprintw(window, row + 4, column, "  Home pos: (%d,%d)", home_row, home_column);
    mvwprintw(window, row + 5, column, "  Mailbox pos: (%d,%d)", mailbox_row, mailbox_column);
    mvwprintw(window, row + 6, column, "  State: %s", state_description);
    wrefresh(window);
    // printw("State of the officer : %s\n", state_description); // Pour le debug
    // wrefresh(window);
    // sem_post(sem_spy_consumer);
}


void display_counterintelligence_officer_information(WINDOW *window, memory_t *mem, int row, int col)
{
    /* --------------------------------------------------------------------- */
    /*    Get information from mem about the counterintelligence officer     */

    counter_intelligence_officer_t * officer = &mem->counter_intelligence_officer;

    int id;
    int health_points;
    int location_row;
    int location_column;
    int city_hall_row;
    int city_hall_column;
    int mailbox_row;
    int mailbox_column;
    int targeted_character_id;
    char state_description[40];

    id                    = officer->id;
    health_points         = officer->health_point;
    location_row          = officer->location_row;
    location_column       = officer->location_column;
    city_hall_row         = officer->city_hall_row;
    city_hall_column      = officer->city_hall_column;
    mailbox_row           = officer->mailbox_column;
    mailbox_column        = officer->mailbox_row;
    targeted_character_id = officer->targeted_character_id;
    strcpy(state_description, officer->description);
   /* ---------------------------------------------------------------------- */

	
    wattron(window, A_BOLD);
    mvwprintw(window, row, col, "Counterintelligence Officer");
    wattroff(window, A_BOLD);
    mvwprintw(window, row + 1, col, "  Id: %d", id);
    mvwprintw(window, row + 2, col, "  Health: %d", health_points);
    mvwprintw(window, row + 3, col, "  Position: (%d,%d)", location_row, location_column);
    mvwprintw(window, row + 4, col, "  City Hall pos: (%d,%d)", city_hall_row, city_hall_column);
    if (mailbox_row != -1) {
        mvwprintw(window, row + 5, col, "  Mailbox pos: (%d,%d)    ", mailbox_row, mailbox_column);
    } else {
        mvwprintw(window, row + 5, col, "  Mailbox pos: not found");
    }
    mvwprintw(window, row + 6, col, "  Target:     ");
    mvwprintw(window, row + 7, col, "%50s", "");
    mvwprintw(window, row + 7, col, "  State: %s", state_description);

    wrefresh(window);
}

void display_character_information(WINDOW *window, memory_t *mem)
{
    int title_column;
    int first_column;
    int second_column;
    char *title = "CHARACTERS";

    title_column = window->_maxx / 2 - strlen(title) / 2;
    first_column = 2;
    second_column = window->_maxx / 2;

    wattron(window, A_BOLD | A_UNDERLINE);
    mvwprintw(window, 1, title_column, "%s", title);
    wattroff(window, A_BOLD | A_UNDERLINE);
    display_case_officer_information(window, mem, 3, first_column);
    display_spy_information(window, mem, 3, second_column, 0);
    display_spy_information(window, mem, NB_OF_INFORMATION_FIELDS + 4, first_column, 1);
    display_spy_information(window, mem, NB_OF_INFORMATION_FIELDS + 4, second_column, 2);
	display_counterintelligence_officer_information(window, mem, NB_OF_INFORMATION_FIELDS * 2 + 5, first_column);
    display_citizen_information(window, mem, NB_OF_INFORMATION_FIELDS * 2 + 5, second_column);
    wrefresh(window);
}

void display_mailbox_content(WINDOW *window, memory_t *mem)
{
    werase(window); // Effacer la fenêtre à chaque appel

    int mailbox_nb_of_msgs = mem->homes->mailbox.message_count;

    // Afficher le titre
    int title_column = window->_maxx / 2 - strlen("MAILBOX CONTENT") / 2;
    wattron(window, A_BOLD | A_UNDERLINE);
    mvwprintw(window, 1, title_column, "MAILBOX CONTENT");
    wattroff(window, A_BOLD | A_UNDERLINE);

    // Si la boîte aux lettres est vide, afficher un message de notification et quitter
    if (mailbox_nb_of_msgs == 0) {
        mvwprintw(window, 3, 2, "The mailbox is empty.");
        wrefresh(window);
        return;
    }

    // Afficher les messages
    int nb_lines = 3;
    for (int i = 0; i < mailbox_nb_of_msgs && i < 5; ++i) { // Limite à 5 messages
        unsigned int priority = mem->homes->mailbox.priority[i];
        char content[MAX_MESSAGES];
        strcpy(content, mem->homes->mailbox.messages[i]);

        clear_line(window, nb_lines);

        // Afficher le message
        if (strcmp(content, "Ghfhswlyh") == 0) {
            mvwprintw(window, nb_lines, 2, ">> [%d] FAKE NEWS (P%d)", i + 1, priority);
        } else {
            mvwprintw(window, nb_lines, 2, ">> [%d] %s (P%d)", i + 1, content, priority);
        }

        nb_lines += 1;
    }

    wrefresh(window);
}

void display_enemy_country_monitor(WINDOW *window, memory_t *mem) {
    int nb_lines;
    int title_column;
    char *title = "ENEMY COUNTRY MONITOR";

    // Afficher le titre
    title_column = window->_maxx / 2 - strlen(title) / 2;
    wattron(window, A_BOLD | A_UNDERLINE);
    mvwprintw(window, 1, title_column, "%s", title);
    wattroff(window, A_BOLD | A_UNDERLINE);

    // Effacer seulement la partie des messages
    for (int i = 3; i < 12; ++i) {
        wmove(window, i, 0);
        wclrtoeol(window);
    }

    // Déterminer l'index de départ et l'ID de départ pour l'affichage des messages
    int start_index = 0;
    int start_id = 1;
    if (mem->message_count > 9) {
        start_index = mem->message_count - 9; // Commencer à partir du 2ème message si plus de 9 messages
        start_id = start_index + 1;
    }

    // Affichage des messages avec ID bien itérés
    nb_lines = 3;
    for (int i = start_index; i < mem->message_count && i < start_index + 9; ++i) {
        if (strlen(mem->messages[i]) > 0) { // Vérifier que le message n'est pas vide
            mvwprintw(window, nb_lines++, 2, "Message %d: %s", start_id++, mem->messages[i]);
        }
    }

    wrefresh(window);
}

void update_values(memory_t *mem) {
    
    int rows, columns;
    getmaxyx(city_window, rows, columns);

    display_general_information_values(city_window, mem);
    display_city(city_window, mem->map, rows, columns, mem);
    display_character_information(character_window, mem);
    display_mailbox_content(mailbox_content_window, mem);
    display_enemy_country_monitor(enemy_country_monitor, mem);

}

