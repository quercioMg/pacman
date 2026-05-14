#include "ghost.h"
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

Vector2 GetRandomSpawnPoint(Mappa* m) {
    Vector2 validPoints[MAP_ROWS * MAP_COLS];
    int count = 0;

    pthread_mutex_lock(&m->mutex);
    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            // Se la cella non è un muro, è un potenziale punto di spawn
            if (m->schema[y][x] != CELL_WALL) {
                validPoints[count] = (Vector2){ 
                    x * TILE_SIZE + TILE_SIZE / 2, 
                    y * TILE_SIZE + TILE_SIZE / 2 
                };
                count++;
            }
        }
    }
    pthread_mutex_unlock(&m->mutex);

    if (count > 0) {
        return validPoints[rand() % count];
    }
    
    // Fallback di sicurezza se non trova nulla (molto improbabile)
    return (Vector2){ TILE_SIZE + 20, TILE_SIZE + 20 };
}
void InitGhost(Ghost* g, Vector2 pos, Color color, Mappa* m) {
    g->position = pos;
    g->color = color;
    g->mappa = m;
    g->active = true;
    g->direction = (Vector2){ 0, 0 };
    g->speed = 100.0f;
    
    // Inizializzazione fondamentale per la logica dei thread
    g->lastGridX = -1;
    g->lastGridY = -1;

    pthread_create(&g->thread, NULL, GhostUpdateThread, g);
}
void* GhostUpdateThread(void* arg) {
    Ghost* g = (Ghost*)arg;
    Vector2 dirs[] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

    while (g->active) {
        float dt = 0.016f;
        int gx = (int)(g->position.x / TILE_SIZE);
        int gy = (int)(g->position.y / TILE_SIZE);
        Vector2 center = { gx * TILE_SIZE + 20, gy * TILE_SIZE + 20 };

        // 1. CONTROLLO BIVIO
        if (CheckCollisionCircles(g->position, 4.0f, center, 0)) {
            Vector2 scelteValide[4];
            int numScelte = 0;

            pthread_mutex_lock(&g->mappa->mutex);
            for (int i = 0; i < 4; i++) {
                int nx = gx + (int)dirs[i].x;
                int ny = gy + (int)dirs[i].y;

                if (nx >= 0 && nx < MAP_COLS && ny >= 0 && ny < MAP_ROWS) {
                    if (g->mappa->schema[ny][nx] != CELL_WALL) {
                        scelteValide[numScelte] = dirs[i];
                        numScelte++;
                    }
                }
            }
            pthread_mutex_unlock(&g->mappa->mutex);

            // LOGICA ANTI-TREMOLIO: Usiamo la memoria personale del fantasma
            if (gx != g->lastGridX || gy != g->lastGridY) {
                if (numScelte > 0) {
                    g->direction = scelteValide[rand() % numScelte];
                    g->position = center; // Snap perfetto
                }
                g->lastGridX = gx;
                g->lastGridY = gy;
            }
        }

        // 2. MOVIMENTO
        Vector2 next = {
            g->position.x + g->direction.x * g->speed * dt,
            g->position.y + g->direction.y * g->speed * dt
        };

        // Controllo muro frontale per sicurezza
        int fx = (int)((next.x + g->direction.x * 15) / TILE_SIZE);
        int fy = (int)((next.y + g->direction.y * 15) / TILE_SIZE);

        pthread_mutex_lock(&g->mappa->mutex);
        if (g->mappa->schema[fy][fx] == CELL_WALL) {
            g->position = center;
            // Se sbatte, resettiamo la memoria per forzare una nuova scelta
            g->lastGridX = -1; 
        } else {
            g->position = next;
        }
        pthread_mutex_unlock(&g->mappa->mutex);

        usleep(16000);
    }
    return NULL;
}
void DrawGhost(Ghost* g) {
    // Calcoliamo la cella in cui si trova il fantasma
    int gx = (int)(g->position.x / TILE_SIZE);
    int gy = (int)(g->position.y / TILE_SIZE);

    // Controllo di sicurezza per non uscire dall'array
    if (gx < 0 || gx >= MAP_COLS || gy < 0 || gy >= MAP_ROWS) return;

    pthread_mutex_lock(&g->mappa->mutex);
    bool visibile = g->mappa->visibile[gy][gx];
    pthread_mutex_unlock(&g->mappa->mutex);

    if (visibile) {
        // Disegno del corpo
        DrawCircleV(g->position, TILE_SIZE/2 - 5, g->color);
        DrawRectangle(g->position.x - (TILE_SIZE/2 - 5), g->position.y, (TILE_SIZE/2 - 5) * 2, TILE_SIZE/2 - 5, g->color);
        
        // Occhietti
        DrawCircle(g->position.x - 5, g->position.y - 2, 3, WHITE);
        DrawCircle(g->position.x + 5, g->position.y - 2, 3, WHITE);
    }
}
