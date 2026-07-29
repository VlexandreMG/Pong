#include "raylib.h"

int main() {
    // === INITIALISATION ===
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Pong");
    SetTargetFPS(60);

    // Chargement des ressources (textures, sons...) une seule fois ici

    // === BOUCLE PRINCIPALE ===
    while (!WindowShouldClose()) {   // détecte ESC ou clic sur la croix par défaut
        // --- UPDATE (logique du jeu, aucun appel de dessin ici) ---
        float dt = GetFrameTime();
        // ... mettre à jour positions, états, collisions ...

        // --- DRAW (uniquement du rendu, aucune logique ici) ---
        BeginDrawing();
            // ClearBackground(RAYWHITE);
            // DrawText("Hello, Raylib!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    // === NETTOYAGE / FERMETURE ===
    // UnloadTexture(...), UnloadSound(...) etc. avant CloseWindow()
    CloseWindow();
    return 0;
}