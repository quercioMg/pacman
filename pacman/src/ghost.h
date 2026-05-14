#ifndef GHOST_H
#define GHOST_H

#include "raylib.h"
#include "map.h"
#include <pthread.h>

typedef struct {
    Vector2 position;
    Vector2 direction;
    float speed;
    Color color;
    pthread_t thread;
    bool active;
    Mappa* mappa;
    // Memoria personale per evitare il tremolio
    int lastGridX;
    int lastGridY;
} Ghost;

void InitGhost(Ghost* g, Vector2 pos, Color color, Mappa* m);
void DrawGhost(Ghost* g);
void* GhostUpdateThread(void* arg);
Vector2 GetRandomSpawnPoint(Mappa* m);

#endif
