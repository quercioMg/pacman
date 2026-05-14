#include "entities.h"

void InitPlayer(Player* p, Vector2 startPos) {
    p->position = startPos;
    p->speed = 110.0f; // Abbassata da 150 a 110 per maggior controllo
    p->radius = 12.0f; // Leggermente più piccolo del TILE_SIZE/2 per entrare nei corridoi
    p->score = 0;      // Inizializziamo il punteggio
    p->direction = DIR_RIGHT;
    p->isMouthOpen = true;

    p->texUp    = LoadTexture("resources/alto.png");
    p->texDown  = LoadTexture("resources/basso.png");
    p->texLeft  = LoadTexture("resources/sinistra.png");
    p->texRight = LoadTexture("resources/destra.png");
    p->texFull  = LoadTexture("resources/pieno.png");

    pthread_mutex_init(&p->mutex, NULL);
}
void UpdatePlayer(Player* p, float dt, Mappa* m) {
    pthread_mutex_lock(&p->mutex);

    // 1. INPUT CON "PRENOTAZIONE"
    int desiredDir = p->direction;
    if (IsKeyDown(KEY_RIGHT))      desiredDir = DIR_RIGHT;
    else if (IsKeyDown(KEY_LEFT))  desiredDir = DIR_LEFT;
    else if (IsKeyDown(KEY_UP))    desiredDir = DIR_UP;
    else if (IsKeyDown(KEY_DOWN))  desiredDir = DIR_DOWN;

    int gridX = (int)(p->position.x / TILE_SIZE);
    int gridY = (int)(p->position.y / TILE_SIZE);
    Vector2 cellCenter = { gridX * TILE_SIZE + 20, gridY * TILE_SIZE + 20 };

    // 2. MARGINE DI TOLLERANZA
    float tolerance = 15.0f;
    bool nearCenter = CheckCollisionCircles(p->position, tolerance, cellCenter, 0);

    if (nearCenter && desiredDir != p->direction) {
        int nextX = gridX, nextY = gridY;
        if (desiredDir == DIR_RIGHT) nextX++;
        else if (desiredDir == DIR_LEFT) nextX--;
        else if (desiredDir == DIR_UP) nextY--;
        else if (desiredDir == DIR_DOWN) nextY++;

        pthread_mutex_lock(&m->mutex);
        if (m->schema[nextY][nextX] != CELL_WALL) {
            p->direction = desiredDir;
            p->position = cellCenter;
        }
        pthread_mutex_unlock(&m->mutex);
    }

    // 3. MOVIMENTO CONTINUO
    Vector2 dirVec = {0, 0};
    if (p->direction == DIR_RIGHT) dirVec.x = 1;
    else if (p->direction == DIR_LEFT) dirVec.x = -1;
    else if (p->direction == DIR_UP) dirVec.y = -1;
    else if (p->direction == DIR_DOWN) dirVec.y = 1;

    Vector2 nextStep = {
        p->position.x + dirVec.x * p->speed * dt,
        p->position.y + dirVec.y * p->speed * dt
    };

    // 4. COLLISIONE E LOGICA MAPPA
    int checkX = (int)((nextStep.x + dirVec.x * 10) / TILE_SIZE);
    int checkY = (int)((nextStep.y + dirVec.y * 10) / TILE_SIZE);

    pthread_mutex_lock(&m->mutex);
    if (m->schema[checkY][checkX] != CELL_WALL) {
        p->position = nextStep;
        if (dirVec.x != 0) p->position.y = cellCenter.y;
        if (dirVec.y != 0) p->position.x = cellCenter.x;

        // Mangia pallini
        if (m->schema[gridY][gridX] == CELL_DOT) {
            m->schema[gridY][gridX] = CELL_EMPTY;
            p->score += 10;
        }
    }

    // --- AGGIUNTA: AGGIORNAMENTO VISIBILITÀ (LUCE) ---
    // Questo ciclo "accende" le celle vicino a Pac-Man
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            // Calcoliamo la distanza tra la posizione di Pac-Man e il centro di ogni cella
            float dist = Vector2Distance(p->position, (Vector2){ j * TILE_SIZE + 20, i * TILE_SIZE + 20 });
            
            // Se la cella è entro 120 pixel, diventa visibile permanentemente
            if (dist < 120.0f) {
                m->visibile[i][j] = true;
            }
        }
    }

    pthread_mutex_unlock(&m->mutex);
    pthread_mutex_unlock(&p->mutex);
}
void DrawPlayer(Player* p) {
    pthread_mutex_lock(&p->mutex);
    Texture2D tex = p->texFull;
    
    if (p->isMouthOpen) {
        switch(p->direction) {
            case DIR_UP:    tex = p->texUp;    break;
            case DIR_DOWN:  tex = p->texDown;  break;
            case DIR_LEFT:  tex = p->texLeft;  break;
            case DIR_RIGHT: tex = p->texRight; break;
        }
    }

    // --- DISEGNO PIÙ PICCOLO ---
    // Riduciamo la dimensione visiva a 30x30 (prima era 36x36)
    // Così non sembrerà mai che Pac-Man stia mangiando i muri.
    Rectangle dest = { p->position.x, p->position.y, 30, 30 };
    Vector2 origin = { 15, 15 }; // Metà di 30 per centrare l'immagine

    DrawTexturePro(tex, 
        (Rectangle){ 0, 0, (float)tex.width, (float)tex.height }, 
        dest, 
        origin, 
        0.0f, 
        WHITE);

    pthread_mutex_unlock(&p->mutex);
}
void UnloadPlayer(Player* p) {
    // ... (Il codice di UnloadPlayer rimane IDENTICO a prima) ...
    UnloadTexture(p->texUp);
    UnloadTexture(p->texDown);
    UnloadTexture(p->texLeft);
    UnloadTexture(p->texRight);
    UnloadTexture(p->texFull);
    pthread_mutex_destroy(&p->mutex);
}
