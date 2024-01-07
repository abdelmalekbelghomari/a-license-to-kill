/**
 * \file astar.h
 *
 * Defines structures and functions for the A* algorithm and heap management.
 * 
 * \author Belghomari Abdelmalek 
 * @email: abdelmalek.belghomari@ecole.ensicaen.fr
 */

#ifndef ASTAR_H
#define ASTAR_H

#include <stdbool.h>
#include "memory.h"

/**
 * \struct heap_t
 * \brief A structure representing a heap for A* algorithm.
 */
typedef struct {
    Node **nodes;       /*!< Dynamic array to store elements. */
    int capacity;       /*!< Current capacity of the heap. */
    int size;           /*!< Current number of elements in the heap. */
} heap_t;

/**
 * \brief Creates a new node for A* algorithm.
 *
 * Creates a new node with the specified coordinates, cost, and heuristic value.
 *
 * \param row The row coordinate of the node.
 * \param column The column coordinate of the node.
 * \param g The cost from the start to the node.
 * \param h The heuristic value from the node to the goal.
 * \return A pointer to the newly created node.
 */
Node *create_node(int row, int column, double g, double h);

/**
 * \brief Calculates the heuristic value between two points.
 *
 * Calculates the heuristic value (estimated cost) between two points using the Manhattan distance.
 *
 * \param row1 The row coordinate of the first point.
 * \param column1 The column coordinate of the first point.
 * \param row2 The row coordinate of the second point.
 * \param column2 The column coordinate of the second point.
 * \return The heuristic value between the two points.
 */
double heuristic(int row1, int column1, int row2, int column2);

/**
 * \brief Creates a new heap for A* algorithm.
 *
 * Creates a new heap with the specified initial capacity.
 *
 * \param initial_capacity The initial capacity of the heap.
 * \return A pointer to the newly created heap.
 */
heap_t *create_heap(int initial_capacity);

/**
 * \brief Destroys a heap.
 *
 * Frees the memory associated with a heap.
 *
 * \param heap A pointer to the heap to be destroyed.
 */
void destroy_heap(heap_t *heap);

/**
 * \brief Inserts a node into the heap.
 *
 * Inserts a node into the heap according to its priority (f-value).
 *
 * \param heap A pointer to the heap.
 * \param node A pointer to the node to be inserted.
 * \return `true` if the insertion is successful, `false` otherwise.
 */
bool insert_heap(heap_t *heap, Node *node);

/**
 * \brief Deletes the root node from the heap.
 *
 * Deletes the root node (the one with the lowest priority) from the heap.
 *
 * \param heap A pointer to the heap.
 * \param deleted_node A pointer to store the deleted node.
 * \return `true` if the deletion is successful, `false` otherwise.
 */
bool delete_root(heap_t *heap, Node **deleted_node);

/**
 * \brief Checks if the heap is empty.
 *
 * Checks if the heap is empty (contains no nodes).
 *
 * \param heap A pointer to the heap.
 * \return `true` if the heap is empty, `false` otherwise.
 */
bool is_heap_empty(const heap_t *heap);

/**
 * \brief Checks if the heap is full.
 *
 * Checks if the heap is full (reached its capacity).
 *
 * \param heap A pointer to the heap.
 * \return `true` if the heap is full, `false` otherwise.
 */
bool is_heap_full(const heap_t *heap);

/**
 * \brief Prints the contents of the heap.
 *
 * Prints the contents of the heap (for debugging purposes).
 *
 * \param heap A pointer to the heap.
 */
void print_heap(const heap_t *heap);

/**
 * \brief Checks if a node is the goal.
 *
 * Checks if a given node is the goal node with the specified coordinates.
 *
 * \param node A pointer to the node to be checked.
 * \param goal_row The row coordinate of the goal.
 * \param goal_column The column coordinate of the goal.
 * \return `true` if the node is the goal, `false` otherwise.
 */
bool is_goal(const Node *node, int goal_row, int goal_column);

/**
 * \brief Checks if a cell on the map is full.
 *
 * Checks if a cell on the map is occupied by characters.
 *
 * \param map A pointer to the map.
 * \param row The row of the cell.
 * \param column The column of the cell.
 * \return `true` if the cell is full, `false` otherwise.
 */
bool is_cell_full(map_t *map, int row, int column);

/**
 * \brief Gets the successors of a node in the A* algorithm.
 *
 * Gets the successor nodes of a given node on the map.
 *
 * \param map A pointer to the map.
 * \param current A pointer to the current node.
 * \param goal_row The row coordinate of the goal.
 * \param goal_column The column coordinate of the goal.
 * \return An array of successor nodes.
 */
Node **get_successors(map_t *map, Node *current, int goal_row, int goal_column);

/**
 * \brief Performs A* search on the map.
 *
 * Performs the A* search algorithm on the map to find the path from the start to the goal.
 *
 * \param map A pointer to the map.
 * \param start_row The row coordinate of the starting point.
 * \param start_column The column coordinate of the starting point.
 * \param goal_row The row coordinate of the goal.
 * \param goal_column The column coordinate of the goal.
 * \return A pointer to the goal node if a path is found, NULL otherwise.
 */
Node *astar_search(map_t *map, int start_row, int start_column, int goal_row, int goal_column);

/**
 * \brief Reconstructs the path from the goal node.
 *
 * Reconstructs the path from the goal node to the start node.
 *
 * \param goal_node A pointer to the goal node.
 * \return An array of nodes representing the path.
 */
Path *reconstruct_path(Node *goal_node);

/**
 * \brief Prints the path for debugging.
 *
 * Prints the path (for debugging purposes).
 *
 * \param path An array of nodes representing the path.
 * \param path_length The length of the path.
 */
void print_path(Node **path, int path_length);

/**
 * \brief Calculates the next step using A* algorithm.
 *
 * Calculates the next step for a character using the A* algorithm.
 *
 * \param current_row The current row coordinate of the character.
 * \param current_column The current column coordinate of the character.
 * \param goal_row The row coordinate of the goal.
 * \param goal_column The column coordinate of the goal.
 * \param map A pointer to the map.
 * \return A pointer to the next node representing the next step.
 */
Node *calculate_next_step(int current_row, int current_column, int goal_row, int goal_column, map_t *map);

/**
 * \brief Gets random neighbors for a spy character.
 *
 * Gets random neighboring nodes for a spy character.
 *
 * \param map A pointer to the map.
 * \param spy A pointer to the spy character.
 * \return
 */
Node *get_random_neighbours_spy(map_t *map, spy_t *spy);

/**
 * \brief Gets random neighbors for a character.
 *
 * Gets random neighboring nodes for a character at the specified row and column.
 *
 * \param map A pointer to the map.
 * \param row The row coordinate of the character.
 * \param column The column coordinate of the character.
 * \return A pointer to a randomly selected neighboring node.
 */
Node *get_random_neighbours(map_t *map, int row, int column);

#endif // ASTAR_H
