#include "Collisions.hpp"
#include "Ball.hpp"

Collisions::Collisions(Ball thisBall) {
    ball = thisBall;
}

void Collisions::collision(Ball& thisBall, int screenWidth) {
    if (thisBall.y + thisBall.radius >= screenWidth) {
        thisBall.speedX = thisBall.speedX;
        thisBall.speedY = -(thisBall.speedY);
    } 
}