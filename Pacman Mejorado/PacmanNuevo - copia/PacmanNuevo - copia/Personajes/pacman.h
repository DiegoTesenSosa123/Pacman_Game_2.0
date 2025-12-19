#ifndef PACMAN_H
#define PACMAN_H

#include "types.h"

#define PACMAN_SOUND_MUNCH      "resources/Audio/munch_1.wav"

#define PACMAN_SKIN_NORMAL_1    "resources/sprites/Pacman/PacmanNormal/Pacman/PacmanNormal1.png"
#define PACMAN_SKIN_NORMAL_2    "resources/sprites/Pacman/PacmanNormal/Pacman/PacmanNormal2.png"

#define PACMAN_SKIN_FIRE_1      "resources/sprites/Pacman/PacmanFuego/Pacman/PacmanFuego1.png"
#define PACMAN_SKIN_FIRE_2      "resources/sprites/Pacman/PacmanFuego/Pacman/PacmanFuego2.png"

#define PACMAN_SKIN_ICE_1       "resources/sprites/Pacman/PacmanHielo/Pacman/PacmanHielo1.png"
#define PACMAN_SKIN_ICE_2       "resources/sprites/Pacman/PacmanHielo/Pacman/PacmanHielo2.png"

#define PACMAN_DEATH_NORMAL_FMT "resources/sprites/Pacman/PacmanNormal/deathAnim/death%d.png"
#define PACMAN_DEATH_FIRE_FMT   "resources/sprites/Pacman/PacmanFuego/deathAnim/death%d.png"
#define PACMAN_DEATH_ICE_FMT    "resources/sprites/Pacman/PacmanHielo/deathAnim/death%d.png"

typedef struct {
    Vector2 pos;
    Direction direction;
    Direction buffer_direction;

    PacmanClass type;
    float baseSpeed;
    float currentSpeed;
    bool hasArmor;

    bool canShootFire;
    bool canShootIce;
    int ammo;

    int ghosts_eaten_this_powerup;
    int lives;
    int score;
    bool isDead;

    bool isDying;
    int deathFrame;
    int deathTimer;

    Sound munchSound;

    Texture2D currentSprite;
    Texture2D spriteOpen;
    Texture2D spriteClosed;

    Texture2D* currentDeathAnim;

    Texture2D skinNormal[2];
    Texture2D skinFire[2];
    Texture2D skinIce[2];

    Texture2D deathNormal[11];
    Texture2D deathFire[11];
    Texture2D deathIce[11];

} Pacman;

Pacman Pacman_Init(PacmanClass pClass);
void Pacman_Update(Pacman* p, char maze[MAZE_HEIGHT][MAZE_WIDTH]);
void Pacman_Draw(Pacman* p);
bool Pacman_UpdateDeath(Pacman* p);

#endif