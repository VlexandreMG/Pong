#include "raylib.h"
#include "Ball.hpp"

int main() {
    // === INITIALISATION ===
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Pong");
    SetTargetFPS(60);

    int startX = screenWidth / 2;
    int startY = screenHeight / 2;

    Ball ball(startX,startY,5.0f,5.0f,20);

    // Chargement des ressources (textures, sons...) une seule fois ici

    // === BOUCLE PRINCIPALE ===
    while (!WindowShouldClose()) {   // détecte ESC ou clic sur la croix par défaut
        // --- UPDATE (logique du jeu, aucun appel de dessin ici) ---
        float dt = GetFrameTime();
        // ... mettre à jour positions, états, collisions ...

        // --- DRAW (uniquement du rendu, aucune logique ici) ---
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawCircle(ball.x, ball.y, ball.radius, );
        EndDrawing();
    }

    // === NETTOYAGE / FERMETURE ===
    // UnloadTexture(...), UnloadSound(...) etc. avant CloseWindow()
    CloseWindow();
    return 0;
}