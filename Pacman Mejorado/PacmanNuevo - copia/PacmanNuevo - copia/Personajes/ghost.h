#ifndef GHOST_H
#define GHOST_H

#include "types.h"

#define GHOST_SPRITE_BLINKY     "resources/sprites/Fantasmas/blinky1.png"
#define GHOST_SPRITE_PINKY      "resources/sprites/Fantasmas/pinky1.png"
#define GHOST_SPRITE_INKY       "resources/sprites/Fantasmas/inky1.png"
#define GHOST_SPRITE_CLYDE      "resources/sprites/Fantasmas/clyde1.png"
#define GHOST_SPRITE_SCARED     "resources/sprites/Fantasmas/frightened.png"
#define GHOST_SPRITE_SCARED_W   "resources/sprites/Fantasmas/frightenedwhite.png"
#define GHOST_SPRITE_EYES       "resources/sprites/Fantasmas/eyes.png"

typedef enum {
    BLINKY, PINKY, INKY, CLYDE
} GhostType;

typedef struct {
    Vector2Int pos;
    Vector2Int target;
    GhostType type;

    Vector2Int* path;
    int pathLength;
    int pathIndex;
    int moveCounter;
    int baseSpeed;
    int currentSpeed;

    GhostState state;
    int stateTimer;
    int frozenSpeedTimer;

    Texture2D textureNormal;
    Texture2D textureFrightened;
    Texture2D textureFrightenedWhite;
    Texture2D textureEyes;
} Ghost;

Ghost Ghost_Init(GhostType type, Texture2D texNormal, Texture2D texFright, Texture2D texFrightWhite, Texture2D texEyes);

void Ghost_Update(Ghost* g, Vector2Int pacmanPos, char maze[MAZE_HEIGHT][MAZE_WIDTH]);
void Ghost_Draw(Ghost* g);
void Ghost_SetFrightened(Ghost* g);
void Ghost_SetFrozen(Ghost* g);
void Ghost_SetEaten(Ghost* g);

#endif