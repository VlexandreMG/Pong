#include "Collisions.hpp"
#include "../entities/Ball.hpp"

Collisions::Collisions(Ball thisBall) {
    ball = thisBall;
}

void Collisions::detectCollision(Ball& thisBall, int screenHeight) {
    if (thisBall.y + thisBall.radius >= screenHeight) {
        thisBall.speedX = thisBall.speedX;
        thisBall.speedY = -(thisBall.speedY);
    } 
}