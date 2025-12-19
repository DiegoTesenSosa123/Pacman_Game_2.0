#include "mapa.h"
#include "raylib.h"


// =========================================================
// 1. DEFINICIÓN DEL NIVEL
// =========================================================
// LEYENDA DE SÍMBOLOS:
// '#' = Pared (Muro sólido)
// '.' = Bolita normal (Puntos)
// 'O' = Power Pellet (Hace vulnerables a los fantasmas)
// '_' = Puerta de la casa de los fantasmas
// 'F' = Fruta de Fuego (Munición roja)
// 'I' = Fruta de Hielo (Munición azul)
// ' ' = Espacio vacío (Ya comido o pasillo)
// =========================================================

const char initialmaze[MAZE_HEIGHT][MAZE_WIDTH] = {
    "############################",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#O####.#####.##.#####.####O#",
    "#.####.#####.##.#####.####.#",
    "#..........................#",
    "#.####.##.########.##.####.#",
    "#.####.##.########.##.####.#",
    "#......##....##....##......#",
    "######.#####.##.#####.######",
    "     #.#####.##.#####.#     ",
    "     #.##          ##.#     ",
    "     #.## ###__### ##.#     ",
    "######.## #      # ##.######",
    "          #      #          ",
    "######.## #      # ##.######",
    "     #.## ######## ##.#     ",
    "     #.##          ##.#     ",
    "     #.##.########.##.#     ",
    "######.##.########.##.######",
    "#............##............#",
    "#.####.#####.##.#####.####.#",
    "#.####.#####.##.#####.####.#",
    "#O..##................##..O#",
    "###.##.##.########.##.##.###",
    "###.##.##.########.##.##.###",
    "#......##....##....##......#",
    "#.##########.##.##########.#",
    "#..........................#",
    "############################"
};

// =========================================================
// 2. FUNCIÓN DE DIBUJADO (RENDER)
// Recorre el mapa letra por letra y dibuja los cuadrados
// o círculos correspondientes en la pantalla.
// =========================================================
void Mapa_Draw(char maze[MAZE_HEIGHT][MAZE_WIDTH], Color wallColor) {
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {

            float posX = (float)x * TILE_SIZE;
            float posY = (float)y * TILE_SIZE;
            Vector2 center = { posX + TILE_SIZE / 2, posY + TILE_SIZE / 2 };

            char cell = maze[y][x];

            if (cell == '#') {
                // Dibujar pared
                DrawRectangle(posX, posY, TILE_SIZE, TILE_SIZE, BLACK);
                DrawRectangleLinesEx((Rectangle) { posX, posY, TILE_SIZE, TILE_SIZE }, 2, wallColor);
            }
            else if (cell == '.') {
                // Dibujar punto normal
                DrawCircleV(center, 3, (Color) { 255, 184, 174, 255 });
            }
            else if (cell == 'O') {
                // Dibujar Power Pellet grande
                DrawCircleV(center, 7, (Color) { 255, 184, 174, 255 });
            }
            else if (cell == '_') {
                // Dibujar puerta de fantasmas
                DrawRectangle(posX, posY + 8, TILE_SIZE, 4, PINK);
            }
            else if (cell == 'F') {
                // Dibujar fruta de Fuego
                DrawCircleV(center, 8, ORANGE);
                DrawCircleV(center, 4, YELLOW);
            }
            else if (cell == 'I') {
                // Dibujar fruta de Hielo
                DrawCircleV(center, 8, SKYBLUE);
                DrawCircleV(center, 4, WHITE);
            }
        }
    }
}

// =========================================================
// 3. CONTADOR DE OBJETIVOS
// Cuenta cuántos puntos quedan. Si llega a 0, el jugador gana.
// =========================================================
int Mapa_CountPellets(char maze[MAZE_HEIGHT][MAZE_WIDTH]) {
    int count = 0;
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            char c = maze[y][x];
            // Suma todo lo que sea comestible
            if (c == '.' || c == 'O' || c == 'F' || c == 'I') {
                count++;
            }
        }
    }
    return count;
}