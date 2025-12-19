#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "types.h"

Vector2Int* Pathfinding_Find(Vector2Int start, Vector2Int target, int* pathLength, char maze[MAZE_HEIGHT][MAZE_WIDTH]);

#endif