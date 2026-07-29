#pragma once

class Ball {
public:
    int x;
    int y;
    float speedY;
    float speedX;
    int radius;

    Ball(int startX, int startY, float startSpeedX , float startSpeedY , int startRadius );

    void update();
};