#pragma once 
#include "raylib.h"

class Line {
public:
    Vector2 startPos;
    Vector2 endPos;
    float thickness;
    Color color;

    void moveY(float speed) {
        startPos.y += speed;
        endPos.y += speed;
    }
};