#ifndef ASTAR_H
#define ASTAR_H

#include <stdbool.h>
#include "memory.h"

#define NUM_DIRECTIONS 8


typedef struct {
    Node **nodes;       // Tableau dynamique pour stocker les éléments
    int capacity;       // Capacité actuelle du tas
    int size;           // Nombre actuel d'éléments dans le tas
} heap_t;

// Fonctions pour la gestion du tas
Node *create_node(int x, int y, double g, double h);
double heuristic(int x1, int y1, int x2, int y2);

heap_t *create_heap(int initial_capacity);
void destroy_heap(heap_t *heap);
bool insert_heap(heap_t *heap, Node item);
bool delete_root(heap_t *heap, Node *deleted_item);
bool is_heap_empty(const heap_t *heap);
bool is_heap_full(const heap_t *heap);
void print_heap(const heap_t *heap);
#endif // ASTAR_H
