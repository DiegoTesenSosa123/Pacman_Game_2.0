#ifndef MENU_H
#define MENU_H

#include "raylib.h"
#include "types.h"   // Importante para conocer GameScreen y PacmanClass
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// --- FUNCIONES VISUALES AUXILIARES ---
void DrawTextCentered(const char* text, int y, int fontSize, Color color);
void DrawButtonCentered(const char* text, int y, int fontSize, Color color);
void DrawKey(const char* key, int x, int y, int width, Color color);

// --- LÓGICA DE PUNTAJES ---
void SaveNewScore(const char* playerName, int score);

// --- PANTALLAS DEL MENÚ ---
void Menu_DrawTitle(GameScreen* currentScreen, Texture2D tOpen, Texture2D tClosed);
void Menu_DrawSettings(GameScreen* currentScreen); 
void Menu_DrawClassSelect(GameScreen* currentScreen, PacmanClass* selectedClass);
void Menu_DrawInstructions(GameScreen* currentScreen, Texture2D tOpen, Texture2D tClosed);
void Menu_DrawHighScores(GameScreen* currentScreen);
void Menu_DrawLoading(GameScreen* currentScreen, int* timer, Texture2D logo, Texture2D tOpen, Texture2D tClosed);
void Menu_DrawReady(GameScreen* currentScreen, bool* startMusicPlayed, Sound startMusic, Sound sirenSound);
void Menu_DrawResult(GameScreen* currentScreen, bool didWin, Texture2D imgWin, Texture2D imgLose);
void Menu_DrawNameInput(GameScreen* currentScreen, int score, float totalTime, char* playerName, int* letterCount);
void Menu_DrawPause(void);

#endif