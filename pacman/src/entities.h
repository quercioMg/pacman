#ifndef ENTITIES_H
#define ENTITIES_H

#include "raylib.h"
#include <pthread.h>
#include "map.h" // Includiamo la mappa per le collisioni

typedef enum {
    DIR_RIGHT = 0,
    DIR_LEFT,
    DIR_UP,
    DIR_DOWN
} Direction;

typedef struct Player {
    Vector2 position;
    float speed;
    float radius;
    int score;           // NUOVO: Punteggio del giocatore
    
    Texture2D texUp;
    Texture2D texDown;
    Texture2D texLeft;
    Texture2D texRight;
    Texture2D texFull;

    Direction direction;
    bool isMouthOpen;
    
    pthread_mutex_t mutex;
} Player;

void InitPlayer(Player* p, Vector2 startPos);
// NUOVO: Passiamo la mappa come puntatore per controllare le collisioni
void UpdatePlayer(Player* p, float dt, Mappa* m); 
void DrawPlayer(Player* p);
void UnloadPlayer(Player* p);

#endif
