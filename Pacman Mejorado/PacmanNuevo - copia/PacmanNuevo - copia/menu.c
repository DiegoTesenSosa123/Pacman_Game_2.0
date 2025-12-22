#define _CRT_SECURE_NO_WARNINGS
#include "menu.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// Definimos constantes de color si no existen en la librería base
#ifndef CYAN
#define CYAN (Color){ 0, 255, 255, 255 }
#endif

// Estructura local para manejar el ordenamiento de puntajes en memoria
typedef struct {
    char name[20];
    int score;
} HighScoreEntry;

// =========================================================
// IMPLEMENTACIÓN DE HERRAMIENTAS VISUALES
// =========================================================


void DrawTextCentered(const char* text, int y, int fontSize, Color color) {
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, GAME_SCREEN_WIDTH / 2 - textWidth / 2, y, fontSize, color);
}

void DrawButtonCentered(const char* text, int y, int fontSize, Color color) {
    int fixedWidth = 450;

    if (fixedWidth > GAME_SCREEN_WIDTH - 20) fixedWidth = GAME_SCREEN_WIDTH - 40;

    int paddingY = 10;
    int rectHeight = fontSize + (paddingY * 2);

    // para centrar 
    int rectX = (GAME_SCREEN_WIDTH / 2) - (fixedWidth / 2);
    int rectY = y - paddingY;

    DrawRectangle(rectX, rectY, fixedWidth, rectHeight, Fade(DARKGRAY, 0.8f));
    DrawRectangleLines(rectX, rectY, fixedWidth, rectHeight, color);

    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (GAME_SCREEN_WIDTH / 2) - (textWidth / 2), y, fontSize, color);
}

void DrawKey(const char* key, int x, int y, int width, Color color) {
    int height = 40;
    DrawRectangle(x, y, width, height, Fade(BLACK, 0.6f));
    DrawRectangleLines(x, y, width, height, color);
    DrawRectangleLines(x - 1, y - 1, width + 2, height + 2, Fade(color, 0.5f));

    int fontSize = 20;
    int textWidth = MeasureText(key, fontSize);
    DrawText(key, x + (width / 2) - (textWidth / 2), y + (height / 2) - (fontSize / 2), fontSize, WHITE);
}

// =========================================================
// LÓGICA DE PERSISTENCIA DE DATOS
// =========================================================

void SaveNewScore(const char* playerName, int score) {
    HighScoreEntry entries[6];
    int count = 0;

    FILE* file = fopen("scores.txt", "r");
    if (file != NULL) {
        while (count < 5 && fscanf(file, "%19s %d", entries[count].name, &entries[count].score) == 2) count++;
        fclose(file);
    }

    strcpy(entries[count].name, playerName);
    entries[count].score = score;
    count++;

    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (entries[j].score < entries[j + 1].score) {
                HighScoreEntry temp = entries[j]; entries[j] = entries[j + 1]; entries[j + 1] = temp;
            }
        }
    }

    file = fopen("scores.txt", "w");
    if (file != NULL) {
        int saveCount = (count < 5) ? count : 5;
        for (int i = 0; i < saveCount; i++) fprintf(file, "%s %d\n", entries[i].name, entries[i].score);
        fclose(file);
    }
}

// =========================================================
// LÓGICA DE INTERFAZ DE USUARIO (UI)
// =========================================================

void Menu_DrawTitle(GameScreen* currentScreen, Texture2D tOpen, Texture2D tClosed) {
    float time = GetTime();
    float pulse = fabs(sin(time * 3.0f));

    Color titleColor = (Color){ 255, 255, 0, (unsigned char)(80 + 170 * pulse) };
    Color startColor = (Color){ 0, 255, 0, (unsigned char)(80 + 170 * pulse) };
    Color instrColor = (Color){ 255, 161, 0, (unsigned char)(80 + 170 * pulse) };
    Color histColor = (Color){ 200, 200, 200, (unsigned char)(80 + 170 * pulse) };
    Color confColor = (Color){ 0, 255, 255, (unsigned char)(80 + 170 * pulse) };

    DrawTextCentered("PAC-MAN 2.0", 100, 60, titleColor);

    static float animX = -50.0f;
    static int animFrame = 0;
    float speed = 4.0f;
    int animY = 200;
    int pelletGap = 40;

    animX += speed;
    animFrame++;
    if (animX > GAME_SCREEN_WIDTH + 50) animX = -50.0f;

    for (int i = 0; i < GAME_SCREEN_WIDTH + 20; i += pelletGap) {
        if (i > animX + 10) DrawCircle(i, animY + 15, 5, Fade(PINK, 0.5f));
    }

    Texture2D currentTex = ((animFrame / 10) % 2 == 0) ? tOpen : tClosed;
    DrawTexture(currentTex, (int)animX, animY, WHITE);

    DrawButtonCentered("PRESIONA ENTER PARA COMENZAR", 300, 20, startColor);
    DrawButtonCentered("ESPACIO - INSTRUCCIONES", 360, 18, instrColor);
    DrawButtonCentered("H - HISTORIAL", 410, 18, histColor);
    DrawButtonCentered("S - CONFIGURACION", 460, 18, confColor);

    if (IsKeyPressed(KEY_ENTER)) *currentScreen = SCREEN_CLASS_SELECT;
    if (IsKeyPressed(KEY_SPACE)) *currentScreen = SCREEN_INSTRUCTIONS;
    if (IsKeyPressed(KEY_H)) *currentScreen = SCREEN_HIGHSCORES;
    if (IsKeyPressed(KEY_S)) *currentScreen = SCREEN_SETTINGS;
}

void Menu_DrawSettings(GameScreen* currentScreen) {
    DrawTextCentered("CONFIGURACION", 60, 40, YELLOW);
    DrawLine(50, 110, GAME_SCREEN_WIDTH - 50, 110, GRAY);

    // 1. DEFINICIÓN DE RESOLUCIONES
    static int resIndex = 0;

    const int widths[] = { GAME_SCREEN_WIDTH, 1280, 1920 };
    const int heights[] = { GAME_SCREEN_HEIGHT, 720, 1080 };
    const char* resNames[] = { "ORIGINAL (Ventana)", "HD 720p", "FULL HD 1080p" };

    // 2. SECCIÓN VOLUMEN
    DrawTextCentered("AJUSTE DE VOLUMEN", 140, 25, CYAN);
    DrawTextCentered("Usa las flechas Izquierda / Derecha", 220, 20, LIGHTGRAY);

    // 3. SECCIÓN RESOLUCIÓN
    DrawTextCentered("TAMAÑO DE PANTALLA", 280, 25, CYAN);

    // LÓGICA DE CAMBIO DE RESOLUCIÓN + CENTRADO
    // Solo usamos ARRIBA/ABAJO para evitar conflictos con el volumen
    if (IsKeyPressed(KEY_UP)) {
        resIndex++;
        if (resIndex > 2) resIndex = 0;

        SetWindowSize(widths[resIndex], heights[resIndex]);

        // CENTRADO AUTOMÁTICO EN EL MONITOR
        // Esto coloca la ventana justo en el medio de tu pantalla física
        SetWindowPosition(
            (GetMonitorWidth(0) - widths[resIndex]) / 2,
            (GetMonitorHeight(0) - heights[resIndex]) / 2
        );
    }

    if (IsKeyPressed(KEY_DOWN)) {
        resIndex--;
        if (resIndex < 0) resIndex = 2;

        SetWindowSize(widths[resIndex], heights[resIndex]);

        // CENTRADO AUTOMÁTICO EN EL MONITOR
        SetWindowPosition(
            (GetMonitorWidth(0) - widths[resIndex]) / 2,
            (GetMonitorHeight(0) - heights[resIndex]) / 2
        );
    }

    Color resColor = (resIndex == 0) ? GREEN : ((resIndex == 1) ? ORANGE : GOLD);
    DrawTextCentered(TextFormat("< %s >", resNames[resIndex]), 320, 25, resColor);
    DrawTextCentered(TextFormat("(%i x %i)", widths[resIndex], heights[resIndex]), 350, 20, DARKGRAY);
    DrawTextCentered("Usa Arriba / Abajo para cambiar", 390, 20, LIGHTGRAY);

    DrawButtonCentered("PRESIONA 'S' PARA VOLVER", 500, 20, ORANGE);

    if (IsKeyPressed(KEY_S)) *currentScreen = SCREEN_TITLE;
}

void Menu_DrawClassSelect(GameScreen* currentScreen, PacmanClass* selectedClass) {
    DrawTextCentered("PAC-MAN 2.0", 60, 40, YELLOW);
    DrawLine(50, 110, GAME_SCREEN_WIDTH - 50, 110, GRAY);
    DrawTextCentered("ELIGE TU CLASE", 150, 35, WHITE);

    Color c1 = (*selectedClass == CLASS_NORMAL) ? GREEN : DARKGRAY;
    Color c2 = (*selectedClass == CLASS_TANK) ? GREEN : DARKGRAY;
    Color c3 = (*selectedClass == CLASS_SPEED) ? GREEN : DARKGRAY;

    int boxWidth = 400;
    if (boxWidth > GAME_SCREEN_WIDTH - 40) boxWidth = GAME_SCREEN_WIDTH - 40;

    int boxX = GAME_SCREEN_WIDTH / 2 - boxWidth / 2;
    int boxHeight = 50; int spacing = 80; int startY = 220;

    if (*selectedClass == CLASS_NORMAL) DrawRectangle(boxX, startY, boxWidth, boxHeight, Fade(GREEN, 0.2f));
    DrawRectangleLines(boxX, startY, boxWidth, boxHeight, c1);
    DrawTextCentered("1. NORMAL (Equilibrado)", startY + 15, 22, c1);

    if (*selectedClass == CLASS_TANK) DrawRectangle(boxX, startY + spacing, boxWidth, boxHeight, Fade(GREEN, 0.2f));
    DrawRectangleLines(boxX, startY + spacing, boxWidth, boxHeight, c2);
    DrawTextCentered("2. TANQUE (Lento + Armor)", startY + spacing + 15, 22, c2);

    if (*selectedClass == CLASS_SPEED) DrawRectangle(boxX, startY + spacing * 2, boxWidth, boxHeight, Fade(GREEN, 0.2f));
    DrawRectangleLines(boxX, startY + spacing * 2, boxWidth, boxHeight, c3);
    DrawTextCentered("3. VELOZ (Rapido - Sin Poder)", startY + spacing * 2 + 15, 22, c3);

    if (IsKeyPressed(KEY_ONE)) *selectedClass = CLASS_NORMAL;
    if (IsKeyPressed(KEY_TWO)) *selectedClass = CLASS_TANK;
    if (IsKeyPressed(KEY_THREE)) *selectedClass = CLASS_SPEED;

    if ((int)(GetTime() * 2) % 2 == 0) {
        DrawButtonCentered("PRESIONA ENTER PARA INICIAR", 530, 22, ORANGE);
    }
}

void Menu_DrawInstructions(GameScreen* currentScreen, Texture2D tOpen, Texture2D tClosed) {
    static int instructionPage = 0;
    float pulse = fabs(sin(GetTime() * 4.0f));
    Color keyColor = (Color){ 0, 255, 255, (unsigned char)(100 + 155 * pulse) };
    int startY = 50;

    if (instructionPage == 0) {
        DrawTextCentered("CONTROLES", startY, 30, YELLOW);
        DrawLine(50, startY + 40, GAME_SCREEN_WIDTH - 50, startY + 40, GRAY);

        int centerX = GAME_SCREEN_WIDTH / 2;
        int keySize = 50;
        int gap = 10;
        int keysY = (GAME_SCREEN_HEIGHT / 2) - 90;

        DrawKey("W", centerX - (keySize / 2), keysY, keySize, keyColor);
        DrawText("Arriba", centerX - MeasureText("Arriba", 20) / 2, keysY - 25, 20, WHITE);

        int row2Y = keysY + keySize + gap;
        DrawKey("A", centerX - (keySize / 2) - keySize - gap, row2Y, keySize, keyColor);
        DrawText("Izq.", centerX - (keySize / 2) - keySize - gap - 45, row2Y + 15, 20, WHITE);

        DrawKey("S", centerX - (keySize / 2), row2Y, keySize, keyColor);
        DrawText("Abajo", centerX - MeasureText("Abajo", 20) / 2, row2Y + keySize + 5, 20, WHITE);

        DrawKey("D", centerX - (keySize / 2) + keySize + gap, row2Y, keySize, keyColor);
        DrawText("Der.", centerX + (keySize / 2) + keySize + gap + 10, row2Y + 15, 20, WHITE);

        int spaceY = row2Y + keySize + gap * 3;
        DrawKey("ESPACIO", centerX - 100, spaceY, 200, ORANGE);
        DrawText("Disparo", centerX - MeasureText("Disparo", 20) / 2, spaceY + 45, 20, ORANGE);

        DrawButtonCentered("ESPACIO - SIGUIENTE PAGINA", GAME_SCREEN_HEIGHT - 60, 20, ORANGE);

        if (IsKeyPressed(KEY_SPACE)) instructionPage = 1;

    }
    else {
        DrawTextCentered("PODERES Y CLASES", startY, 30, YELLOW);
        DrawLine(50, startY + 40, GAME_SCREEN_WIDTH - 50, startY + 40, GRAY);

        Texture2D currentAnimSprite = ((int)(GetTime() * 10) % 2 == 0) ? tOpen : tClosed;
        int lineHeight = 40;
        int contentStart = startY + 80;
        int startX = 50;

        DrawTexturePro(currentAnimSprite, (Rectangle) { 0, 0, 16, 16 }, (Rectangle) { startX, contentStart + 3, 18, 18 }, (Vector2) { 0, 0 }, 0, WHITE);
        DrawText("NORMAL: Equilibrado", startX + 40, contentStart + 2, 20, GREEN);

        DrawTexturePro(currentAnimSprite, (Rectangle) { 0, 0, 16, 16 }, (Rectangle) { startX, contentStart + lineHeight + 3, 18, 18 }, (Vector2) { 0, 0 }, 0, WHITE);
        DrawCircleLines(startX + 9, contentStart + lineHeight + 12, 12, GREEN);
        DrawText("TANQUE: Lento + Armadura (1 golpe)", startX + 40, contentStart + lineHeight + 2, 20, WHITE);

        DrawTexturePro(currentAnimSprite, (Rectangle) { 0, 0, 16, 16 }, (Rectangle) { startX, contentStart + lineHeight * 2 + 3, 18, 18 }, (Vector2) { 0, 0 }, 0, WHITE);
        DrawText("VELOZ: Muy Rapido - SIN DISPARO", startX + 40, contentStart + lineHeight * 2 + 2, 20, WHITE);

        int powerY = contentStart + lineHeight * 3 + 40;
        DrawText("ITEMS ESPECIALES:", startX, powerY, 22, YELLOW);

        DrawCircle(startX + 12, powerY + 40, 8, RED);
        DrawText("FUEGO: 2 Balas (Mata Fantasmas)", startX + 40, powerY + 30, 20, WHITE);

        DrawCircle(startX + 12, powerY + 80, 8, SKYBLUE);
        DrawText("HIELO: Congela el mapa 5 seg", startX + 40, powerY + 70, 20, WHITE);

        DrawCircle(startX + 12, powerY + 120, 9, GOLD);
        DrawText("CLASICA: Fantasmas Comibles", startX + 40, powerY + 110, 20, WHITE);

        DrawButtonCentered("ESPACIO - VOLVER AL MENU", GAME_SCREEN_HEIGHT - 60, 20, ORANGE);

        if (IsKeyPressed(KEY_SPACE)) {
            *currentScreen = SCREEN_TITLE;
            instructionPage = 0;
        }
    }
}

void Menu_DrawHighScores(GameScreen* currentScreen) {
    DrawTextCentered("MEJORES PUNTAJES", 50, 40, YELLOW);
    DrawLine(50, 100, GAME_SCREEN_WIDTH - 50, 100, GRAY);

    HighScoreEntry entries[10];
    int count = 0;

    FILE* file = fopen("scores.txt", "r");
    if (file != NULL) {
        while (count < 10 && fscanf(file, "%19s %d", entries[count].name, &entries[count].score) == 2) count++;
        fclose(file);
    }

    if (count == 0) DrawTextCentered("No hay puntajes registrados aun.", 250, 20, WHITE);
    else {
        for (int i = 0; i < count - 1; i++) {
            for (int j = 0; j < count - i - 1; j++) {
                if (entries[j].score < entries[j + 1].score) {
                    HighScoreEntry temp = entries[j]; entries[j] = entries[j + 1]; entries[j + 1] = temp;
                }
            }
        }
        int y = 150;
        for (int i = 0; i < count; i++) {
            Color c = (i == 0) ? GOLD : (i == 1 ? LIGHTGRAY : (i == 2 ? ORANGE : WHITE));
            char textBuffer[60];
            sprintf(textBuffer, "#%d  %-10s ....... %d", i + 1, entries[i].name, entries[i].score);
            DrawTextCentered(textBuffer, y, 30, c);
            y += 50;
        }
    }
    DrawButtonCentered("H - REGRESAR", GAME_SCREEN_HEIGHT - 60, 20, ORANGE);
    if (IsKeyPressed(KEY_H)) *currentScreen = SCREEN_TITLE;
}

void Menu_DrawLoading(GameScreen* currentScreen, int* timer, Texture2D logo, Texture2D tOpen, Texture2D tClosed) {
    (*timer)++;
    float time = GetTime();
    float pulse = fabs(sin(time * 3.0f));
    Color titleColor = (Color){ 255, 255, 0, (unsigned char)(80 + 170 * pulse) };

    int centerY = GAME_SCREEN_HEIGHT / 2;
    DrawTextCentered("PAC-MAN 2.0", centerY - 100, 60, titleColor);
    DrawTextCentered("CARGANDO MAPA...", centerY - 20, 20, LIGHTGRAY);

    int barWidth = 300;
    int startX = GAME_SCREEN_WIDTH / 2 - barWidth / 2;
    int yPos = centerY + 30;
    float progress = (float)(*timer) / 180.0f;
    int pacX = startX + (int)(barWidth * progress);

    for (int i = 0; i < 10; i++) {
        int dotX = startX + i * 30 + 15;
        if (pacX < dotX) DrawCircle(dotX, yPos, 4, WHITE);
    }
    Texture2D currentPacTex = ((*timer / 10) % 2 == 0) ? tOpen : tClosed;
    DrawTexture(currentPacTex, pacX - 10, yPos - 10, WHITE);

    if (*timer > 180) *currentScreen = SCREEN_READY;
}

void Menu_DrawReady(GameScreen* currentScreen, bool* startMusicPlayed, Sound startMusic, Sound sirenSound) {
    if (!(*startMusicPlayed)) {
        PlaySound(startMusic);
        *startMusicPlayed = true;
    }
    DrawTextCentered("READY!", 325, 50, YELLOW);
    if (!IsSoundPlaying(startMusic)) {
        PlaySound(sirenSound);
        *currentScreen = SCREEN_GAMEPLAY;
    }
}

void Menu_DrawResult(GameScreen* currentScreen, bool didWin, Texture2D imgWin, Texture2D imgLose) {
    if (didWin) {
        DrawTexture(imgWin, 40, 200, WHITE);
        DrawTextCentered("¡HAS GANADO!", 150, 40, GREEN);
    }
    else {
        DrawTexture(imgLose, 200, 180, WHITE);
        DrawTextCentered("GAME OVER", 120, 40, RED);
    }
    DrawButtonCentered("PRESIONA ENTER PARA CONTINUAR", 500, 20, WHITE);
    if (IsKeyPressed(KEY_ENTER)) *currentScreen = SCREEN_NAME_INPUT;
}

void Menu_DrawNameInput(GameScreen* currentScreen, int score, float totalTime, char* playerName, int* letterCount) {
    DrawTextCentered(TextFormat("PUNTAJE FINAL: %i", score), 200, 40, SKYBLUE);
    DrawTextCentered(TextFormat("TIEMPO TOTAL: %02i:%02i", (int)totalTime / 60, (int)totalTime % 60), 250, 30, YELLOW);
    DrawTextCentered("INGRESA TU NOMBRE (Sin espacios):", 330, 20, WHITE);

    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 33) && (key <= 125) && (*letterCount < 10)) {
            playerName[*letterCount] = (char)key;
            playerName[*letterCount + 1] = '\0';
            (*letterCount)++;
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        (*letterCount)--;
        if (*letterCount < 0) *letterCount = 0;
        playerName[*letterCount] = '\0';
    }
    DrawTextCentered(playerName, 380, 30, WHITE);

    if (*letterCount > 0) {
        DrawButtonCentered("PRESIONA ENTER PARA GUARDAR", 500, 20, GREEN);
    }
}

void Menu_DrawPause(void) {
    DrawRectangle(0, 0, GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, Fade(BLACK, 0.7f));
    DrawTextCentered("PAUSA", GAME_SCREEN_HEIGHT / 2 - 50, 60, YELLOW);
    DrawButtonCentered("PRESIONA 'P' PARA CONTINUAR", GAME_SCREEN_HEIGHT / 2 + 30, 20, WHITE);
}