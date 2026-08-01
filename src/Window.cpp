#include "raylib.h"
#include "entities/Ball.hpp"
#include "entities/Line.hpp"
#include "systems/Collisions.hpp"

int main() {
    // === INITIALISATION ===
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Pong");
    SetTargetFPS(60);

    int startX = screenWidth / 2;
    int startY = screenHeight / 2;

    Ball ball(startX,startY,5.0f,5.0f,20);
    Collisions collision;
    Line net = {{200.0f,80.0f},{200.0f, 600.0f}, 100.0f, LIGHTGRAY};
    float lineSpeed = 5.0f;

    // Chargement des ressources (textures, sons...) une seule fois ici

    // === BOUCLE PRINCIPALE ===
    while (!WindowShouldClose()) {   // détecte ESC ou clic sur la croix par défaut

        if (IsKeyDown(KEY_UP)) {
            net.moveY(-lineSpeed);
        }


        if (IsKeyDown(KEY_DOWN)) {
            net.moveY(lineSpeed);
        }

        ball.update();
        collision.detectCollision(ball,screenHeight,screenWidth);
        collision.detectLineCollision(ball,net);
        // --- DRAW (uniquement du rendu, aucune logique ici) ---
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawCircle(ball.x, ball.y, ball.radius, RED);
            DrawText(TextFormat("Point accumulé : %d",collision.point), 190, 200, 20, LIGHTGRAY);
            DrawLineEx(net.startPos, net.endPos, net.thickness, net.color); 
        EndDrawing();
    }

    // === NETTOYAGE / FERMETURE ===
    // UnloadTexture(...), UnloadSound(...) etc. avant CloseWindow()
    CloseWindow();
    return 0;
}