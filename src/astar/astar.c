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

Node *create_node(int x, int y, double g, double h) {
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        // Gestion des erreurs de l'allocation mémoire
        perror("Unable to allocate memory for new node");
        exit(EXIT_FAILURE);
    }

    new_node->position[0] = x;
    new_node->position[1] = y;
    new_node->g = g;
    new_node->h = h;
    new_node->f = g + h;
    new_node->parent = NULL; // Initialisation du parent à NULL

    return new_node;
}


// Créer un nouveau nœud
Node *create_node(int x, int y) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node) {
        new_node->position[0] = x;
        new_node->position[1] = y;
        new_node->g = 0.0;
        new_node->h = 0.0;
        new_node->f = 0.0;
        new_node->parent = NULL;
    }
    return new_node;
}

heap_t *create_heap(int initial_capacity) {
    heap_t *heap = (heap_t *)malloc(sizeof(heap_t));
    if (!heap) return NULL;

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
        // Vous pouvez choisir d'augmenter la capacité du tas ici
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
            // Échanger avec le plus petit enfant
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
    return fabs(x1 - x2) + fabs(y1 - y2);
}

bool is_goal(const Node *node, int goal_x, int goal_y) {
    if (node == NULL) {
        return false;
    }
    return (node->position[0] == goal_x && node->position[1] == goal_y);
}

bool is_cell_full(map_t *map, int row, int column) {
    cell_t cell = map->cells[column][row];
    return cell.current_capacity >= cell.nb_of_characters;
}

Node **get_successors(map_t *map, Node*current, int goal_x, int goal_y){
    
    Node **neighbors = (Node **)malloc(sizeof(Node *) * NUM_DIRECTIONS);
    if(neighbors == NULL){
        perror("Unable to allocate memory for neighbors");
        exit(EXIT_FAILURE);
    }
    int number_of_neighbors = 0;

    // Vérifier les 8 voisins
    for (int i = 0; i < NUM_DIRECTIONS ; i++) {
        int x = current->position[0] + DIRECTIONS[i][0];
        int y = current->position[1] + DIRECTIONS[i][1];

        // Vérifier si le voisin est dans les limites de la carte
        if (x < 0 || x >= MAX_ROWS || y < 0 || y >= MAX_COLUMNS) {
            continue;
        }

        // Vérifier si le voisin est plein
        if (is_cell_full(map, x, y)) {
            continue;
        }

        double g = current->g++; 
        double h = heuristic(x, y, goal_x, goal_y);

        neighbors[number_of_neighbors++] = create_node(x, y, g, h);
    }
    if (number_of_neighbors < 8) {
        neighbors = realloc(neighbors, number_of_neighbors * sizeof(Node*));
        if (neighbors == NULL) {
            perror("Unable to reallocate memory for neighbors");
            exit(EXIT_FAILURE);
        }
    }
    return neighbors;
}


Node *astar_search(map_t *map, int start_x, int start_y, int goal_x, int goal_y) {
    // Initialiser les structures de données nécessaires
    heap_t *open_set = create_heap(100); // Capacité initiale
    bool closed_set[MAX_ROWS][MAX_COLUMNS] = {false};

    // Ajouter le nœud de départ au tas
    Node *start_node = create_node(start_x, start_y, 0, heuristic(start_x, start_y, goal_x, goal_y));
    start_node->parent = NULL;
    insert_heap(open_set, start_node);

    while (!is_heap_empty(open_set)) {
        // Récupérer le nœud avec le plus bas coût F
        Node *current_node;
        delete_root(open_set, &current_node);

        // Si le nœud est la destination, construire et retourner le chemin
        if (is_goal(current_node, goal_x, goal_y)) {
            Node *path = reconstruct_path(current_node);
            destroy_heap(open_set);
            return path;
        }

        // Marquer le nœud comme exploré
        closed_set[current_node->position[0]][current_node->position[1]] = true;

        // Gérer les voisins du nœud actuel
        Node **neighbors = get_successors(map, current_node);
        for (int i = 0; neighbors[i] != NULL; i++) {
            Node *neighbor = neighbors[i];

            if (closed_set[neighbor->position[0]][neighbor->position[1]]) {
                free(neighbor);
                continue;
            }

            double tentative_g = current_node->g + 1 ; // Coût de mouvement = 1

            if (!is_in_heap(open_set, neighbor) || tentative_g < neighbor->g) {
                neighbor->parent = current_node;
                neighbor->g = tentative_g;
                neighbor->h = heuristic(neighbor->position[0], neighbor->position[1], goal_x, goal_y);
                neighbor->f = neighbor->g + neighbor->h;

                if (!is_in_heap(open_set, neighbor)) {
                    insert_heap(open_set, neighbor);
                }
            }
        }

        free(neighbors);
    }

    // Chemin non trouvé
    destroy_heap(open_set);
    return NULL;
}


// Fonction pour reconstruire le chemin une fois la destination atteinte
Node **reconstruct_path(Node *goal_node) {
    int path_length = 0;
    Node *current = goal_node;
    Node **path = NULL;

    // Compter le nombre de nœuds dans le chemin
    while (current != NULL) {
        path_length++;
        current = current->parent;
    }

    // Allocation de mémoire pour le chemin
    path = malloc(sizeof(Node *) * path_length);
    if (path == NULL) {
        perror("Unable to allocate memory for path");
        exit(EXIT_FAILURE);
    }

    // Reconstruire le chemin
    current = goal_node;
    for (int i = path_length - 1; i >= 0; i--) {
        path[i] = current;
        current = current->parent;
    }

    return path;
}


/* =========== COMMENT UTILISER ASTAR =========== */
// 1. Initialiser les chemins des cityens
// 2. Dans la boucle principale, pour chaque citoyen, appeler la fonction astar_search

// Exemple:

/*
void init_citizens(memory_t *memory) {
    // ...
    for (int i = 0; i < CITIZENS_COUNT; i++) {
        citizen_t *citizen = &memory->citizens[i];
        
        // Initialisation des autres attributs du citoyen...

        // Définition des points de départ et d'arrivée
        int start_x = citizen->home->position[0];
        int start_y = citizen->home->position[1];
        int goal_x_company = citizen->workplace->position[0];
        int goal_y_company = citizen->workplace->position[1];
        int goal_x_supermarket = citizen->supermarket->position[0];
        int goal_y_supermarket = citizen->supermarket->position[1];

        // A* de la maison à l'entreprise
        Node *end_node_company = astar_search(&memory->map, start_x, start_y, goal_x_company, goal_y_company);
        if (end_node_company != NULL) {
            citizen->path_to_work = reconstruct_path(end_node_company);
        }

        // A* de l'entreprise au supermarché
        Node *end_node_supermarket = astar_search(&memory->map, goal_x_company, goal_y_company, goal_x_supermarket, goal_y_supermarket);
        if (end_node_supermarket != NULL) {
            citizen->path_to_supermarket = reconstruct_path(end_node_supermarket);
        }

        // A* du supermarché à la maison (ou d'autres chemins nécessaires)
        // ...

        // Initialisation des états et autres attributs du citoyen
        // ...
    }
    // ...
}

et après faut juste modifier les steps dans le pattern state pour que chaque tick, ils donnent une nouvelle position 
au personnage.

Exemple de step pour le pattern state:

    if (citizen->current_state == citizen->going_to_company) {
        move_along_path(citizen, citizen->path_to_work);
        // Si le citoyen arrive à l'entreprise, changer l'état
        if (arrived_at_destination(citizen, citizen->workplace)) {
            change_state(citizen, citizen->working);
        }
    }
}


*/
