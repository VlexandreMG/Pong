#include "Collisions.hpp"
#include "../entities/Ball.hpp"

void Collisions::detectCollision(Ball& thisBall, int screenHeight , int screenWidth) {
    if (thisBall.y + thisBall.radius >= screenHeight) {
        thisBall.speedY = -(thisBall.speedY);
    } 
    
    if (thisBall.x + thisBall.radius >= screenWidth) {
        thisBall.speedX = -(thisBall.speedX);
    }
}