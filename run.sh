#!/bin/bash

# 1. On recherche dynamiquement TOUS les fichiers .cpp sous src/
CPP_FILES=$(find src -name "*.cpp")

# 2. On recherche tous les dossiers contenant des .hpp pour les inclure (-I)
INCLUDE_FLAGS=$(find src -type d -exec echo -I{} +)

# 3. Compilation
g++ $CPP_FILES -o src/mon_jeu $INCLUDE_FLAGS -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# 4. Exécution si la compilation a réussi
if [ $? -eq 0 ]; then
    ./src/mon_jeu
else
    echo "Erreur de compilation !"
fi