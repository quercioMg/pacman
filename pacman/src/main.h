#ifndef MAIN_H
#define MAIN_H

#include "raylib.h"
#include "entities.h"
#include "map.h"
#include "ghost.h"
#include <pthread.h>

// Struttura per passare dati al thread della nebbia
typedef struct {
    Mappa* mappa;
    Player* player;
} ThreadData;

// Prototipi delle funzioni thread
void* FogUpdateThread(void* arg);
void* AnimationThread(void* arg);

#endif
