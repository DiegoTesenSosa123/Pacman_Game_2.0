#include "mapa.h"
#include "raylib.h"

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
// Recorremos la matriz celda por celda, 
// comenzando desde la esquina superior izquierda, moviéndonos por columnas (x) y bajando por filas (y)."
void Mapa_Draw(char maze[MAZE_HEIGHT][MAZE_WIDTH], Color wallColor) {
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {


            // Hacemos la conversion logica de la matriz x y  a un poscion real en pixeles pos x y pos y
            float posX = (float)x * TILE_SIZE;
            float posY = (float)y * TILE_SIZE;
            Vector2 center = { posX + TILE_SIZE / 2, posY + TILE_SIZE / 2 };


            // Capturamos el simbolo de la pos de la matriz para saber que pintar
            char cell = maze[y][x];


            // renderizar el cotenido del mapa, mediante las condiciones dibuja
            if (cell == '#') {
                DrawRectangle(posX, posY, TILE_SIZE, TILE_SIZE, BLACK);
                DrawRectangleLinesEx((Rectangle) { posX, posY, TILE_SIZE, TILE_SIZE }, 2, wallColor);
            }
            else if (cell == '.') {
                DrawCircleV(center, 3, (Color) { 255, 184, 174, 255 });
            }
            else if (cell == 'O') {
                DrawCircleV(center, 7, (Color) { 255, 184, 174, 255 });
            }
            else if (cell == '_') {
                DrawRectangle(posX, posY + 8, TILE_SIZE, 4, PINK);
            }
            else if (cell == 'F') {
                DrawCircleV(center, 8, ORANGE);
                DrawCircleV(center, 4, YELLOW);
            }
            else if (cell == 'I') {
                DrawCircleV(center, 8, SKYBLUE);
                DrawCircleV(center, 4, WHITE);
            }
        }
    }
}
int Mapa_CountPellets(char maze[MAZE_HEIGHT][MAZE_WIDTH]) {
    int count = 0;
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            //capturamos el conetido de la poscion
            char c = maze[y][x];
            if (c == '.' || c == 'O' || c == 'F' || c == 'I') {
                count++;
            }
        }
    }
    return count;
}