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
bool insert_heap(heap_t *heap, Node *node);
bool delete_root(heap_t *heap, Node **deleted_node);
bool is_heap_empty(const heap_t *heap);
bool is_heap_full(const heap_t *heap);
void print_heap(const heap_t *heap);

bool is_goal(const Node *node, int goal_x, int goal_y);
bool is_cell_full(map_t *map, int row, int column);
Node **get_successors(map_t *map, Node*current, int goal_x, int goal_y);
Node *astar_search(map_t *map, int start_x, int start_y, int goal_x, int goal_y);
Path *reconstruct_path(Node *goal_node);
void print_path(Node **path, int path_length);
Node *calculate_next_step(int current_x, int current_y, int goal_x, int goal_y, map_t *map);
Node *get_random_neighbours_spy(map_t* map, spy_t* spy);

#endif // ASTAR_H
