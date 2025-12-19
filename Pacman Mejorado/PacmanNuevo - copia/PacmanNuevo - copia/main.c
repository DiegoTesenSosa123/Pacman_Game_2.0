#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include "types.h"
#include "config.h"
#include "menu.h"
#include "Personajes/pacman.h"
#include "Personajes/ghost.h"
#include "Niveles/mapa.h"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#define MAX_PROJECTILES 10

// Definimos CYAN para que puedas usarlo en la barra de volumen sin errores
#ifndef CYAN
#define CYAN (Color){ 0, 255, 255, 255 }
#endif

// Variables Globales
char maze[MAZE_HEIGHT][MAZE_WIDTH];
char initialmaze_copy[MAZE_HEIGHT][MAZE_WIDTH];
Projectile bullets[MAX_PROJECTILES] = { 0 };

char playerName[20] = "\0";
int letterCount = 0;

// --- FUNCIONES AUXILIARES ---
void SpawnFireball(Vector2 pos, Direction dir) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (!bullets[i].active) {
            bullets[i].active = true;
            bullets[i].pos = pos;
            bullets[i].velocity = (Vector2){ 0,0 };
            float speed = 6.0f;
            if (dir == RIGHT) bullets[i].velocity.x = speed;
            if (dir == LEFT) bullets[i].velocity.x = -speed;
            if (dir == UP) bullets[i].velocity.y = -speed;
            if (dir == DOWN) bullets[i].velocity.y = speed;
            if (dir == STOP) bullets[i].velocity.x = speed;
            break;
        }
    }
}

void UpdateProjectiles(Ghost fantasmas[], char maze[MAZE_HEIGHT][MAZE_WIDTH]) {
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (bullets[i].active) {
            bullets[i].pos.x += bullets[i].velocity.x;
            bullets[i].pos.y += bullets[i].velocity.y;
            int gx = (int)(bullets[i].pos.x / TILE_SIZE);
            int gy = (int)(bullets[i].pos.y / TILE_SIZE);
            if (gx < 0 || gx >= MAZE_WIDTH || gy < 0 || gy >= MAZE_HEIGHT || maze[gy][gx] == '#') {
                bullets[i].active = false;
                continue;
            }
            for (int g = 0; g < 4; g++) {
                if (fantasmas[g].state != GHOST_STATE_EATEN && fantasmas[g].state != GHOST_STATE_WAITING) {
                    Vector2 gPos = { fantasmas[g].pos.col * 20 + 10, fantasmas[g].pos.row * 20 + 10 };
                    if (CheckCollisionCircles(bullets[i].pos, 5, gPos, 10)) {
                        Ghost_SetEaten(&fantasmas[g]);
                        bullets[i].active = false;
                        break;
                    }
                }
            }
        }
    }
}

void ResetAndSpawnItems(char currentMaze[MAZE_HEIGHT][MAZE_WIDTH], PacmanClass pClass) {
    for (int i = 0; i < MAZE_HEIGHT; i++) for (int j = 0; j < MAZE_WIDTH; j++) currentMaze[i][j] = initialmaze_copy[i][j];
    int maxSpecialItems = (pClass == CLASS_SPEED) ? 2 : 4;
    int itemsPlaced = 0;
    while (itemsPlaced < maxSpecialItems) {
        int r = GetRandomValue(1, MAZE_HEIGHT - 2);
        int c = GetRandomValue(1, MAZE_WIDTH - 2);
        if (currentMaze[r][c] == '.' && !(r >= 13 && r <= 16 && c >= 11 && c <= 16)) {
            currentMaze[r][c] = (itemsPlaced < maxSpecialItems / 2) ? 'F' : 'I';
            itemsPlaced++;
        }
    }
}

int main(void) {
    // 1. CONFIGURACIÓN DE VENTANA
    InitWindow(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, "PAC-MAN FINAL");

    SetTargetFPS(60);
    InitAudioDevice();

    // VARIABLE DE VOLUMEN
    float masterVolume = 0.5f;
    SetMasterVolume(masterVolume);

    // --- CARGA DE RECURSOS ---
    for (int i = 0; i < MAZE_HEIGHT; i++) for (int j = 0; j < MAZE_WIDTH; j++) initialmaze_copy[i][j] = initialmaze[i][j];

    Texture2D pacmanLogo = LoadTexture(UI_LOGO_PACMAN);
    Texture2D tPacOpen = LoadTexture(PACMAN_SKIN_NORMAL_1);
    Texture2D tPacClosed = LoadTexture(PACMAN_SKIN_NORMAL_2);
    Texture2D tBlinky = LoadTexture(GHOST_SPRITE_BLINKY);
    Texture2D tPinky = LoadTexture(GHOST_SPRITE_PINKY);
    Texture2D tInky = LoadTexture(GHOST_SPRITE_INKY);
    Texture2D tClyde = LoadTexture(GHOST_SPRITE_CLYDE);
    Texture2D tFrightened = LoadTexture(GHOST_SPRITE_SCARED);
    Texture2D tFrightenedWhite = LoadTexture(GHOST_SPRITE_SCARED_W);
    Texture2D tEyes = LoadTexture(GHOST_SPRITE_EYES);
    Texture2D gameoverImg = LoadTexture(UI_IMG_GAMEOVER);
    Texture2D gamewinImg = LoadTexture(UI_IMG_GAMEWIN);

    Sound introMusic = LoadSound(AUDIO_INTRO_MUSIC);
    Sound startMusic = LoadSound(AUDIO_START_GAME);
    Sound sirenSound = LoadSound(AUDIO_SIREN_LOOP);
    Sound deathSound = LoadSound(AUDIO_DEATH_FX);
    Sound powerSound = LoadSound(AUDIO_POWER_PELLET);

    GameScreen currentScreen = SCREEN_TITLE;
    PacmanClass selectedClass = CLASS_NORMAL;

    float totalGameTime = 0.0f;
    int currentRound = 1; int maxRounds = 3; int loadingTimer = 0;
    bool didWin = false; bool powerMode = false; bool startMusicPlayed = false;

    Pacman pacman = Pacman_Init(CLASS_NORMAL);
    Ghost fantasmas[4];
    fantasmas[0] = Ghost_Init(BLINKY, tBlinky, tFrightened, tFrightenedWhite, tEyes);
    fantasmas[1] = Ghost_Init(PINKY, tPinky, tFrightened, tFrightenedWhite, tEyes);
    fantasmas[2] = Ghost_Init(INKY, tInky, tFrightened, tFrightenedWhite, tEyes);
    fantasmas[3] = Ghost_Init(CLYDE, tClyde, tFrightened, tFrightenedWhite, tEyes);

    while (!WindowShouldClose()) {
        // Lógica Global de Audio
        if (IsKeyPressed(KEY_P) && (currentScreen == SCREEN_GAMEPLAY || currentScreen == SCREEN_PAUSE)) {
            currentScreen = (currentScreen == SCREEN_GAMEPLAY) ? SCREEN_PAUSE : SCREEN_GAMEPLAY;
        }

        // --- AQUÍ ESTÁ EL CAMBIO PARA QUE SUENE EN RANKING (SCREEN_HIGHSCORES) ---
        if (!IsSoundPlaying(introMusic) && (currentScreen == SCREEN_TITLE || currentScreen == SCREEN_CLASS_SELECT || currentScreen == SCREEN_INSTRUCTIONS || currentScreen == SCREEN_SETTINGS || currentScreen == SCREEN_HIGHSCORES)) {
            PlaySound(introMusic); SetSoundVolume(introMusic, 0.6f);
        }
        if (currentScreen != SCREEN_TITLE && currentScreen != SCREEN_CLASS_SELECT && currentScreen != SCREEN_INSTRUCTIONS && currentScreen != SCREEN_SETTINGS && currentScreen != SCREEN_HIGHSCORES) {
            StopSound(introMusic);
        }

        // --- DIBUJADO DIRECTO ---
        BeginDrawing();
        ClearBackground(BLACK);

        switch (currentScreen) {
        case SCREEN_TITLE:
            Menu_DrawTitle(&currentScreen, tPacOpen, tPacClosed);
            if (IsKeyPressed(KEY_S)) currentScreen = SCREEN_SETTINGS;
            break;

        case SCREEN_SETTINGS:
        {
            Menu_DrawSettings(&currentScreen);

            if (IsKeyDown(KEY_RIGHT)) {
                masterVolume += 0.01f;
                if (masterVolume > 1.0f) masterVolume = 1.0f;
                SetMasterVolume(masterVolume);
            }
            else if (IsKeyDown(KEY_LEFT)) {
                masterVolume -= 0.01f;
                if (masterVolume < 0.0f) masterVolume = 0.0f;
                SetMasterVolume(masterVolume);
            }

            int barY = 180;
            int barWidth = 300;
            int barX = GAME_SCREEN_WIDTH / 2 - barWidth / 2;
            int barHeight = 25;

            DrawRectangle(barX, barY, barWidth, barHeight, Fade(DARKGRAY, 0.5f));
            DrawRectangleLines(barX, barY, barWidth, barHeight, WHITE);
            DrawRectangle(barX + 2, barY + 2, (int)((barWidth - 4) * masterVolume), barHeight - 4, GREEN);
            DrawText(TextFormat("%d%%", (int)(masterVolume * 100)), barX + barWidth + 10, barY + 2, 20, WHITE);

            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_S)) {
                currentScreen = SCREEN_TITLE;
            }
        }
        break;

        case SCREEN_CLASS_SELECT:
            Menu_DrawClassSelect(&currentScreen, &selectedClass);
            if (IsKeyPressed(KEY_ENTER)) {
                pacman = Pacman_Init(selectedClass);
                ResetAndSpawnItems(maze, selectedClass);
                currentScreen = SCREEN_LOADING; loadingTimer = 0; totalGameTime = 0.0f;
                fantasmas[0] = Ghost_Init(BLINKY, tBlinky, tFrightened, tFrightenedWhite, tEyes);
                fantasmas[1] = Ghost_Init(PINKY, tPinky, tFrightened, tFrightenedWhite, tEyes);
                fantasmas[2] = Ghost_Init(INKY, tInky, tFrightened, tFrightenedWhite, tEyes);
                fantasmas[3] = Ghost_Init(CLYDE, tClyde, tFrightened, tFrightenedWhite, tEyes);
            }
            break;

        case SCREEN_INSTRUCTIONS: Menu_DrawInstructions(&currentScreen, tPacOpen, tPacClosed); break;
        case SCREEN_HIGHSCORES: Menu_DrawHighScores(&currentScreen); break;

        case SCREEN_LOADING:
            Menu_DrawLoading(&currentScreen, &loadingTimer, pacmanLogo, tPacOpen, tPacClosed);
            if (currentScreen == SCREEN_READY) startMusicPlayed = false;
            break;

        case SCREEN_READY:
            Mapa_Draw(maze, BLUE); Pacman_Draw(&pacman); for (int i = 0; i < 4; i++) Ghost_Draw(&fantasmas[i]);
            Menu_DrawReady(&currentScreen, &startMusicPlayed, startMusic, sirenSound);
            DrawTextCentered(TextFormat("ROUND %i / %i", currentRound, maxRounds), 280, 30, YELLOW);
            break;

        case SCREEN_GAMEPLAY: {
            totalGameTime += GetFrameTime();
            if (powerMode) { if (!IsSoundPlaying(powerSound)) PlaySound(powerSound); }
            else { if (IsSoundPlaying(powerSound)) StopSound(powerSound); if (!IsSoundPlaying(sirenSound)) PlaySound(sirenSound); }

            Pacman_Update(&pacman, maze);

            if (pacman.canShootIce) { for (int i = 0; i < 4; i++) Ghost_SetFrozen(&fantasmas[i]); pacman.canShootIce = false; }
            if (pacman.ghosts_eaten_this_powerup == 1) { for (int i = 0; i < 4; i++) Ghost_SetFrightened(&fantasmas[i]); pacman.ghosts_eaten_this_powerup = 0; powerMode = true; }

            bool someoneScared = false; Color currentWallColor = BLUE;
            for (int i = 0; i < 4; i++) { if (fantasmas[i].state == GHOST_STATE_FRIGHTENED) someoneScared = true; if (fantasmas[i].frozenSpeedTimer > 0) currentWallColor = SKYBLUE; }
            if (!someoneScared) powerMode = false;

            if (IsKeyPressed(KEY_SPACE) && pacman.ammo > 0) { SpawnFireball(pacman.pos, pacman.direction); pacman.ammo--; }
            UpdateProjectiles(fantasmas, maze);

            Vector2Int pacmanGridPos = { (int)(pacman.pos.y / TILE_SIZE), (int)(pacman.pos.x / TILE_SIZE) };
            for (int i = 0; i < 4; i++) {
                Ghost_Update(&fantasmas[i], pacmanGridPos, maze);
                Vector2 gPos = { fantasmas[i].pos.col * 20 + 10, fantasmas[i].pos.row * 20 + 10 };
                if (CheckCollisionCircles(pacman.pos, 8, gPos, 8)) {
                    if (fantasmas[i].state == GHOST_STATE_FRIGHTENED) { Ghost_SetEaten(&fantasmas[i]); pacman.score += 200; }
                    else if (fantasmas[i].state == GHOST_STATE_NORMAL || fantasmas[i].state == GHOST_STATE_FROZEN) {
                        bool muerte = true;
                        if (pacman.type == CLASS_TANK && pacman.hasArmor) { pacman.hasArmor = false; pacman.currentSpeed = 2.0f; muerte = false; Ghost_SetEaten(&fantasmas[i]); }
                        if (muerte) { pacman.lives--; StopSound(sirenSound); StopSound(powerSound); PlaySound(deathSound); currentScreen = SCREEN_PACMAN_DYING; }
                    }
                }
            }

            if (Mapa_CountPellets(maze) == 0) {
                currentRound++; StopSound(sirenSound); if (IsSoundPlaying(powerSound)) StopSound(powerSound);
                if (currentRound > maxRounds) { didWin = true; currentScreen = SCREEN_RESULT; }
                else {
                    ResetAndSpawnItems(maze, selectedClass);
                    pacman.pos = (Vector2){ (float)(MAZE_WIDTH * TILE_SIZE) / 2 + TILE_SIZE / 2, (float)((MAZE_HEIGHT * TILE_SIZE) / 2 + TILE_SIZE * 2) };
                    pacman.direction = STOP;
                    if (pacman.type == CLASS_TANK) { pacman.hasArmor = true; pacman.currentSpeed = 1.5f; }
                    fantasmas[0] = Ghost_Init(BLINKY, tBlinky, tFrightened, tFrightenedWhite, tEyes);
                    fantasmas[1] = Ghost_Init(PINKY, tPinky, tFrightened, tFrightenedWhite, tEyes);
                    fantasmas[2] = Ghost_Init(INKY, tInky, tFrightened, tFrightenedWhite, tEyes);
                    fantasmas[3] = Ghost_Init(CLYDE, tClyde, tFrightened, tFrightenedWhite, tEyes);
                    startMusicPlayed = false; currentScreen = SCREEN_LOADING; loadingTimer = 0;
                }
            }

            Mapa_Draw(maze, currentWallColor); Pacman_Draw(&pacman);
            for (int i = 0; i < 4; i++) Ghost_Draw(&fantasmas[i]);
            for (int i = 0; i < MAX_PROJECTILES; i++) if (bullets[i].active) DrawCircleV(bullets[i].pos, 4, ORANGE);

            DrawText(TextFormat("ROUND: %i / %i", currentRound, maxRounds), 10, MAZE_HEIGHT * TILE_SIZE + 10, 20, SKYBLUE);
            DrawText(TextFormat("CLASS: %s", pacman.type == CLASS_TANK ? "TANK" : (pacman.type == CLASS_SPEED ? "SPEED" : "NORMAL")), 10, MAZE_HEIGHT * TILE_SIZE + 30, 20, SKYBLUE);
            DrawText(TextFormat("AMMO: %i", pacman.ammo), 200, MAZE_HEIGHT * TILE_SIZE + 30, 20, ORANGE);
            DrawText(TextFormat("LIVES: %i", pacman.lives), GetScreenWidth() - 100, MAZE_HEIGHT * TILE_SIZE + 10, 20, RED);
            DrawText(TextFormat("SCORE: %i", pacman.score), GetScreenWidth() - 100, MAZE_HEIGHT * TILE_SIZE + 30, 20, YELLOW);
        } break;

        case SCREEN_PAUSE:
            Mapa_Draw(maze, BLUE); Pacman_Draw(&pacman); for (int i = 0; i < 4; i++) Ghost_Draw(&fantasmas[i]);
            Menu_DrawPause();
            break;

        case SCREEN_PACMAN_DYING:
            Mapa_Draw(maze, BLUE);
            if (Pacman_UpdateDeath(&pacman)) {
                if (pacman.lives <= 0) { didWin = false; currentScreen = SCREEN_RESULT; }
                else {
                    pacman.pos = (Vector2){ (float)(MAZE_WIDTH * TILE_SIZE) / 2 + TILE_SIZE / 2, (float)((MAZE_HEIGHT * TILE_SIZE) / 2 + TILE_SIZE * 2) };
                    pacman.direction = STOP; pacman.isDying = false;
                    if (pacman.type == CLASS_TANK) { pacman.hasArmor = true; pacman.currentSpeed = 1.5f; }
                    fantasmas[0] = Ghost_Init(BLINKY, tBlinky, tFrightened, tFrightenedWhite, tEyes);
                    fantasmas[1] = Ghost_Init(PINKY, tPinky, tFrightened, tFrightenedWhite, tEyes);
                    fantasmas[2] = Ghost_Init(INKY, tInky, tFrightened, tFrightenedWhite, tEyes);
                    fantasmas[3] = Ghost_Init(CLYDE, tClyde, tFrightened, tFrightenedWhite, tEyes);
                    startMusicPlayed = false; currentScreen = SCREEN_READY;
                }
            }
            Pacman_Draw(&pacman);
            break;

        case SCREEN_RESULT: Menu_DrawResult(&currentScreen, didWin, gamewinImg, gameoverImg); break;

        case SCREEN_NAME_INPUT:
            Menu_DrawNameInput(&currentScreen, pacman.score, totalGameTime, playerName, &letterCount);
            if (letterCount > 0 && IsKeyPressed(KEY_ENTER)) {
                SaveNewScore(playerName, pacman.score);
                letterCount = 0; playerName[0] = '\0'; currentRound = 1; didWin = false; pacman.score = 0;
                currentScreen = SCREEN_TITLE;
            }
            break;
        }

        EndDrawing();
    }

    // LIMPIEZA
    UnloadSound(introMusic); UnloadSound(startMusic); UnloadSound(sirenSound); UnloadSound(deathSound); UnloadSound(powerSound);
    UnloadTexture(tBlinky); UnloadTexture(tPinky); UnloadTexture(tInky); UnloadTexture(tClyde);
    UnloadTexture(tFrightened); UnloadTexture(tFrightenedWhite); UnloadTexture(tEyes);
    UnloadTexture(pacmanLogo); UnloadTexture(gameoverImg); UnloadTexture(gamewinImg);
    UnloadTexture(tPacOpen); UnloadTexture(tPacClosed);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}