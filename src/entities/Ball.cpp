#include "Ball.hpp"

Ball::Ball(int startX, int startY, float startSpeedX , float startSpeedY , int startRadius ) {
    x = startX;
    y = startY;
    speedX = startSpeedX;
    speedY = startSpeedY;
    radius = startRadius;
}

void Ball::update() {
    x += speedX;
    y += speedY;
}