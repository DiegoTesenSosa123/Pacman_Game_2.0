# ᗧ···ᗣ···ᗣ Pac-Man 2.0: Enhanced Clone

> Un tributo al legendario arcade de 1980, reprogramado desde cero en C con mecánicas avanzadas, IA de estados y sistema de escalado dinámico.

![Status](https://img.shields.io/badge/Estado-Completado-success)
![Lenguaje](https://img.shields.io/badge/Lenguaje-C99-blue)
![Librería](https://img.shields.io/badge/Librería-Raylib-red)
![Licencia](https://img.shields.io/badge/Licencia-MIT-green)

## 📋 Sobre el Proyecto
Este proyecto es una recreación avanzada del clásico **Pac-Man**, desarrollada como parte de mi formación en **Computación e Informática**. El objetivo fue ir más allá del clon básico, implementando una arquitectura modular y características técnicas modernas:

* **Sistema de Clases:** Selección de personajes con atributos únicos (Normal, Tanque con armadura y Veloz).
* **Habilidades Activas:** Implementación de proyectiles (fuego) y estados alterados (congelamiento).
* **Motor de Escalado (Letterboxing):** El juego utiliza una pantalla virtual de resolución fija que se adapta y centra automáticamente en cualquier tamaño de ventana, manteniendo la relación de aspecto original.
* **Persistencia de Datos:** Gestión de archivos para el guardado y ordenamiento (Bubble Sort) de los mejores puntajes.

## 📂 Estructura del Proyecto
El código sigue una organización modular para facilitar el mantenimiento y la escalabilidad:

```text
📦 PacMan-Project
 ┣ 📂 Niveles
 ┃ ┣ 📜 mapa.c         # Renderizado del laberinto y gestión de celdas
 ┃ ┗ 📜 mapa.h         # Matriz del nivel y definiciones de tiles
 ┣ 📂 Personajes
 ┃ ┣ 📜 ghost.c        # IA de fantasmas (Chase, Scatter, Frightened)
 ┃ ┣ 📜 ghost.h        # Máquina de estados de los enemigos
 ┃ ┣ 📜 pacman.c       # Físicas, movimiento y lógica de clases
 ┃ ┗ 📜 pacman.h       # Estructuras del jugador y habilidades
 ┣ 📂 assets           # Sprites .png y efectos de sonido .wav
 ┣ 📜 config.h         # Constantes globales, rutas y configuración de colores
 ┣ 📜 main.c           # Entry point, Game Loop y sistema de renderizado virtual
 ┣ 📜 menu.c           # Implementación de la UI (Menús, Settings y Highscores)
 ┣ 📜 menu.h           # Prototipos de las pantallas de interfaz
 ┣ 📜 scores.txt       # Almacenamiento local de puntajes
 ┗ 📜 types.h          # Definiciones de tipos, estructuras y enums

## 🎮 Controles

| Tecla | Acción |
| :---: | :--- |
| ⬆️ `Arriba` | Mover hacia arriba |
| ⬇️ `Abajo` | Mover hacia abajo |
| ⬅️ `Izquierda` | Mover hacia la izquierda |
| ➡️ `Derecha` | Mover hacia la derecha |
| `P` | Pausar juego |
| `R` | Reiniciar partida |


## 🛠️ Tecnologías Usadas
* **Lenguaje:** C (Standard C99/C11)
* **Gráficos:** [Raylib](https://www.raylib.com/)
* **IDE:** Visual Studio Code / Visual Studio

---
### ⚠️ Disclaimer
Este software es un proyecto estudiantil sin fines de lucro. **Pac-Man** es una marca registrada de **Bandai Namco Entertainment**. Todos los derechos de los personajes y el diseño original pertenecen a sus respectivos dueños.

⌨️ Desarrollado con ❤️ por [DiegoTesenSosa123](https://github.com/DiegoTesenSosa123)
