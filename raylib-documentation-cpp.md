# Documentation Raylib — Guide complet pour développeur C++

> Basé sur raylib v5.x/6.0 (API C, utilisable nativement en C++). zlib license, cross-platform (Windows, Linux, macOS).

---

## 1. GUIDES D'INSTALLATION & CONFIGURATION

Raylib est écrite en C99 mais s'utilise **directement en C++** sans wrapper : il suffit d'inclure `raylib.h` dans un fichier `.cpp`. Deux approches principales existent : installation système (package manager) ou intégration via CMake (recommandée, reproductible sur les 3 OS).

### 1.1 Linux (Ubuntu/Debian)

```bash
# Dépendances de compilation nécessaires (X11/Wayland, OpenGL, ALSA...)
sudo apt install build-essential git cmake \
    libasound2-dev libx11-dev libxrandr-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev

# Option A — via le gestionnaire de paquets (si disponible, version parfois ancienne)
sudo apt install libraylib-dev

# Option B — compiler depuis les sources et installer dans /usr/local
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
```

### 1.2 Windows

```powershell
# Option A — via vcpkg (recommandé)
git clone https://github.com/microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg install raylib

# Option B — MSYS2 / MinGW
pacman -S mingw-w64-x86_64-raylib

# Option C — w64devkit (kit officiel raylib, tout-en-un GCC + raylib précompilé)
# Téléchargeable directement sur raylib.com/ -> "raylib for Windows"
```

### 1.3 macOS

```bash
# Via Homebrew
brew install raylib
```

### 1.4 Méthode recommandée cross-platform : CMake + FetchContent

Cette méthode télécharge et compile raylib automatiquement depuis GitHub, **identique sur les 3 OS**, sans installation système préalable.

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(mon_jeu CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "5.5"          # fige une version stable plutôt que "master"
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(raylib)

add_executable(mon_jeu src/main.cpp)
target_link_libraries(mon_jeu PRIVATE raylib)

# Sur macOS, raylib nécessite ces frameworks système
if (APPLE)
    target_link_libraries(mon_jeu PRIVATE "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
endif()
```

```bash
# Compilation identique sur les 3 OS
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
./build/mon_jeu        # ou build\mon_jeu.exe sous Windows
```

### 1.5 Compilation manuelle en ligne de commande (sans CMake)

```bash
# Linux
g++ main.cpp -o mon_jeu -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# macOS
clang++ main.cpp -o mon_jeu -lraylib -framework IOKit -framework Cocoa -framework OpenGL

# Windows (MinGW)
g++ main.cpp -o mon_jeu.exe -lraylib -lopengl32 -lgdi32 -lwinmm
```

> **Flag clé à retenir :** `-lraylib` doit systématiquement être placé **après** les fichiers source à lier (l'ordre des `-l` compte pour l'éditeur de liens sur Linux/GCC).

---

## 2. ARCHITECTURE DE BASE & GAME LOOP

### 2.1 Squelette minimal

```cpp
#include "raylib.h"

int main() {
    // === INITIALISATION ===
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Mon Jeu Raylib");
    SetTargetFPS(60);

    // Chargement des ressources (textures, sons...) une seule fois ici

    // === BOUCLE PRINCIPALE ===
    while (!WindowShouldClose()) {   // détecte ESC ou clic sur la croix par défaut
        // --- UPDATE (logique du jeu, aucun appel de dessin ici) ---
        float dt = GetFrameTime();
        // ... mettre à jour positions, états, collisions ...

        // --- DRAW (uniquement du rendu, aucune logique ici) ---
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Hello, Raylib!", 190, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    // === NETTOYAGE / FERMETURE ===
    // UnloadTexture(...), UnloadSound(...) etc. avant CloseWindow()
    CloseWindow();
    return 0;
}
```

### 2.2 Les 4 phases

| Phase | Rôle | Fonctions typiques |
|---|---|---|
| **Initialisation** | Créer la fenêtre, charger les assets, définir l'état initial | `InitWindow`, `SetTargetFPS`, `LoadTexture`, `InitAudioDevice` |
| **Update** | Logique pure : physique, entrées, IA, collisions — **jamais d'appel `Draw*` ici** | `GetFrameTime`, `IsKeyDown`, `CheckCollisionRecs` |
| **Draw** | Rendu pur : **jamais de logique de jeu ici**, seulement afficher l'état déjà calculé | `BeginDrawing`/`EndDrawing`, `DrawRectangle`, `DrawTexture` |
| **Unload/Fermeture** | Libérer toutes les ressources chargées, dans l'ordre inverse du chargement | `UnloadTexture`, `UnloadSound`, `CloseAudioDevice`, `CloseWindow` |

> **Règle d'or :** toute logique qui modifie l'état du jeu va dans Update ; tout ce qui dessine va dans Draw. Mélanger les deux rend le code impossible à maintenir dès que le projet grossit.

---

## 3. SYNTAXE & FONCTIONS ESSENTIELLES (CHEAT SHEET C++)

### 3.1 Fenêtre & Temps

```cpp
InitWindow(800, 450, "Titre");     // crée la fenêtre + le contexte OpenGL
SetTargetFPS(60);                   // limite/lisse la boucle à 60 FPS
bool fini = WindowShouldClose();    // true si ESC pressé ou fenêtre fermée
float dt = GetFrameTime();          // temps écoulé (en secondes) depuis la frame précédente
double t = GetTime();               // temps total écoulé depuis InitWindow (secondes)
int fps = GetFPS();                 // FPS courant réel
CloseWindow();                      // libère le contexte graphique — dernier appel du programme

// Exemple : afficher le FPS en direct
DrawFPS(10, 10);
```

### 3.2 Entrées utilisateur

```cpp
// Clavier
if (IsKeyDown(KEY_RIGHT))   { /* maintenue enfoncée, à chaque frame */ }
if (IsKeyPressed(KEY_SPACE)){ /* vrai une seule fois, à l'instant de l'appui */ }
if (IsKeyReleased(KEY_A))   { /* vrai une seule fois, au relâchement */ }

// Souris
Vector2 pos = GetMousePosition();          // position (x, y) du curseur
if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { /* clic gauche */ }
float molette = GetMouseWheelMove();       // -1, 0 ou 1 selon le scroll

// Manette
if (IsGamepadAvailable(0)) {
    float axeX = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
}
```

**Exemple — déplacement d'un joueur avec les flèches :**
```cpp
Vector2 playerPos = { 400, 225 };
float speed = 200.0f;   // pixels par seconde

// Dans Update :
if (IsKeyDown(KEY_RIGHT)) playerPos.x += speed * dt;
if (IsKeyDown(KEY_LEFT))  playerPos.x -= speed * dt;
if (IsKeyDown(KEY_DOWN))  playerPos.y += speed * dt;
if (IsKeyDown(KEY_UP))    playerPos.y -= speed * dt;
```

### 3.3 Formes & Rendu 2D

```cpp
DrawRectangle(x, y, largeur, hauteur, RED);
DrawRectangleRec(rect, BLUE);                     // avec une struct Rectangle
DrawRectangleLines(x, y, largeur, hauteur, BLACK); // contour seulement

DrawCircle(centreX, centreY, rayon, GREEN);
DrawCircleV(centre, rayon, GREEN);                // avec une struct Vector2

DrawLine(x1, y1, x2, y2, DARKGRAY);
DrawLineEx(p1, p2, epaisseur, DARKGRAY);          // ligne avec épaisseur (Vector2)

DrawText("Score: 100", posX, posY, tailleFonte, BLACK);
DrawTextEx(fonte, "Texte", position, taille, espacement, BLACK); // police personnalisée

DrawPixel(x, y, WHITE);
DrawTriangle(p1, p2, p3, PURPLE);
```

Toujours encadrées par :
```cpp
BeginDrawing();
    ClearBackground(RAYWHITE);   // efface l'écran — à appeler en premier
    // ... tous les Draw* ici ...
EndDrawing();                    // échange les buffers, affiche réellement l'image
```

### 3.4 Gestion des couleurs & Structures

```cpp
// Vector2 : point ou vecteur 2D
Vector2 position = { 100.0f, 200.0f };
Vector2 somme = Vector2Add(position, {10, 0});   // fonctions utilitaires raymath.h

// Rectangle : boîte englobante (x, y = coin haut-gauche)
Rectangle boite = { 50.0f, 50.0f, 32.0f, 32.0f }; // x, y, largeur, hauteur

// Color : struct RGBA (valeurs 0-255)
Color monRouge = { 230, 41, 55, 255 };
// Couleurs prédéfinies : RED, GREEN, BLUE, RAYWHITE, BLACK, WHITE, GRAY,
// DARKGRAY, LIGHTGRAY, YELLOW, ORANGE, PURPLE, BROWN, MAGENTA...

Color transparent = Fade(RED, 0.5f);  // même couleur à 50% d'opacité
```

### 3.5 Textures & Sprites (introduction)

```cpp
// Chargement (une seule fois, en Initialisation)
Texture2D sprite = LoadTexture("assets/player.png");

// Affichage (en Draw, à chaque frame)
DrawTexture(sprite, x, y, WHITE);                 // position simple
DrawTextureV(sprite, position, WHITE);            // avec un Vector2
DrawTextureEx(sprite, position, rotation, scale, WHITE); // rotation + échelle

// Affichage d'une portion précise (sprite sheet / animation)
Rectangle source = { 0, 0, (float)sprite.width, (float)sprite.height };
Rectangle dest   = { x, y, 64, 64 };
Vector2 origin   = { 0, 0 };
DrawTexturePro(sprite, source, dest, origin, 0.0f, WHITE);

// Déchargement (en Fermeture, une seule fois)
UnloadTexture(sprite);
```

> Chaque `LoadTexture` doit avoir son `UnloadTexture` correspondant — sinon fuite mémoire GPU (voir section 4.2).

### 3.6 Collisions 2D

```cpp
Rectangle a = { 0, 0, 40, 40 };
Rectangle b = { 20, 20, 40, 40 };
if (CheckCollisionRecs(a, b)) { /* rectangle-rectangle */ }

Vector2 centreCercle = { 100, 100 };
float rayon = 20.0f;
if (CheckCollisionCircleRec(centreCercle, rayon, a)) { /* cercle-rectangle */ }

Vector2 centre2 = { 150, 100 };
if (CheckCollisionCircles(centreCercle, rayon, centre2, 15.0f)) { /* cercle-cercle */ }

if (CheckCollisionPointRec(GetMousePosition(), a)) { /* point (souris)-rectangle, utile pour un bouton UI */ }
```

---

## 4. BONS RÉFLEXES & ARCHITECTURE DE CODE C++

### 4.1 Structurer avec `struct` / `class`

Pour un petit projet, une `struct` avec des données + méthodes suffit largement — pas besoin d'héritage complexe pour démarrer.

```cpp
// entity.hpp
struct Player {
    Vector2 position;
    Vector2 velocity;
    Texture2D sprite;   // référence à une texture chargée ailleurs (voir 4.2)
    float speed = 200.0f;

    void Update(float dt) {
        // logique pure, aucun appel Draw ici
        if (IsKeyDown(KEY_RIGHT)) position.x += speed * dt;
        if (IsKeyDown(KEY_LEFT))  position.x -= speed * dt;
    }

    void Draw() const {
        // rendu pur, aucune logique ici
        DrawTextureV(sprite, position, WHITE);
    }
};
```

**Séparation logique/affichage :** chaque entité expose une méthode `Update(dt)` et une méthode `Draw()` distinctes. La boucle principale reste simple :

```cpp
while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    player.Update(dt);      // toute la logique ici
    ennemi.Update(dt);

    BeginDrawing();
        ClearBackground(RAYWHITE);
        player.Draw();      // uniquement du rendu ici
        ennemi.Draw();
    EndDrawing();
}
```

Pour un projet qui grossit, on regroupe généralement en :
```
src/
├── main.cpp
├── game.hpp / game.cpp        # orchestration Update()/Draw() globale
├── entities/
│   ├── player.hpp / .cpp
│   └── enemy.hpp / .cpp
├── systems/
│   └── collision_system.hpp   # logique transversale (ex: gérer toutes les collisions)
└── resources/
    └── resource_manager.hpp   # centralise chargement/déchargement des assets
```

### 4.2 Gestion des ressources (éviter les fuites mémoire)

Raylib **ne fait aucune gestion automatique de la mémoire GPU** (pas de RAII natif en C) : chaque `Load*` doit être explicitement déchargé avec le `Unload*` correspondant.

```cpp
LoadTexture(...)   ↔ UnloadTexture(...)
LoadSound(...)     ↔ UnloadSound(...)
LoadMusicStream(...) ↔ UnloadMusicStream(...)
LoadFont(...)      ↔ UnloadFont(...)
LoadModel(...)     ↔ UnloadModel(...)
InitAudioDevice()  ↔ CloseAudioDevice()
InitWindow(...)    ↔ CloseWindow()
```

**Solution recommandée en C++ moderne : encapsuler dans une classe RAII** pour que le destructeur décharge automatiquement la ressource.

```cpp
class TextureRAII {
public:
    Texture2D handle{};

    explicit TextureRAII(const char* path) {
        handle = LoadTexture(path);
    }
    ~TextureRAII() {
        UnloadTexture(handle);
    }

    // Interdire la copie (une Texture2D ne doit pas être déchargée deux fois)
    TextureRAII(const TextureRAII&) = delete;
    TextureRAII& operator=(const TextureRAII&) = delete;

    // Autoriser le déplacement
    TextureRAII(TextureRAII&& other) noexcept : handle(other.handle) {
        other.handle = { 0 };  // évite un double-unload
    }
};
```

> **Piège classique :** copier une `Texture2D` (struct simple contenant un ID GPU) ne duplique **pas** les données GPU. Si deux copies sont déchargées séparément, la seconde provoque une erreur ou un crash. D'où l'intérêt du pattern RAII ci-dessus, ou à minima d'un `ResourceManager` central qui possède les textures et ne les partage que par référence/pointeur.

**Un `ResourceManager` simple (alternative au RAII par objet) :**
```cpp
class ResourceManager {
public:
    Texture2D& Get(const std::string& path) {
        if (textures.find(path) == textures.end()) {
            textures[path] = LoadTexture(path.c_str());
        }
        return textures[path];
    }

    ~ResourceManager() {
        for (auto& [path, tex] : textures) UnloadTexture(tex);
    }

private:
    std::unordered_map<std::string, Texture2D> textures;
};
```

---

## 5. ERREURS CLASSIQUES & DÉBOGAGE (TROUBLESHOOTING)

### 5.1 Écran blanc / noir ou clignotements

**Cause :** oubli ou mauvais ordre de `BeginDrawing()` / `ClearBackground()` / `EndDrawing()`.

```cpp
// ❌ FAUX — ClearBackground appelé après avoir dessiné : efface tout ce qu'on vient de tracer
BeginDrawing();
    DrawRectangle(10, 10, 50, 50, RED);
    ClearBackground(RAYWHITE);
EndDrawing();

// ✅ CORRECT — Clear en premier, dessin ensuite
BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawRectangle(10, 10, 50, 50, RED);
EndDrawing();
```

Un **clignotement** vient souvent d'un `ClearBackground` appelé **en dehors** de `BeginDrawing`/`EndDrawing`, ou de plusieurs paires `BeginDrawing`/`EndDrawing` dans la même frame (il ne doit y en avoir **qu'une seule par itération de boucle**).

### 5.2 Erreurs de linkage à la compilation

```
undefined reference to `InitWindow'
undefined reference to `WindowShouldClose'
```

**Causes possibles et solutions :**
- `-lraylib` absent ou placé **avant** le fichier source dans la commande g++ → toujours le mettre **après** :
  ```bash
  g++ main.cpp -o jeu -lraylib -lGL -lm -lpthread -ldl -lrt -lX11   # ✅
  g++ -lraylib main.cpp -o jeu                                      # ❌ souvent en échec sur Linux/GCC
  ```
- Bibliothèques système manquantes sur Linux (X11, OpenGL, ALSA) → réinstaller les paquets `-dev` listés en section 1.1
- Sur Windows/MinGW, oubli de `-lopengl32 -lgdi32 -lwinmm`
- CMake : vérifier que `target_link_libraries(mon_jeu PRIVATE raylib)` est bien présent **après** `add_executable`

### 5.3 Segfaults ou crashs au chargement d'assets

**Cause n°1 : chemin relatif incorrect.** Le chemin passé à `LoadTexture("assets/player.png")` est relatif au **répertoire de travail courant au lancement du programme**, pas à l'emplacement du fichier source ni de l'exécutable.

```cpp
Texture2D sprite = LoadTexture("assets/player.png");
if (sprite.id == 0) {
    // La texture n'a pas pu être chargée : chemin invalide, raylib logue une erreur
    // dans la console mais NE PLANTE PAS directement ici...
}
DrawTexture(sprite, 0, 0, WHITE); // ...mais utiliser une texture invalide peut
                                    // provoquer un comportement indéfini plus loin
```

**Solutions :**
- Toujours lancer l'exécutable depuis le dossier racine du projet, ou définir un chemin absolu construit dynamiquement (`GetApplicationDirectory()` fourni par raylib)
- Vérifier systématiquement `sprite.id != 0` après un `LoadTexture` en développement
- En CMake, copier automatiquement le dossier `assets/` à côté du binaire :
  ```cmake
  file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
  ```

### 5.4 Déplacements dépendant des FPS

**Cause :** additionner une vitesse constante directement aux coordonnées, sans la multiplier par `GetFrameTime()` → le jeu va plus vite sur une machine à 144 FPS que sur une à 30 FPS.

```cpp
// ❌ FAUX — dépend du framerate de la machine
position.x += 5;

// ✅ CORRECT — indépendant du framerate (pixels par seconde réels)
float dt = GetFrameTime();
float speed = 300.0f; // pixels/seconde
position.x += speed * dt;
```

> Cette erreur est **la plus fréquente chez les débutants** venant d'autres frameworks : dans Raylib, `GetFrameTime()` doit systématiquement intervenir dans tout calcul de mouvement, physique, ou timer basé sur le temps réel.

### 5.5 Astuces de debug générales

```cpp
SetTraceLogLevel(LOG_ALL);        // affiche tous les logs internes raylib (chargements, erreurs...)
TraceLog(LOG_INFO, "Valeur : %d", maVariable); // logger custom au même format que raylib
DrawFPS(10, 10);                  // surveiller les FPS en overlay pendant le dev
```
