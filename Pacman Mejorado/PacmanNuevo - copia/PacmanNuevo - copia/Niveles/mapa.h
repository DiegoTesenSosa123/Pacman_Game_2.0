#ifndef MAPA_H
#define MAPA_H

#include "types.h"

// Declaracion del mapa
extern const char initialmaze[MAZE_HEIGHT][MAZE_WIDTH];

// Dibuja el laberinto completo en pantalla
void Mapa_Draw(char maze[MAZE_HEIGHT][MAZE_WIDTH], Color wallColor);

// Verifica el estado de las bolitas
int Mapa_CountPellets(char maze[MAZE_HEIGHT][MAZE_WIDTH]);

#endif