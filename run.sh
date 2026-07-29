#!/bin/bash
g++ src/*.cpp src/entities/*.cpp -o src/mon_jeu -I src -I src/entities -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
src/mon_jeu