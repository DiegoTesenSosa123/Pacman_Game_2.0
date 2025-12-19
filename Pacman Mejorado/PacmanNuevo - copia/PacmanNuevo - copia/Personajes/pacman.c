#define _CRT_SECURE_NO_WARNINGS
#include "pacman.h"
#include <math.h>
#include <stdlib.h> 
#include <stdio.h> 

// =========================================================
// 1. SISTEMA DE COLISIONES
// Verifica si una coordenada específica del mapa es una pared ('#')
// o si está fuera de los límites del mapa.
// =========================================================
bool IsWall(int gridX, int gridY, char maze[MAZE_HEIGHT][MAZE_WIDTH]) {
    if (gridX < 0 || gridX >= MAZE_WIDTH || gridY < 0 || gridY >= MAZE_HEIGHT) return true;
    return (maze[gridY][gridX] == '#');
}

// =========================================================
// 2. CONSTRUCTOR / INICIALIZACIÓN
// Configura al Pacman al inicio: carga texturas, sonidos, define
// su velocidad según la clase (Tanque, Veloz, Normal) y su posición.
// =========================================================
Pacman Pacman_Init(PacmanClass pClass) {
    Pacman p;
    // Posición inicial centrada en el tile
    p.pos = (Vector2){ (float)(MAZE_WIDTH * TILE_SIZE) / 2 + TILE_SIZE / 2, (float)((MAZE_HEIGHT * TILE_SIZE) / 2 + TILE_SIZE * 2) };
    p.direction = STOP;
    p.buffer_direction = STOP;
    p.lives = 3;
    p.score = 0;
    p.isDead = false;
    p.isDying = false;
    p.type = pClass; 
    p.canShootFire = false;
    p.canShootIce = false;
    p.ammo = 0;
    p.ghosts_eaten_this_powerup = 0;

    // Configuración de estadísticas según la clase elegida
    if (p.type == CLASS_TANK) { p.baseSpeed = 1.5f; p.hasArmor = true; }
    else if (p.type == CLASS_SPEED) { p.baseSpeed = 2.6f; p.hasArmor = false; }
    else { p.baseSpeed = 2.0f; p.hasArmor = false; } 
    p.currentSpeed = p.baseSpeed;

    // Carga de imágenes 
    p.skinNormal[0] = LoadTexture("resources/sprites/Pacman/PacmanNormal/Pacman/PacmanNormal1.png");
    p.skinNormal[1] = LoadTexture("resources/sprites/Pacman/PacmanNormal/Pacman/PacmanNormal2.png");
    p.skinFire[0] = LoadTexture("resources/sprites/Pacman/PacmanFuego/Pacman/PacmanFuego1.png");
    p.skinFire[1] = LoadTexture("resources/sprites/Pacman/PacmanFuego/Pacman/PacmanFuego2.png");
    p.skinIce[0] = LoadTexture("resources/sprites/Pacman/PacmanHielo/Pacman/PacmanHielo1.png");
    p.skinIce[1] = LoadTexture("resources/sprites/Pacman/PacmanHielo/Pacman/PacmanHielo2.png");

    // Carga de la animación de muerte 
    for (int i = 0; i < 11; i++) {
        char path[256];
        sprintf(path, "resources/sprites/Pacman/PacmanNormal/deathAnim/death%d.png", i + 1);
        p.deathNormal[i] = LoadTexture(path);
        sprintf(path, "resources/sprites/Pacman/PacmanFuego/deathAnim/death%d.png", i + 1);
        p.deathFire[i] = LoadTexture(path);
        sprintf(path, "resources/sprites/Pacman/PacmanHielo/deathAnim/death%d.png", i + 1);
        p.deathIce[i] = LoadTexture(path);
    }

    p.munchSound = LoadSound("resources/Audio/munch_1.wav");

    // Configuración visual inicial
    p.currentDeathAnim = p.deathNormal;
    p.spriteOpen = p.skinNormal[0];
    p.spriteClosed = p.skinNormal[1];
    p.currentSprite = p.spriteClosed;

    return p;
}

// =========================================================
// 3. ANIMACIÓN DE MUERTE
// Controla la secuencia cuadro por cuadro cuando Pacman muere.
// Devuelve 'true' cuando la animación termina.
// =========================================================
bool Pacman_UpdateDeath(Pacman* p) {
    if (!p->isDying) {
        // Inicio de la muerte: Reseteamos contadores y elegimos color de muerte
        p->isDying = true;
        p->deathFrame = 0;
        p->deathTimer = 0;

        if (p->canShootFire) p->currentDeathAnim = p->deathFire;
        else p->currentDeathAnim = p->deathNormal;
    }

    // Avance de la animación (velocidad controlada por timer)
    p->deathTimer++;
    if (p->deathTimer > 8) {
        p->deathFrame++;
        p->deathTimer = 0;
    }

    // Fin de la animación
    if (p->deathFrame >= 11) {
        p->isDying = false;
        return true;
    }
    return false;
}

// =========================================================
// 4. BUCLE PRINCIPAL (UPDATE)
// Maneja inputs, movimiento, físicas y colisión con items.
// =========================================================
void Pacman_Update(Pacman* p, char maze[MAZE_HEIGHT][MAZE_WIDTH]) {
    if (p->isDying) return; // Si está muriendo, no se mueve

    // Gestión de munición
    if (p->canShootFire && p->ammo <= 0) {
        p->canShootFire = false;
    }

    // --- CAMBIO DE SKIN (VISUAL) SEGÚN PODER ---
    if (p->type != CLASS_SPEED) { // El veloz no cambia de skin
        if (p->canShootFire) {
            p->spriteOpen = p->skinFire[0];
            p->spriteClosed = p->skinFire[1];
            p->currentDeathAnim = p->deathFire;
        }
        else {
            p->spriteOpen = p->skinNormal[0];
            p->spriteClosed = p->skinNormal[1];
            p->currentDeathAnim = p->deathNormal;
        }
    }
    else {
        p->spriteOpen = p->skinNormal[0];
        p->spriteClosed = p->skinNormal[1];
        p->currentDeathAnim = p->deathNormal;
    }

    // --- LECTURA DE TECLADO (INPUT) ---
    if (IsKeyDown(KEY_RIGHT)) p->buffer_direction = RIGHT;
    else if (IsKeyDown(KEY_LEFT)) p->buffer_direction = LEFT;
    else if (IsKeyDown(KEY_UP)) p->buffer_direction = UP;
    else if (IsKeyDown(KEY_DOWN)) p->buffer_direction = DOWN;

    // --- CÁLCULOS DE POSICIÓN EN LA GRILLA ---
    int gridX = (int)(p->pos.x / TILE_SIZE);
    int gridY = (int)(p->pos.y / TILE_SIZE);
    float centerX = gridX * TILE_SIZE + TILE_SIZE / 2.0f;
    float centerY = gridY * TILE_SIZE + TILE_SIZE / 2.0f;
    float distX = fabsf(p->pos.x - centerX);
    float distY = fabsf(p->pos.y - centerY);

    // Lógica para permitir girar solo si estamos cerca del centro del tile
    float turnThreshold = p->currentSpeed + 1.0f;
    bool atCenter = (distX <= turnThreshold && distY <= turnThreshold);

    // --- LÓGICA DE GIRO (BUFFER DIRECTION) ---
    // Permite girar si no hay pared o si es un cambio de sentido (ej. Derecha a Izquierda)
    if (p->buffer_direction != STOP && p->buffer_direction != p->direction) {
        int nextX = gridX, nextY = gridY;
        if (p->buffer_direction == RIGHT) nextX++;
        if (p->buffer_direction == LEFT)  nextX--;
        if (p->buffer_direction == UP)    nextY--;
        if (p->buffer_direction == DOWN)  nextY++;

        bool isOpposite = (p->direction == RIGHT && p->buffer_direction == LEFT) ||
            (p->direction == LEFT && p->buffer_direction == RIGHT) ||
            (p->direction == UP && p->buffer_direction == DOWN) ||
            (p->direction == DOWN && p->buffer_direction == UP);

        if ((atCenter && !IsWall(nextX, nextY, maze)) || isOpposite) {
            p->direction = p->buffer_direction;
            // Alineación perfecta al girar
            if (p->direction == UP || p->direction == DOWN) p->pos.x = centerX;
            if (p->direction == LEFT || p->direction == RIGHT) p->pos.y = centerY;
        }
    }

    // --- MOVIMIENTO REAL ---
    Vector2 nextPos = p->pos;
    if (p->direction == RIGHT) nextPos.x += p->currentSpeed;
    if (p->direction == LEFT)  nextPos.x -= p->currentSpeed;
    if (p->direction == UP)    nextPos.y -= p->currentSpeed;
    if (p->direction == DOWN)  nextPos.y += p->currentSpeed;

    // Predicción de colisión futura
    int frontX = (int)((nextPos.x + (p->direction == RIGHT ? 10 : (p->direction == LEFT ? -10 : 0))) / TILE_SIZE);
    int frontY = (int)((nextPos.y + (p->direction == DOWN ? 10 : (p->direction == UP ? -10 : 0))) / TILE_SIZE);

    if (IsWall(frontX, frontY, maze)) {
        // Si choca, se detiene y se alinea al centro
        p->pos.x = centerX;
        p->pos.y = centerY;
    }
    else {
        // Si no choca, avanza
        p->pos = nextPos;
        // Teletransporte de pantalla (Warp) izquierda-derecha
        if (p->pos.x > MAZE_WIDTH * TILE_SIZE) p->pos.x = 0;
        if (p->pos.x < 0) p->pos.x = MAZE_WIDTH * TILE_SIZE;
    }

    // --- ANIMACIÓN (BOCA ABRE/CIERRA) ---
    static int animFrame = 0;
    if (p->direction != STOP) {
        animFrame++;
        if (animFrame >= 10 / p->baseSpeed) {
            if (p->currentSprite.id == p->spriteOpen.id) p->currentSprite = p->spriteClosed;
            else p->currentSprite = p->spriteOpen;
            animFrame = 0;
        }
    }

    // --- INTERACCIÓN CON EL MAPA (COMER) ---
    if (distX < 6.0f && distY < 6.0f) {
        char tile = maze[gridY][gridX];

        if (tile == '.') { // Comer punto normal
            maze[gridY][gridX] = ' '; // Borrar del mapa
            p->score += 1;
            if (!IsSoundPlaying(p->munchSound)) PlaySound(p->munchSound);
        }
        else if (tile == 'F') { // Comer Fruta FUEGO
            maze[gridY][gridX] = ' ';
            p->score += 3;
            if (p->type != CLASS_SPEED) { // El veloz no recibe poder
                p->canShootFire = true;
                p->canShootIce = false;
                p->ammo = 2;
            }
        }
        else if (tile == 'I') { // Comer Fruta HIELO
            maze[gridY][gridX] = ' ';
            p->score += 3;
            if (p->type != CLASS_SPEED) {
                p->canShootIce = true;
                // Si ya tenía fuego, mantiene ambos (lógica híbrida)
                if (p->ammo > 0) {
                    p->canShootFire = true;
                }
                else {
                    p->canShootFire = false;
                }
            }
        }
        else if (tile == 'O') { // Comer Power Pellet (Fantasmas azules)
            maze[gridY][gridX] = ' ';
            p->score += 50;
            p->ghosts_eaten_this_powerup = 1;

            // Al comer esto, pierde armas de disparo
            if (p->type != CLASS_SPEED) {
                p->canShootFire = false;
                p->canShootIce = false;
                p->ammo = 0;
            }
        }
    }
}

// =========================================================
// 5. RENDERIZADO (DIBUJADO)
// Dibuja la textura correcta en la posición correcta con la rotación correcta.
// =========================================================
void Pacman_Draw(Pacman* p) {
    if (p->isDying) {
        // Dibuja animación de muerte
        DrawTexture(p->currentDeathAnim[p->deathFrame], (int)p->pos.x - 5, (int)p->pos.y - 5, WHITE);
    }
    else {
        // Calcula rotación según dirección
        float rotation = 0.0f;
        if (p->direction == LEFT) rotation = 180;
        if (p->direction == UP) rotation = 270;
        if (p->direction == DOWN) rotation = 90;

        // Configuración para DrawTexturePro (permite rotar desde el centro)
        Rectangle source = { 0, 0, (float)p->currentSprite.width, (float)p->currentSprite.height };
        Rectangle dest = { p->pos.x, p->pos.y, (float)p->currentSprite.width, (float)p->currentSprite.height };
        Vector2 origin = { (float)p->currentSprite.width / 2, (float)p->currentSprite.height / 2 };

        DrawTexturePro(p->currentSprite, source, dest, origin, rotation, WHITE);

        // Si es Tanque, dibuja el escudo verde extra
        if (p->hasArmor) DrawCircleLines((int)p->pos.x, (int)p->pos.y, 12, GREEN);
    }
}