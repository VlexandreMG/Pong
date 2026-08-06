#pragma once 
#include "Line.hpp"

class BotLine : public Line {
public: 
    int direction = 1;
    
    BotLine(Vector2 start , Vecto2 finish , float thick , Color couleur , float vitesse) {
        startPos = start;
        endPos = finish;
        thickness = thick;
        color = couleur;
        speed = vitesse;
    }

    void update (int screenHeight) {
        float lineLength = endPos.y - startPos.y;
        
        startPos.y += speed * direction;
        endPos.y += speed * direction;

        if (startPos.y < 0.0f) {
            startPos.y = 0.0f;
            endPos.y = startPos.y + lineLength;
            direction = 1;
        }

        if (endPos.y > screenHeight) {
            endPos.y = screenHeight;
            startPos.y = endPos.y - lineLength;
            direction = -1;
        }
    }
}