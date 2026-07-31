#include "Collisions.hpp"
#include "../entities/Ball.hpp"

Collisions::Collisions(Ball thisBall) {
    ball = thisBall;
}

void Collisions::detectCollision(Ball& thisBall, int screenHeight , int screenWidth) {
    if (thisBall.y + thisBall.radius >= screenHeight) {
        thisBall.speedX = thisBall.speedX;
        thisBall.speedY = -(thisBall.speedY);
    } else if (thisBall.x + thisBall.radius >= screenWidth) {
        thisBall.speedX = thisBall.speedX;
        thisBall.speedY = -(thisBall.speedY);
    }
}