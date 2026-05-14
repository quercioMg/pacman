#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include <pthread.h>
#include <stdbool.h>

#define MAP_ROWS 15
#define MAP_COLS 19
#define TILE_SIZE 40

// Usiamo i nomi che il main si aspetta (Stile BladeShift)
typedef enum {
    CELL_EMPTY = 0,
    CELL_WALL = 1,
    CELL_DOT = 2,
    CELL_SPECIAL = 3
} CellType;

typedef struct {
    int schema[MAP_ROWS][MAP_COLS];   // Rinominato da layout a schema per matchare il main
    bool visibile[MAP_ROWS][MAP_COLS];
    pthread_mutex_t mutex; 
} Mappa;

// Firme delle funzioni in inglese (come richiesto dal main)
void InitMap(Mappa *m);
void DrawMap(Mappa *m);
void UpdateFogOfWar(Mappa *m, Vector2 pos, float radius);
void UnloadMap(Mappa *m);

#endif
