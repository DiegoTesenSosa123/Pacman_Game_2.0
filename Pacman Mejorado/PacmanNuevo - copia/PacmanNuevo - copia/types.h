#ifndef TYPES_H
#define TYPES_H

#include "raylib.h"

// =========================================================
// DIMENSIONES GLOBALES
// =========================================================
#define MAZE_WIDTH 28
#define MAZE_HEIGHT 31
#define TILE_SIZE 20

// --- ESTO ES LO QUE TE FALTA ---
// Resolución virtual del juego
#define GAME_SCREEN_WIDTH   (MAZE_WIDTH * TILE_SIZE)       
#define GAME_SCREEN_HEIGHT  (MAZE_HEIGHT * TILE_SIZE + 50) 
// ------------------------------

// =========================================================
// ENUMS (Listas de Opciones)
// =========================================================

// Movemos GameScreen aquí para que menu.h pueda usarlo
// En types.h (dentro del enum GameScreen)
typedef enum {
    SCREEN_TITLE,
    SCREEN_SETTINGS, 
    SCREEN_CLASS_SELECT,
    SCREEN_INSTRUCTIONS,
    SCREEN_HIGHSCORES,
    SCREEN_LOADING,
    SCREEN_READY,
    SCREEN_GAMEPLAY,
    SCREEN_PAUSE,
    SCREEN_PACMAN_DYING,
    SCREEN_RESULT,
    SCREEN_NAME_INPUT
} GameScreen;

typedef enum {
    CLASS_NORMAL,
    CLASS_TANK,
    CLASS_SPEED
} PacmanClass;

typedef enum {
    STOP, UP, DOWN, LEFT, RIGHT
} Direction;

typedef enum {
    GHOST_STATE_NORMAL,
    GHOST_STATE_FRIGHTENED,
    GHOST_STATE_FROZEN,
    GHOST_STATE_EATEN,
    GHOST_STATE_WAITING
} GhostState;

// Estructuras auxiliares
typedef struct {
    int row;
    int col;
} Vector2Int;

typedef struct {
    Vector2 pos;
    Vector2 velocity;
    bool active;
} Projectile;

#endif