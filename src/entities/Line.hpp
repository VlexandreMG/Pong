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
        
        float lineLength = endPos.y - startPos.y;
        
        startPos.y += amount;
        endPos.y += amount;

        if (startPos.y < 0.0f) {
            startPos.y = 0.0f;
            endPos.y = startPos.y + lineLength;
        }
    }
};