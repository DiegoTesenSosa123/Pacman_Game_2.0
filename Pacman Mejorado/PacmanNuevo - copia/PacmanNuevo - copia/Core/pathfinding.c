#include "pathfinding.h" 
#include "types.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// =========================================================
// 1. VALIDACIÓN DE CASILLA
// Verifica si una coordenada (fila, columna) es transitable.
// Retorna 'false' si es una pared ('#') o si está fuera del mapa.
// =========================================================
bool IsValid(Vector2Int pos, char maze[MAZE_HEIGHT][MAZE_WIDTH]) {
    if (pos.row < 0 || pos.col < 0 || pos.row >= MAZE_HEIGHT || pos.col >= MAZE_WIDTH)
        return false; // Fuera de límites
    return maze[pos.row][pos.col] != '#'; // Es válido si NO es pared
}

// =========================================================
// 2. BUSCADOR DE RUTAS (Algoritmo BFS)
// Encuentra el camino más corto desde 'start' hasta 'target'.
// Retorna un array dinámico con los pasos a seguir.
// =========================================================
Vector2Int* Pathfinding_Find(Vector2Int start, Vector2Int target, int* pathLength, char maze[MAZE_HEIGHT][MAZE_WIDTH]) {

    // Cola para el BFS (almacena casillas por visitar)
    Vector2Int* queue = (Vector2Int*)malloc(sizeof(Vector2Int) * MAZE_HEIGHT * MAZE_WIDTH);

    // Matriz para recordar qué casillas ya visitamos (evita bucles infinitos)
    bool visited[MAZE_HEIGHT][MAZE_WIDTH];
    memset(visited, false, sizeof(visited));

    // Matriz "Padre": Guarda de dónde venimos para reconstruir el camino después
    // Ejemplo: parent[3][4] = {3, 3} significa que llegamos a la casilla [3][4] desde la [3][3]
    Vector2Int parent[MAZE_HEIGHT][MAZE_WIDTH];
    for (int i = 0; i < MAZE_HEIGHT; i++)
        for (int j = 0; j < MAZE_WIDTH; j++)
            parent[i][j] = (Vector2Int){ -1, -1 };

    int front = 0, rear = 0;

    // 1. Empezamos agregando la posición inicial a la cola
    queue[rear++] = start;
    visited[start.row][start.col] = true;
    bool found = false;

    // 2. Bucle principal: Mientras haya casillas en la cola...
    while (front < rear) {
        Vector2Int current = queue[front++]; // Sacamos la siguiente casilla

        // ¿Llegamos al destino?
        if (current.row == target.row && current.col == target.col) {
            found = true;
            break; // ¡Camino encontrado! Terminamos la búsqueda.
        }

        // Direcciones posibles: Derecha, Izquierda, Abajo, Arriba
        Vector2Int directions[] = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };

        // Explorar vecinos
        for (int i = 0; i < 4; i++) {
            Vector2Int next = { current.row + directions[i].row, current.col + directions[i].col };

            // Si el vecino es válido y no lo hemos visitado aún...
            if (IsValid(next, maze) && !visited[next.row][next.col]) {
                queue[rear++] = next;                 // Lo metemos a la cola
                visited[next.row][next.col] = true;   // Lo marcamos visitado
                parent[next.row][next.col] = current; // Guardamos quién es su "padre" (de dónde vinimos)
            }
        }
    }

    free(queue); // Liberamos la memoria de la cola temporal

    // Si no se encontró camino (ej. destino inalcanzable)
    if (!found) {
        *pathLength = 0;
        return NULL;
    }

    // 3. Reconstrucción del camino (Backtracking)
    // Vamos desde el Target hacia atrás hasta el Start usando la matriz 'parent'

    // Primero: Contamos cuántos pasos son
    int length = 0;
    Vector2Int curr = target;
    while (!(curr.row == start.row && curr.col == start.col)) {
        length++;
        curr = parent[curr.row][curr.col];
    }
    length++; // Incluimos el paso final

    // Segundo: Creamos el array final del tamaño exacto
    Vector2Int* path = (Vector2Int*)malloc(sizeof(Vector2Int) * length);
    *pathLength = length;

    // Tercero: Llenamos el array (al revés para que quede Start -> Target)
    curr = target;
    for (int i = 0; i < length; i++) {
        path[length - 1 - i] = curr; // Guardamos de atrás para adelante
        if (!(curr.row == start.row && curr.col == start.col))
            curr = parent[curr.row][curr.col];
    }

    return path; // Retornamos el camino calculado
}