#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Thread per la nebbia dinamica
void* FogUpdateThread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    while (!WindowShouldClose()) {
        pthread_mutex_lock(&data->player->mutex);
        Vector2 pos = data->player->position;
        pthread_mutex_unlock(&data->player->mutex);

        UpdateFogOfWar(data->mappa, pos, 120.0f);
        WaitTime(0.016); 
    }
    return NULL;
}

// Thread per l'animazione della bocca
void* AnimationThread(void* arg) {
    Player* p = (Player*)arg;
    while (!WindowShouldClose()) {
        pthread_mutex_lock(&p->mutex);
        p->isMouthOpen = !p->isMouthOpen;
        pthread_mutex_unlock(&p->mutex);
        WaitTime(0.2); 
    }
    return NULL;
}

int main(void) {
    srand(time(NULL));

    const int screenWidth = 760;  
    const int screenHeight = 630; 

    InitWindow(screenWidth, screenHeight, "Pac-Man Multi-Thread Edition");
    SetTargetFPS(60);

    // --- 1. INIZIALIZZAZIONE ---
    Mappa mappa;
    InitMap(&mappa);

    Player pacman;
    InitPlayer(&pacman, (Vector2){ 60, 60 });

    Ghost fantasma1;
    InitGhost(&fantasma1, GetRandomSpawnPoint(&mappa), RED, &mappa);

    Ghost fantasma2;
    InitGhost(&fantasma2, GetRandomSpawnPoint(&mappa), PINK, &mappa);

    bool gameOver = false;

    // Rettangoli per i bottoni
    Rectangle btnRiapri = { screenWidth/2 - 150, screenHeight/2 + 60, 140, 40 };
    Rectangle btnEsci = { screenWidth/2 + 10, screenHeight/2 + 60, 140, 40 };
    Vector2 mousePoint = { 0.0f, 0.0f };

    // --- 2. AVVIO THREAD AMBIENTE ---
    pthread_t tid_fog, tid_anim;
    ThreadData fData = { &mappa, &pacman };
    
    pthread_create(&tid_fog, NULL, FogUpdateThread, &fData);
    pthread_create(&tid_anim, NULL, AnimationThread, &pacman);

    // --- 3. LOOP PRINCIPALE ---
    while (!WindowShouldClose()) {
        if (!gameOver) {
            HideCursor();
            UpdatePlayer(&pacman, GetFrameTime(), &mappa);

            // Controllo Collisione
            pthread_mutex_lock(&pacman.mutex);
            if (CheckCollisionCircles(pacman.position, 12, fantasma1.position, 12) ||
                CheckCollisionCircles(pacman.position, 12, fantasma2.position, 12)) {
                gameOver = true;
            }
            pthread_mutex_unlock(&pacman.mutex);
        } else {
            ShowCursor();
            mousePoint = GetMousePosition();

            // LOGICA BOTTONE RIAPRI (RESET)
            if (CheckCollisionPointRec(mousePoint, btnRiapri) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                pacman.position = (Vector2){ 60, 60 };
                pacman.direction = DIR_RIGHT;
                pacman.score = 0;
                
                InitMap(&mappa);
                
                fantasma1.position = GetRandomSpawnPoint(&mappa);
                fantasma2.position = GetRandomSpawnPoint(&mappa);
                
                gameOver = false;
            }

            // --- LOGICA BOTTONE ESCI (CORRETTA) ---
            if (CheckCollisionPointRec(mousePoint, btnEsci) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		// 1. Fermiamo i fantasmi
    		fantasma1.active = false;
    		fantasma2.active = false;

    		// 2. Chiudiamo la finestra (questo sblocca anche WindowShouldClose() nei thread)
    		CloseWindow();

    		// 3. Usciamo direttamente dal main senza aspettare i join (opzionale se hai fretta)
    		// o semplicemente return 0;
    		exit(0);
            }
        }

        BeginDrawing();
            ClearBackground(BLACK);

            DrawMap(&mappa);
            DrawGhost(&fantasma1);
            DrawGhost(&fantasma2);
            DrawPlayer(&pacman);

            if (gameOver) {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
                DrawText("GAME OVER", screenWidth/2 - 110, screenHeight/2 - 40, 40, RED);

                Color colRiapri = CheckCollisionPointRec(mousePoint, btnRiapri) ? DARKGREEN : GREEN;
                Color colEsci = CheckCollisionPointRec(mousePoint, btnEsci) ? MAROON : RED;

                DrawRectangleRec(btnRiapri, colRiapri);
                DrawText("RIGIOCA", btnRiapri.x + 30, btnRiapri.y + 10, 20, WHITE);

                DrawRectangleRec(btnEsci, colEsci);
                DrawText("ESCI", btnEsci.x + 45, btnEsci.y + 10, 20, WHITE);
            } else {
                DrawRectangle(0, screenHeight - 30, screenWidth, 30, Fade(BLACK, 0.8f));
                DrawText(TextFormat("SCORE: %05d", pacman.score), 10, screenHeight - 25, 20, YELLOW);
            }
        EndDrawing();
    }

    // --- 4. CHIUSURA E PULIZIA ---
    fantasma1.active = false;
    fantasma2.active = false;

    // Usiamo CloseWindow prima dei join per segnalare a WindowShouldClose() nei thread
    CloseWindow();

    pthread_join(tid_anim, NULL);
    pthread_join(tid_fog, NULL);
    pthread_join(fantasma1.thread, NULL);
    pthread_join(fantasma2.thread, NULL);

    UnloadPlayer(&pacman);
    UnloadMap(&mappa);

    return 0;
}
