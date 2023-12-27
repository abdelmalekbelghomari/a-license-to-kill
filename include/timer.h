#ifndef TIMER_HH
#define TIMER_HH

#include "memory.h"

// Déclaration des fonctions qui seront utilisées pour la gestion du timer et des signaux.

// Configure et initialise les signaux nécessaires.
void set_signals();

// Initialise et configure un timer.
void set_timer(void);

// Réception d'un signal d'identifiant "signum".
void handler(int signum);

// Crée et initialise un nouveau timer.
time_s new_timer();

// Met à jour le temps du timer spécifié.
void update_time(time_s *t);

// Configure le timer pour envoyer des signaux périodiques.
void set_clock();

// Configure le gestionnaire de signal pour le timer.
void set_signal();

// Configure et accède à la mémoire partagée.
void access_memory();

#endif
