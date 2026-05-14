#include "map.h"
#include <math.h>

void InitMap(Mappa *m) {
    // Layout della mappa (1 = muro, 2 = pallino, 0 = vuoto)
    int schema_iniziale[MAP_ROWS][MAP_COLS] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,2,1},
        {1,2,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,2,1},
        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,2,1,2,1,1,1,1,1,2,1,2,1,1,2,1},
        {1,2,2,2,2,1,2,2,2,1,2,2,2,1,2,2,2,2,1},
        {1,1,1,1,2,1,1,1,0,1,0,1,1,1,2,1,1,1,1},
        {1,2,2,2,2,1,2,2,2,2,2,2,2,1,2,2,2,2,1},
        {1,2,1,1,2,1,2,1,1,1,1,1,2,1,2,1,1,2,1},
        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,2,1},
        {1,2,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,2,1},
        {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            m->schema[i][j] = schema_iniziale[i][j];
            m->visibile[i][j] = false; // All'inizio è tutto buio
        }
    }
    pthread_mutex_init(&m->mutex, NULL);
}

void UpdateFogOfWar(Mappa *m, Vector2 pos, float radius) {
    pthread_mutex_lock(&m->mutex);
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            // RESET: Ogni cella torna buia prima del calcolo
            m->visibile[i][j] = false; 

            Vector2 cellPos = { j * TILE_SIZE + TILE_SIZE/2, i * TILE_SIZE + TILE_SIZE/2 };
            float dist = sqrtf(powf(pos.x - cellPos.x, 2) + powf(pos.y - cellPos.y, 2));
            
            if (dist < radius) {
                m->visibile[i][j] = true;
            }
        }
    }
    pthread_mutex_unlock(&m->mutex);
}
void DrawMap(Mappa *m) {
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            pthread_mutex_lock(&m->mutex);
            bool vis = m->visibile[i][j];
            int tipo = m->schema[i][j];
            pthread_mutex_unlock(&m->mutex);

            Rectangle rect = { j * TILE_SIZE, i * TILE_SIZE, TILE_SIZE, TILE_SIZE };

            if (vis) {
                // Zona illuminata
                if (tipo == CELL_WALL) DrawRectangleRec(rect, BLUE);
                else if (tipo == CELL_DOT) DrawCircle(rect.x + TILE_SIZE/2, rect.y + TILE_SIZE/2, 4, YELLOW);
            } else {
                // Zona al buio: Disegniamo un contorno grigio scuro per i muri
                if (tipo == CELL_WALL) {
                    DrawRectangleLinesEx(rect, 1, DARKGRAY); 
                }
            }
        }
    }
}
void UnloadMap(Mappa *m) {
    pthread_mutex_destroy(&m->mutex);
}
