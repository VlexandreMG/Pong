#pragma once 
#include "raylib.h"

class Line {
public:
    Vector2 startPos;
    Vector2 endPos;
    float thickness;
    Color color;

    void detectLineCollision(Ball& ball, Line& line);
};