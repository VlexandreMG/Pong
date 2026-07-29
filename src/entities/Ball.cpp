#include "Ball.hpp"

Ball::Ball(int startX, int startY, float startSpeedX , float startSpeedY , int startRadius );
    x = startX;
    y = startY;
    SpeedX = startSpeedX;
    SpeedY = startSpeedY;
    radius = startRadius;

void update() {
    x += (int)SpeedX;
    y += (int)SpeedY;
}