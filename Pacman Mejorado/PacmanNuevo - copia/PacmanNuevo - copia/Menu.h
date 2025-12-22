#ifndef MENU_H  
#define MENU_H

#include "raylib.h"
#include "types.h"   
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

// 1. PROTOTIPOS / HERRAMIENTAS 

// CENTRAR PANTALLA
void DrawTextCentered(const char* text, int y, int fontSize, Color color);

// ESTANDARIZACIÓN DE BOTONES
void DrawButtonCentered(const char* text, int y, int fontSize, Color color);

// UI VISUAL
void DrawKey(const char* key, int x, int y, int width, Color color);


// 2. ARCHIVOS

void SaveNewScore(const char* playerName, int score);


// 3. PANTALLAS 

// INICIO
void Menu_DrawTitle(GameScreen* currentScreen, Texture2D tOpen, Texture2D tClosed);

// AJUSTES
void Menu_DrawSettings(GameScreen* currentScreen);

// SELECCIÓN 
void Menu_DrawClassSelect(GameScreen* currentScreen, PacmanClass* selectedClass);

// TUTORIAL 
void Menu_DrawInstructions(GameScreen* currentScreen, Texture2D tOpen, Texture2D tClosed);

// RANKING
void Menu_DrawHighScores(GameScreen* currentScreen);

// CARGA
void Menu_DrawLoading(GameScreen* currentScreen, int* timer, Texture2D logo, Texture2D tOpen, Texture2D tClosed);

// TRANSICIÓN 
void Menu_DrawReady(GameScreen* currentScreen, bool* startMusicPlayed, Sound startMusic, Sound sirenSound);

// FEEDBACK FINAL 
void Menu_DrawResult(GameScreen* currentScreen, bool didWin, Texture2D imgWin, Texture2D imgLose);

// INPUT DE TEXTO
void Menu_DrawNameInput(GameScreen* currentScreen, int score, float totalTime, char* playerName, int* letterCount);

// OVERLAY
void Menu_DrawPause(void);

#endif