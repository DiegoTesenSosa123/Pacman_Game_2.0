#ifndef MAPA_H
#define MAPA_H

#include "types.h"

// =========================================================
// DATOS GLOBALES DEL MAPA
// 'extern' significa: "El mapa existe en mapa.c, pero aquí
// te doy permiso de usarlo en otros archivos".
// =========================================================
extern const char initialmaze[MAZE_HEIGHT][MAZE_WIDTH];

// Dibuja el laberinto completo en pantalla
void Mapa_Draw(char maze[MAZE_HEIGHT][MAZE_WIDTH], Color wallColor);

// Retorna la cantidad de items restantes (para saber si ganaste)
int Mapa_CountPellets(char maze[MAZE_HEIGHT][MAZE_WIDTH]);

#endif