/**
 * \file astar.c
 * \brief Implementation of the A* algorithm.
 * 
 * Implementation of the A* algorithm for finding the shortest path between two points on a map
 * and returning the next node to reach in the case of random movement.
 * 
 * \author Belghomari Abdelmalek 
 * @email: abdelmalek.belghomari@ecole.ensicaen.fr
 */

#include "astar.h"
#include <stdlib.h>
#include <math.h>


int DIRECTIONS[NUM_DIRECTIONS][2] = {{-1, 0},
                                     {1,  0},
                                     {0,  -1},
                                     {0,  1},
                                     {-1, -1},
                                     {-1, 1},
                                     {1,  -1},
                                     {1,  1}};

Node *create_node(int row, int column, double g, double h) {
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        // Gestion des erreurs de l'allocation mémoire
        perror("Unable to allocate memory for new node");
        exit(EXIT_FAILURE);
    }

    new_node->position[0] = row;
    new_node->position[1] = column;
    new_node->g = g;
    new_node->h = h;
    new_node->f = g + h;
    new_node->parent = NULL;

    return new_node;
}

heap_t *create_heap(int initial_capacity) {
    heap_t *heap = (heap_t *)malloc(sizeof(heap_t));
    if (!heap) {
        return NULL;
    }

    heap->nodes = (Node **)malloc(sizeof(Node *) * initial_capacity);
    if (!heap->nodes) {
        free(heap);
        return NULL;
    }

    heap->capacity = initial_capacity;
    heap->size = 0;

    return heap;
}

void destroy_heap(heap_t *heap) {
    if (heap) {
        for (int i = 0; i < heap->size; i++) {
            free(heap->nodes[i]);
        }
        free(heap->nodes);
        free(heap);
    }
}

bool insert_heap(heap_t *heap, Node *node) {
    if (is_heap_full(heap)) {
        return false;
    }

    heap->nodes[heap->size] = node;
    int current = heap->size++;
    while (current > 0 && heap->nodes[current]->f < heap->nodes[(current - 1) / 2]->f) {
        // Échanger avec le parent
        Node *temp = heap->nodes[(current - 1) / 2];
        heap->nodes[(current - 1) / 2] = heap->nodes[current];
        heap->nodes[current] = temp;
        current = (current - 1) / 2;
    }

    return true;
}

bool delete_root(heap_t *heap, Node **deleted_node) {
    if (is_heap_empty(heap)) return false;

    *deleted_node = heap->nodes[0];
    heap->nodes[0] = heap->nodes[--heap->size];

    // Réorganiser le tas
    int current = 0;
    while (current * 2 + 1 < heap->size) {
        int child;
        if (current * 2 + 2 < heap->size && heap->nodes[current * 2 + 2]->f < heap->nodes[current * 2 + 1]->f) {
            child = current * 2 + 2;
        } else {
            child = current * 2 + 1;
        }

        if (heap->nodes[current]->f > heap->nodes[child]->f) {
            Node *temp = heap->nodes[current];
            heap->nodes[current] = heap->nodes[child];
            heap->nodes[child] = temp;
            current = child;
        } else {
            break;
        }
    }

    return true;
}

bool is_in_heap(heap_t *heap, Node *node) {
    for (int i = 0; i < heap->size; i++) {
        if (heap->nodes[i]->position[0] == node->position[0] && heap->nodes[i]->position[1] == node->position[1]) {
            return true;
        }
    }
    return false;
}


bool is_heap_empty(const heap_t *heap) {
    return heap->size == 0;
}

bool is_heap_full(const heap_t *heap) {
    return heap->size == heap->capacity;
}

void print_heap(const heap_t *heap) {
    for (int i = 0; i < heap->size; i++) {
        printf("(%d, %d): f=%f\n", heap->nodes[i]->position[0], heap->nodes[i]->position[1], heap->nodes[i]->f);
    }
}

// Calcul de l'heuristique - Distance de Manhattan
double heuristic(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

bool is_goal(const Node *node, int goal_row, int goal_column) {
    if (node == NULL) {
        return false;
    }
    return (node->position[0] == goal_row && node->position[1] == goal_column);

}

bool is_cell_full(map_t *map, int row, int column) {
    cell_t cell = map->cells[row][column];
    return cell.current_capacity >= cell.nb_of_characters;
}

Node **get_successors(map_t *map, Node *current, int goal_row, int goal_column) {
    Node **neighbors = (Node **)malloc(sizeof(Node *) * (NUM_DIRECTIONS + 1)); // +1 pour NULL
    if (neighbors == NULL) {
        perror("Unable to allocate memory for neighbors");
        exit(EXIT_FAILURE);
    }

    int number_of_neighbors = 0;
    for (int i = 0; i < NUM_DIRECTIONS; i++) {
    int row = current->position[0] + DIRECTIONS[i][0];
    int column = current->position[1] + DIRECTIONS[i][1]; 

    if (row < 0 || row >= MAX_ROWS || column < 0 || column >= MAX_COLUMNS /*|| is_cell_full(map, row, row)*/ ) {
        continue;
    }

    if (map->cells[row][column].type != WASTELAND && (row != goal_row || column != goal_column)) {
        continue;
    }

    double g = current->g + 1;
    double h = heuristic(row, column, goal_row, goal_column);
    neighbors[number_of_neighbors++] = create_node(row, column, g, h);
    }

    neighbors[number_of_neighbors] = NULL; 
    return neighbors;
}

Node* get_random_neighbours_spy(map_t* map, spy_t* spy) {
    Node **neighbors = (Node **)malloc(sizeof(Node *) * NUM_DIRECTIONS);
    if (neighbors == NULL) {
        perror("Unable to allocate memory for neighbors");
        exit(EXIT_FAILURE);
    }

    int number_of_neighbors = 0;
    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        int row = spy->location_row + DIRECTIONS[i][0];
        int column = spy->location_column + DIRECTIONS[i][1];

        if (row < 0 || row >= MAX_ROWS || column < 0 || column >= MAX_COLUMNS) {
            continue;
        }
        
        // Ajouter des voisins si c'est du type WASTELAND
        if (map->cells[row][column].type != WASTELAND) {
            continue;
        }
        else {
            neighbors[number_of_neighbors++] = create_node(row, column, 0, 0);
        }
    }

    if (number_of_neighbors == 0) {
        free(neighbors);
        return NULL;
    }

    int random_neighbour = rand() % number_of_neighbors;
    Node *selected_neighbor = neighbors[random_neighbour];

    for (int i = 0; i < number_of_neighbors; i++) {
        if (i != random_neighbour) {
            free(neighbors[i]);
        }
    }

    return selected_neighbor;
}

Node* get_random_neighbours(map_t* map, int row, int column){
    Node **neighbors = (Node **)malloc(sizeof(Node *) * NUM_DIRECTIONS);
    if (neighbors == NULL) {
        perror("Unable to allocate memory for neighbors");
        exit(EXIT_FAILURE);
    }

    int number_of_neighbors = 0;
    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        int random_row = row + DIRECTIONS[i][0];
        int random_column = column + DIRECTIONS[i][1];

        if (row < 0 || row >= MAX_ROWS || column < 0 || column >= MAX_COLUMNS) {
            continue; // Ignorer les voisins non valides
        }
        
        if (map->cells[row][column].type != WASTELAND) {
            continue;
        }
        else {
            neighbors[number_of_neighbors++] = create_node(row, column, 0, 0);
        }
    }

    if (number_of_neighbors == 0) {
        free(neighbors);
        return NULL;
    }

    int random_neighbour = rand() % number_of_neighbors;
    Node *selected_neighbor = neighbors[random_neighbour];

    for (int i = 0; i < number_of_neighbors; i++) {
        if (i != random_neighbour) {
            free(neighbors[i]);
        }
    }


    return selected_neighbor;
}



Node *astar_search(map_t *map, int start_row, int start_column, int goal_row, int goal_column) {
    // Initialiser les structures de données nécessaires
    heap_t *open_set = create_heap(100); // Capacité initiale
    bool closed_set[MAX_ROWS][MAX_COLUMNS] = {false};

    // Ajouter le nœud de départ au tas
    Node *start_node = create_node(start_row, start_column, 0, heuristic(start_row, start_column, goal_row, goal_column));
    start_node->parent = NULL;
    insert_heap(open_set, start_node);

    while (!is_heap_empty(open_set)) {
        // Récupérer le nœud avec le plus bas coût F
        Node *current_node;
        delete_root(open_set, &current_node);

        // Si le nœud est la destination, le retourner
        if (is_goal(current_node, goal_row, goal_column)) {
            destroy_heap(open_set);
            return current_node;
        }

        // Marquer le nœud comme exploré
        closed_set[current_node->position[0]][current_node->position[1]] = true;

        // Gérer les voisins du nœud actuel
        Node **neighbors = get_successors(map, current_node, goal_row, goal_column);
        bool neighbors_to_free[NUM_DIRECTIONS] = {false};
        for (int i = 0; neighbors[i] != NULL; i++) {
            Node *neighbor = neighbors[i];

            if (neighbor == NULL) {
                printf("Neighbor is NULL\n");
                continue;
            }
            
            if (neighbor->position[0] < 0 || neighbor->position[0] >= MAX_ROWS ||
                neighbor->position[1] < 0 || neighbor->position[1] >= MAX_COLUMNS) {
                free(neighbor); 
                continue;
            }
            if (closed_set[neighbor->position[0]][neighbor->position[1]]) {
                free(neighbor);
                continue;
            }

            double tentative_g = current_node->g + 1 ; // Coût de mouvement = 1

            if (!is_in_heap(open_set, neighbor) || tentative_g < neighbor->g) {
                neighbor->parent = current_node;
                neighbor->g = tentative_g;
                neighbor->h = heuristic(neighbor->position[0], neighbor->position[1], goal_row, goal_column);
                neighbor->f = neighbor->g + neighbor->h;

                if (!is_in_heap(open_set, neighbor)) {
                    insert_heap(open_set, neighbor);
                }
            }        
        }
        for (int i = 0; i < NUM_DIRECTIONS; i++) {
            if (neighbors_to_free[i]) {
                free(neighbors[i]);
            }
        }

        free(neighbors);
    }

    destroy_heap(open_set);
    return NULL;
}


// Fonction pour reconstruire le chemin une fois la destination atteinte
Path *reconstruct_path(Node *goal_node) {
    int path_length = 0;
    Node *current = goal_node;
    Path *path = NULL;

    while (current != NULL) {
        path_length++;
        current = current->parent;
    }

    path = malloc(sizeof(Path));
    if (path == NULL) {
        perror("Unable to allocate memory for path");
        exit(EXIT_FAILURE);
    }

    path->nodes = malloc(sizeof(Node **) * path_length);
    if (path->nodes == NULL) {
        perror("Unable to allocate memory for path nodes");
        free(path);
        exit(EXIT_FAILURE);
    }

    path->length = path_length;

    current = goal_node;
    for (int i = path_length - 1; i >= 0; i--) {
        if (current == NULL) {
            fprintf(stderr, "Erreur: un parent de nœud est NULL dans reconstruct_path\n");
            free(path->nodes);
            free(path);
            return NULL;
        }
        path->nodes[i] = current;
        current = current->parent;
    }

    return path;
}

void print_path(Node **path, int path_length) {
    if (path == NULL || path_length <= 0) {
        printf("Chemin vide ou non défini\n");
        return;
    }

    printf("Chemin : ");
    for (int i = 0; i < path_length; i++) {
        if (path[i] != NULL) {
            printf("(%d, %d)", path[i]->position[0], path[i]->position[1]);
            if (i < path_length - 1) {
                printf(" -> ");
            }
        }
    }
    printf("\n");
}

Node *calculate_next_step(int current_row, int current_column, int goal_row, int goal_column, map_t *map) {
    // Vérifier si la position actuelle est déjà la destination
    if (current_row == goal_row && current_column == goal_column) {
        return NULL; // Aucun mouvement nécessaire
    }

    Node *goal_node = astar_search(map, current_row, current_column, goal_row, goal_column);

    if (goal_node == NULL) {
        return NULL;
    }

    // Revenir en arrière depuis le nœud de destination pour trouver la prochaine étape
    Node *next_step = goal_node;
    while (next_step->parent != NULL && next_step->parent->parent != NULL) {
        next_step = next_step->parent;
    }
    int h = next_step->h;
    int g = next_step->g;
    Node *next_step_node = create_node(next_step->position[0], next_step->position[1], h, g);

    Node *current = goal_node;
    while (current != NULL) {
        Node *temp = current;
        current = current->parent;
        free(temp);
    }

    return next_step_node;
}
