#pragma once 
#include "raylib.h"

class Line {
public:
    Vector2 startPos;
    Vector2 endPos;
    float thickness;
    Color color;
    float speed;

    void moveY(float amount) {
        startPos.y += amount;
        endPos.y += amount;
    }
};