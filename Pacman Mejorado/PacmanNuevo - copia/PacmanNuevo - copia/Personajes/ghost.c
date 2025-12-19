#include "ghost.h"
#include "pathfinding.h"
#include <stdlib.h> 
#include <string.h> 
#include <math.h> 
#include <stdbool.h> 

Ghost Ghost_Init(GhostType type, Texture2D texNormal, Texture2D texFright, Texture2D texFrightWhite, Texture2D texEyes) {
    Ghost g;
    g.type = type;
    g.textureNormal = texNormal;
    g.textureFrightened = texFright;
    g.textureFrightenedWhite = texFrightWhite;
    g.textureEyes = texEyes;

    g.baseSpeed = 12;
    g.currentSpeed = g.baseSpeed;
    g.moveCounter = 0;
    g.path = NULL;
    g.pathLength = 0;
    g.pathIndex = 0;
    g.state = GHOST_STATE_NORMAL;
    g.stateTimer = 0;
    g.frozenSpeedTimer = 0;

    if (type == BLINKY) g.pos = (Vector2Int){ 14, 11 };
    else if (type == PINKY)  g.pos = (Vector2Int){ 13, 13 };
    else if (type == INKY)   g.pos = (Vector2Int){ 15, 13 };
    else if (type == CLYDE)  g.pos = (Vector2Int){ 14, 14 };

    g.target = g.pos;
    return g;
}

void Ghost_SetFrightened(Ghost* g) {
    if (g->state == GHOST_STATE_EATEN || g->state == GHOST_STATE_WAITING) return;
    g->state = GHOST_STATE_FRIGHTENED;
    g->stateTimer = 600;
}

void Ghost_SetFrozen(Ghost* g) {
    if (g->state == GHOST_STATE_EATEN || g->state == GHOST_STATE_WAITING) return;
    g->frozenSpeedTimer = 300;
}

void Ghost_SetEaten(Ghost* g) {
    g->state = GHOST_STATE_EATEN;
    g->currentSpeed = 8;
    g->pathIndex = 0;
    if (g->path) free(g->path);
    g->path = NULL;
}

void Ghost_Update(Ghost* g, Vector2Int pacmanPos, char maze[MAZE_HEIGHT][MAZE_WIDTH]) {

    if (g->state == GHOST_STATE_FRIGHTENED) {
        g->stateTimer--;
        if (g->stateTimer <= 0) g->state = GHOST_STATE_NORMAL;
    }

    if (g->frozenSpeedTimer > 0) {
        g->frozenSpeedTimer--;
        g->currentSpeed = 40;
    }
    else {
        g->currentSpeed = g->baseSpeed;
        if (g->state == GHOST_STATE_FRIGHTENED) g->currentSpeed = 20;
        if (g->state == GHOST_STATE_EATEN) g->currentSpeed = 8;
    }

    if (g->state == GHOST_STATE_WAITING) {
        g->stateTimer--;
        if (g->stateTimer <= 0) {
            g->state = GHOST_STATE_NORMAL;
            g->currentSpeed = g->baseSpeed;
            g->pos = (Vector2Int){ 14, 11 };
            if (g->path) free(g->path); g->path = NULL; g->pathIndex = 0;
        }
        return;
    }

    if (g->state == GHOST_STATE_EATEN) {
        g->target = (Vector2Int){ 14, 11 };
        if (g->pos.row == 14 && g->pos.col == 11) {
            g->pos = (Vector2Int){ 14, 14 };
            g->state = GHOST_STATE_WAITING;
            g->stateTimer = 120;
            if (g->path) free(g->path); g->path = NULL;
            return;
        }
    }
    else if (g->state == GHOST_STATE_FRIGHTENED) { g->target = (Vector2Int){ 1, 1 }; }
    else {
        if (g->type == BLINKY) g->target = pacmanPos;
        else if (g->type == PINKY) { g->target = pacmanPos; if (pacmanPos.col > 4) g->target.col -= 4; }
        else if (g->type == CLYDE) {
            int dist = abs(g->pos.row - pacmanPos.row) + abs(g->pos.col - pacmanPos.col);
            if (dist > 8) g->target = pacmanPos;
            else g->target = (Vector2Int){ MAZE_HEIGHT - 2, MAZE_WIDTH - 2 };
        }
        else g->target = pacmanPos;
    }

    if (g->path == NULL || g->pathIndex >= g->pathLength) {
        if (g->path != NULL) free(g->path);
        g->path = Pathfinding_Find(g->pos, g->target, &g->pathLength, maze);
        g->pathIndex = 0;
    }

    g->moveCounter++;
    if (g->moveCounter >= g->currentSpeed) {
        if (g->path != NULL && g->pathIndex < g->pathLength) {
            g->pos = g->path[g->pathIndex];
            g->pathIndex++;
        }
        g->moveCounter = 0;
    }
}

void Ghost_Draw(Ghost* g) {
    Texture2D texToDraw = g->textureNormal;
    Color tint = WHITE;

    if (g->state == GHOST_STATE_WAITING) {
        tint = Fade(WHITE, 0.0f);
    }
    else if (g->state == GHOST_STATE_EATEN) {
        texToDraw = g->textureEyes;
    }

    else if (g->state == GHOST_STATE_FRIGHTENED) {
        if (g->stateTimer < 180 && (g->stateTimer / 10) % 2 == 0) { 
            texToDraw = g->textureFrightenedWhite; 
        }
        else {
            texToDraw = g->textureFrightened; 
        }
    }

    else {
        texToDraw = g->textureNormal;
    }

    if (g->frozenSpeedTimer > 0) {
        tint = SKYBLUE;
    }

    DrawTexture(texToDraw, g->pos.col * 20, g->pos.row * 20, tint);
}