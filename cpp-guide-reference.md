# Guide de référence C++ moderne (C++17/C++20)

> Ressource de consultation pour développement d'applications et de moteurs de jeu 2D/3D.

---

## 1. LES FONDAMENTAUX & LA GESTION DE LA MÉMOIRE

### 1.1 Types de données & Transtypage (Casting)

```cpp
int a = 5;
float b = 2.0f;

// Conversion implicite int -> float : sans risque, aucune perte
float c = a;              // 5.0f

// Conversion implicite float -> int : TRONQUE (ne fait pas d'arrondi !)
int d = 7.9f;              // d == 7, pas 8 !
int e = -7.9f;              // e == -7 (troncature vers zéro, pas vers le bas)

// Division entière : piège classique
int x = 5, y = 2;
float resultat = x / y;     // 2.0f !! la division se fait en int AVANT la conversion
float correct  = (float)x / y;   // 2.5f — il faut caster AVANT la division
```

**Les 4 casts C++ explicites (à préférer systématiquement aux casts C `(type)value`) :**

```cpp
// static_cast : conversion "normale" entre types compatibles (numériques, hiérarchie de classes connue)
double pi = 3.14159;
int entier = static_cast<int>(pi);           // 3, troncature explicite et intentionnelle

// dynamic_cast : conversion sécurisée dans une hiérarchie polymorphique (nécessite des méthodes virtuelles)
Base* basePtr = new Derived();
Derived* d = dynamic_cast<Derived*>(basePtr); // nullptr si la conversion est invalide

// const_cast : retire (ou ajoute) le const — À ÉVITER sauf cas très spécifiques (API legacy)
void ancienneFonction(char* str);
const char* txt = "hello";
ancienneFonction(const_cast<char*>(txt));    // dangereux si la fonction modifie réellement txt

// reinterpret_cast : réinterprète le bit-pattern brut — le plus dangereux, rare en usage normal
```

> **Règle :** `static_cast` couvre 95% des besoins légitimes. Si tu écris `(int)maVariable` (cast à la C), remplace systématiquement par `static_cast<int>(maVariable)` — plus explicite, plus grep-able, et le compilateur refuse les conversions incohérentes que le cast C accepterait silencieusement.

### 1.2 Stack vs Heap (Pile vs Tas)

| | Stack (pile) | Heap (tas) |
|---|---|---|
| Allocation | Automatique, à la déclaration | Manuelle (`new`) ou via smart pointer |
| Libération | Automatique, à la sortie du scope | Manuelle (`delete`) ou automatique (smart pointer) |
| Vitesse | Très rapide (juste un déplacement de pointeur de pile) | Plus lente (recherche d'un bloc libre) |
| Taille | Limitée (quelques Mo typiquement) | Limitée par la RAM disponible |
| Durée de vie | Le temps du scope `{ }` | Jusqu'à libération explicite ou fin de vie du smart pointer |

```cpp
void exempleStack() {
    int x = 42;                 // alloué sur la pile
    Vector2 pos = {0, 0};       // idem, struct alloué sur la pile
}   // x et pos sont automatiquement détruits ici — aucune action requise

void exempleHeap() {
    int* x = new int(42);       // alloué sur le tas
    // ... utilisation de x ...
    delete x;                    // OBLIGATOIRE — sinon fuite mémoire
}
```

**Quand utiliser quoi :**
- **Stack par défaut**, toujours — plus rapide, aucun risque de fuite, gestion automatique.
- **Heap uniquement quand nécessaire** : taille inconnue à la compilation, durée de vie dépassant le scope courant (un objet qui doit survivre à la fonction qui le crée), objets volumineux qui saturaient la pile, ou polymorphisme via pointeurs de classe de base.
- En C++ moderne, le heap se manipule **quasiment toujours via un smart pointer** (section 1.4), jamais via `new`/`delete` bruts dans du code applicatif.

### 1.3 Pointeurs & Références

```cpp
int valeur = 10;

// Pointeur : contient une ADRESSE, peut être null, peut être réassigné
int* ptr = &valeur;         // ptr contient l'adresse de "valeur"
*ptr = 20;                   // déréférencement : modifie "valeur" via son adresse
ptr = nullptr;                // un pointeur peut ne rien pointer

// Référence : un ALIAS du même objet, ne peut PAS être null, ne peut PAS être réassignée
int& ref = valeur;            // ref EST valeur, juste sous un autre nom
ref = 30;                     // modifie directement "valeur", pas de déréférencement nécessaire
// int& ref2;                 // ERREUR DE COMPIL : une référence doit être initialisée immédiatement
```

| | Pointeur | Référence |
|---|---|---|
| Peut être `nullptr` | Oui | Non (toujours lié à un objet valide) |
| Réassignable | Oui | Non, liée définitivement à son objet d'origine |
| Syntaxe d'accès | `*ptr`, `ptr->membre` | Transparente, comme l'objet lui-même |
| Arithmétique | Possible (`ptr++`) | Impossible |

### Passage par valeur vs par référence

```cpp
// Passage par VALEUR : copie complète de l'objet — coûteux pour les gros objets
void parValeur(std::vector<int> v) { v.push_back(1); }   // modifie une COPIE, l'original est intact

// Passage par référence : aucune copie, modifie l'original
void parReference(std::vector<int>& v) { v.push_back(1); } // modifie directement l'appelant

// Passage par référence CONSTANTE : aucune copie, ET protection contre la modification
// -> LE CHOIX PAR DÉFAUT pour passer un objet en lecture seule (string, vector, struct volumineuse)
void parReferenceConst(const std::vector<int>& v) {
    // v.push_back(1);   // ERREUR DE COMPIL : v est en lecture seule
    std::cout << v.size();
}
```

> **Règle pratique :** pour les types simples (`int`, `float`, `bool`), passe par valeur (c'est déjà optimal). Pour tout objet plus complexe (`std::string`, `std::vector`, une `struct`/`class`), passe par `const Type&` sauf si tu as explicitement besoin de le modifier (alors `Type&`) ou d'en garder une copie indépendante (alors `Type` par valeur, volontairement).

### 1.4 Pointeurs intelligents (Smart Pointers)

Ils encapsulent un pointeur brut et libèrent automatiquement la mémoire via leur destructeur (RAII, voir 2.2) — **plus jamais de `delete` manuel en code applicatif moderne**.

```cpp
#include <memory>

// std::unique_ptr : PROPRIÉTÉ UNIQUE — un seul unique_ptr peut posséder l'objet à la fois
std::unique_ptr<Texture2D> tex = std::make_unique<Texture2D>();
// tex se détruit (et libère la mémoire) automatiquement à la fin du scope
// std::unique_ptr<Texture2D> tex2 = tex;   // ERREUR DE COMPIL : la copie est interdite
std::unique_ptr<Texture2D> tex2 = std::move(tex); // OK : transfert de propriété, tex devient nullptr

// std::shared_ptr : PROPRIÉTÉ PARTAGÉE — compteur de références, libéré quand le dernier détenteur disparaît
std::shared_ptr<Player> joueur = std::make_shared<Player>();
std::shared_ptr<Player> autreRef = joueur;   // OK : les deux pointent vers le même objet, compteur = 2
// L'objet n'est détruit que quand TOUTES les références (joueur ET autreRef) sortent de scope
```

**Quand utiliser lequel :**
- **`unique_ptr` par défaut** — dès qu'un seul "propriétaire" logique existe (cas le plus fréquent : une entité de jeu, une ressource chargée une fois). Coût quasi nul (équivalent à un pointeur brut).
- **`shared_ptr` seulement si plusieurs parties du code doivent réellement co-posséder l'objet** (ex : plusieurs systèmes référencent la même texture et aucun n'est clairement responsable de sa durée de vie). Coût réel : compteur atomique thread-safe à chaque copie.
- **Ne jamais** utiliser `shared_ptr` par réflexe "au cas où" — c'est un signe fréquent d'architecture pas assez pensée.
- Un pointeur brut (`Type*`) reste légitime pour une **référence non-possédante** (ex : passer un pointeur pour observer un objet sans en être responsable) — dans ce cas, préférer même souvent une référence `Type&` si `nullptr` n'a pas de sens.

---

## 2. ORGANISATION ET ARCHITECTURE DU CODE

### 2.1 Séparation Header / Source

- **`.hpp` (header)** : déclare l'**interface** — signatures de fonctions, définitions de classes/structs, déclarations de variables (`extern`). Ce que les autres fichiers ont besoin de connaître.
- **`.cpp` (source)** : contient l'**implémentation** — le corps des fonctions/méthodes. Compilé une seule fois en un fichier objet.

```cpp
// player.hpp
#pragma once   // Include Guard moderne — équivalent plus simple et sans risque de typo que #ifndef

class Player {
public:
    Player(float x, float y);   // déclaration seulement
    void Update(float dt);
    void Draw() const;

private:
    float x_, y_;
};
```

```cpp
// player.cpp
#include "player.hpp"
#include "raylib.h"

Player::Player(float x, float y) : x_(x), y_(y) {}   // implémentation

void Player::Update(float dt) {
    if (IsKeyDown(KEY_RIGHT)) x_ += 200.0f * dt;
}

void Player::Draw() const {
    DrawRectangle((int)x_, (int)y_, 32, 32, RED);
}
```

**Include Guards — deux syntaxes équivalentes :**
```cpp
// Style historique, portable à 100%
#ifndef PLAYER_HPP
#define PLAYER_HPP
// ... contenu ...
#endif

// Style moderne, recommandé (supporté par tous les compilateurs actuels)
#pragma once
// ... contenu ...
```
Les deux évitent qu'un header soit inclus deux fois dans la même unité de compilation (ce qui provoquerait des erreurs de redéfinition). `#pragma once` est plus court et sans risque de collision de nom de macro entre deux fichiers `PLAYER_HPP` mal nommés dans des projets différents.

### 2.2 POO & Structures : `struct` vs `class`

**En C++, la SEULE différence technique entre `struct` et `class` est la visibilité par défaut :**

```cpp
struct Point {
    float x, y;    // public par défaut dans une struct
};

class Point2 {
    float x, y;    // private par défaut dans une class
public:
    Point2(float x, float y) : x(x), y(y) {}
};
```

**Convention (pas une règle du langage, mais largement respectée) :**
- `struct` : données simples, souvent sans logique complexe ni invariants à protéger (`Vector2`, `Rectangle`, données de config...)
- `class` : objets avec logique/état interne à protéger et encapsuler (`Player`, `GameEngine`, `ResourceManager`...)

**Encapsulation :**
```cpp
class CompteBancaire {
public:
    void Deposer(double montant) {
        if (montant > 0) solde_ += montant;   // logique de validation centralisée
    }
    double GetSolde() const { return solde_; }   // lecture seule depuis l'extérieur

private:
    double solde_ = 0.0;   // inaccessible directement depuis l'extérieur — protège l'invariant "solde cohérent"
};
```

**Constructeurs / Destructeurs et RAII (Resource Acquisition Is Initialization) :**

Le principe RAII est **central en C++ moderne** : une ressource (mémoire, fichier, handle GPU...) est acquise dans le constructeur et libérée automatiquement dans le destructeur — ainsi la libération est garantie même en cas d'exception.

```cpp
class FichierRAII {
public:
    explicit FichierRAII(const std::string& chemin) {
        fichier_ = fopen(chemin.c_str(), "r");   // acquisition de la ressource
    }
    ~FichierRAII() {
        if (fichier_) fclose(fichier_);          // libération GARANTIE, quoi qu'il arrive
    }

    // Règle des 5 : si tu définis un destructeur custom, pense à gérer aussi
    // copie/déplacement, sinon interdis-les explicitement pour éviter un double-fclose
    FichierRAII(const FichierRAII&) = delete;
    FichierRAII& operator=(const FichierRAII&) = delete;

private:
    FILE* fichier_ = nullptr;
};
```

### 2.3 Espaces de nommage (Namespaces)

```cpp
namespace Moteur {
    class Texture { /* ... */ };
    void Charger();
}

namespace Physique {
    class Texture { /* ... */ };   // aucune collision malgré le même nom !
}

// Utilisation explicite (recommandée dans les headers et le code général)
Moteur::Texture t1;
Physique::Texture t2;

// "using" ponctuel, limité à un scope précis (acceptable en .cpp, jamais dans un .hpp global)
void maFonction() {
    using namespace Moteur;
    Charger();   // équivaut à Moteur::Charger()
}
```

> **Règle stricte :** ne **jamais** écrire `using namespace std;` (ou tout autre namespace large) au niveau global d'un header — cela pollue l'espace de noms de **tous** les fichiers qui incluent ce header, provoquant des collisions imprévisibles dans de gros projets. Toujours préfixer (`std::vector`, `std::cout`) ou limiter le `using` à l'intérieur d'une fonction précise.

---

## 3. LA BIBLIOTHÈQUE STANDARD (STL) ESSENTIELLE

### 3.1 Conteneurs

**`std::vector`** — tableau dynamique, le conteneur par défaut à utiliser dans 90% des cas :
```cpp
#include <vector>

std::vector<int> nombres = {1, 2, 3};
nombres.push_back(4);           // ajoute à la fin — O(1) amorti
nombres.pop_back();              // retire le dernier élément
int premier = nombres[0];        // accès direct — AUCUNE vérification de borne (danger, voir section 4.3)
int securise = nombres.at(0);    // accès vérifié — lève std::out_of_range si invalide
size_t taille = nombres.size();
nombres.reserve(100);             // pré-alloue la capacité pour éviter des réallocations répétées
```

**`std::array`** — tableau de **taille fixe connue à la compilation**, alloué sur la pile (pas d'allocation dynamique) :
```cpp
#include <array>

std::array<int, 5> fixe = {1, 2, 3, 4, 5};   // taille figée à 5, ne peut pas grandir
int val = fixe[2];
size_t n = fixe.size();   // contrairement à un tableau C brut, .size() existe
```

**`std::unordered_map`** — table de hachage clé/valeur, accès moyen en O(1) :
```cpp
#include <unordered_map>

std::unordered_map<std::string, int> scores;
scores["Alice"] = 100;
scores["Bob"] = 85;

if (scores.find("Alice") != scores.end()) {   // vérifier l'existence d'une clé
    std::cout << scores["Alice"];
}
// scores.count("Alice") > 0   // alternative équivalente, plus lisible pour juste tester l'existence
```

| Conteneur | Taille | Allocation | Accès | Cas d'usage |
|---|---|---|---|---|
| `std::vector` | Dynamique | Heap | O(1) par index | Liste générique, cas par défaut |
| `std::array` | Fixe (compile-time) | Stack | O(1) par index | Taille connue et petite, perf maximale |
| `std::unordered_map` | Dynamique | Heap | O(1) moyen par clé | Recherche par identifiant/nom |

### 3.2 Parcours & Algorithmes

**Range-based for loop** (C++11+, à privilégier systématiquement sur les boucles indexées) :
```cpp
std::vector<int> nombres = {1, 2, 3, 4, 5};

for (int n : nombres) {              // copie de chaque élément
    std::cout << n << " ";
}

for (const int& n : nombres) {       // référence constante — évite la copie, pas de modif
    std::cout << n << " ";
}

for (int& n : nombres) {              // référence non-const — permet de MODIFIER en place
    n *= 2;                            // double chaque valeur du vector original
}
```

**`<algorithm>` — les fonctions les plus utilisées :**
```cpp
#include <algorithm>

std::vector<int> v = {5, 3, 1, 4, 2};

// std::sort : trie en place
std::sort(v.begin(), v.end());                    // ordre croissant : {1,2,3,4,5}
std::sort(v.begin(), v.end(), std::greater<>());  // ordre décroissant

// std::find : retourne un itérateur vers l'élément trouvé, ou .end() si absent
auto it = std::find(v.begin(), v.end(), 3);
if (it != v.end()) {
    std::cout << "Trouvé à l'index " << std::distance(v.begin(), it);
}

// Lambdas simples avec les algorithmes (très courant en C++ moderne)
int count = std::count_if(v.begin(), v.end(), [](int n) { return n > 2; });

std::sort(v.begin(), v.end(), [](int a, int b) {
    return a > b;   // lambda comme critère de tri personnalisé
});

// std::for_each : applique une fonction/lambda à chaque élément
std::for_each(v.begin(), v.end(), [](int& n) { n += 1; });
```

**Anatomie d'une lambda :**
```cpp
[capture](paramètres) -> type_retour { corps }

auto addition = [](int a, int b) { return a + b; };   // pas de capture
int externe = 10;
auto avecCapture = [externe](int a) { return a + externe; };  // capture par valeur
auto parReference = [&externe](int a) { externe += a; };       // capture par référence
auto captureTout = [=](int a) { return a + externe; };          // capture tout par valeur ([&] = tout par réf)
```

---

## 4. ERREURS CLASSIQUES, BUGS ET DÉBOGAGE

### 4.1 Oubli de point-virgule sur les structures/classes

```cpp
// ❌ FAUX — il manque le point-virgule final
struct Point {
    float x, y;
}   // <- provoque souvent une cascade d'erreurs de compilation sur les lignes SUIVANTES

int main() { return 0; }

// ✅ CORRECT
struct Point {
    float x, y;
};   // le ; est OBLIGATOIRE après l'accolade fermante d'une struct/class/enum
```
> Contrairement à une fonction (`void f() { ... }`, pas de `;` requis), toute définition de `struct`/`class`/`enum`/`union` doit se terminer par un point-virgule. C'est un vestige historique du C, où cette syntaxe permettait de déclarer une variable en même temps (`struct Point { ... } origine;`).

### 4.2 Inclusions circulaires et déclaration anticipée

```cpp
// ❌ PROBLÈME — a.hpp inclut b.hpp qui inclut a.hpp -> boucle infinie de préprocesseur
// a.hpp
#pragma once
#include "b.hpp"
class A { B* b; };

// b.hpp
#pragma once
#include "a.hpp"    // inclusion circulaire !
class B { A* a; };
```

**Solution : forward declaration** — quand on n'a besoin que d'un **pointeur ou d'une référence** vers un type (pas de sa définition complète), on peut le déclarer sans l'inclure :

```cpp
// a.hpp
#pragma once
class B;              // déclaration anticipée : "B existe, sa définition est ailleurs"

class A {
    B* b;              // OK : un pointeur ne nécessite pas la définition complète de B
};

// b.hpp
#pragma once
class A;

class B {
    A* a;
};
```

> **Règle :** la forward declaration suffit pour un pointeur/référence membre, ou un paramètre de fonction. Elle **ne suffit pas** si tu as besoin de la taille de l'objet (membre par valeur), d'appeler une de ses méthodes, ou d'hériter de la classe — dans ces cas, il faut le `#include` complet, généralement dans le `.cpp` plutôt que dans le `.hpp`.

### 4.3 Fuites de mémoire, dangling pointers, out-of-bounds

**Fuite mémoire (memory leak) :**
```cpp
void fuite() {
    int* p = new int(42);
    // pas de delete -> la mémoire n'est JAMAIS libérée tant que le programme tourne
}   // p (le pointeur lui-même) disparaît, mais PAS la mémoire qu'il pointait

// ✅ Solution moderne : smart pointer, libération garantie même en cas d'exception
void pasDeFuite() {
    auto p = std::make_unique<int>(42);
}   // libéré automatiquement ici
```

**Dangling pointer (pointeur pendouillant) :**
```cpp
int* dangereux() {
    int local = 42;
    return &local;      // ❌ retourne l'adresse d'une variable LOCALE
}                        // "local" est détruite à la sortie de la fonction

int* p = dangereux();
std::cout << *p;         // COMPORTEMENT INDÉFINI — p pointe vers une mémoire déjà libérée

// ✅ Solution : retourner par valeur (le compilateur optimise via RVO/move), jamais l'adresse d'une locale
int correct() {
    int local = 42;
    return local;         // copie/move sûr, aucune référence pendante
}
```

**Out-of-bounds (dépassement de borne) :**
```cpp
std::vector<int> v = {1, 2, 3};
int x = v[10];             // ❌ COMPORTEMENT INDÉFINI — aucune vérification avec operator[]
int y = v.at(10);          // ✅ lève std::out_of_range — détectable et catchable proprement
```

> **Astuce de debug :** compiler en debug avec les **AddressSanitizer** (`-fsanitize=address` avec g++/clang) détecte immédiatement dangling pointers, out-of-bounds et fuites mémoire à l'exécution, avec un message précis pointant la ligne fautive.

### 4.4 Erreurs de linkage courantes

```
undefined reference to `MaClasse::MaMethode()'
```

**Causes fréquentes :**
1. **Méthode déclarée dans le `.hpp` mais jamais implémentée dans le `.cpp`** — vérifier que chaque signature du header a bien son corps quelque part.
2. **Le fichier `.cpp` n'est pas inclus dans la compilation/le CMakeLists** — vérifier `add_executable`/`target_sources` en CMake, ou la liste de fichiers passée à g++.
3. **Mismatch entre déclaration et définition** (signature légèrement différente : `const` manquant, type de paramètre différent) → le linker cherche une signature exacte et ne trouve pas de correspondance.
4. **Ordre des bibliothèques statiques mal placé** en ligne de commande g++/clang (une lib qui dépend d'une autre doit être listée **avant** celle dont elle dépend, sur Linux/GCC).
5. **Fonction `main` définie deux fois**, ou définie dans un header inclus plusieurs fois sans être `inline`.

```cpp
// Erreur typique : définir une fonction non-inline directement dans un .hpp inclus par plusieurs .cpp
// header.hpp
void maFonction() { /* ... */ }   // ❌ "multiple definition" au linkage si inclus dans 2+ .cpp

// ✅ Solution : soit la déclarer seulement (implémentation en .cpp), soit la marquer inline
inline void maFonction() { /* ... */ }   // ✅ OK, autorisé dans plusieurs unités de compilation
```

---

## 5. BONS RÉFLEXES DE COMPILATION & TOOLING

### 5.1 Flags de compilation essentiels (g++ / clang++)

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 main.cpp -o programme
```

| Flag | Rôle |
|---|---|
| `-std=c++17` (ou `c++20`) | Fixe explicitement la version du standard — sans lui, le compilateur utilise une version par défaut potentiellement plus ancienne |
| `-Wall` | Active la plupart des avertissements utiles (variables non utilisées, comparaisons suspectes...) |
| `-Wextra` | Avertissements supplémentaires, plus stricts, non inclus dans `-Wall` |
| `-Wpedantic` | Signale les extensions non-standard utilisées (portabilité) |
| `-O2` | Optimisations du compilateur pour la vitesse d'exécution (à utiliser en Release, pas en debug) |
| `-O0` | Aucune optimisation — recommandé en développement/debug pour que le code exécuté corresponde exactement au code source |
| `-g` | Inclut les symboles de débogage (nécessaire pour utiliser gdb/lldb) |
| `-fsanitize=address` | Détecte fuites mémoire, dangling pointers, out-of-bounds à l'exécution (debug uniquement, désactiver en Release) |

**Configuration typique debug vs release :**
```bash
# Debug — priorité au diagnostic, pas à la vitesse
g++ -std=c++20 -Wall -Wextra -g -O0 -fsanitize=address main.cpp -o debug_build

# Release — priorité à la performance
g++ -std=c++20 -O2 -DNDEBUG main.cpp -o release_build
```

> **Règle non négociable :** traiter `-Wall -Wextra` comme des erreurs à corriger, pas des suggestions. La grande majorité des bugs C++ (variable non initialisée, comparaison signé/non-signé, shadowing de variable) sont détectés par ces flags **avant même l'exécution**.

### 5.2 Rôle de CMake / Makefile / scripts Bash

```
Bash script  →  automatise une SUITE de commandes shell (compilation, tests, déploiement)
Makefile     →  décrit des RÈGLES de dépendances entre fichiers (ne recompile que ce qui a changé)
CMake        →  GÉNÈRE un système de build (Makefile, Ninja, projet Visual Studio...) de façon portable
```

- **Bash/scripts** : pratique pour un tout petit projet ou pour enchaîner build + tests + packaging, mais ne gère pas la dépendance incrémentale (tout recompile à chaque fois, sauf logique ajoutée à la main).
- **Makefile** : reconstruit uniquement les fichiers `.cpp` modifiés depuis la dernière compilation (via horodatage) — plus rapide en itération, mais syntaxe spécifique à `make` et peu portable tel quel entre OS.
- **CMake** : **standard de facto** pour du C++ moderne multiplateforme — il ne compile rien lui-même, il **génère** la configuration adaptée à l'OS/IDE cible (Makefiles sous Linux, solution Visual Studio sous Windows, Xcode sous macOS...). Permet aussi de gérer proprement les dépendances externes (`FetchContent`, `find_package`).

```cmake
# CMakeLists.txt minimal mais complet
cmake_minimum_required(VERSION 3.20)
project(mon_projet CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)   # applique les flags à toutes les cibles

add_executable(mon_projet
    src/main.cpp
    src/player.cpp
)
target_include_directories(mon_projet PRIVATE include/)
```

```bash
# Utilisation typique de CMake, identique sur les 3 OS
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
```

> **Recommandation pour un projet qui grandit :** démarrer directement avec CMake, même pour un petit projet — la migration d'un Makefile artisanal vers CMake plus tard coûte plus cher que de bien structurer dès le départ, surtout si le projet doit un jour tourner sur plusieurs OS ou intégrer une dépendance externe (Raylib, une lib de tests, etc.).
