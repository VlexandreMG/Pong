#include "Collisions.hpp"
#include "../entities/Ball.hpp"

void Collisions::detectCollision(Ball& thisBall, int screenHeight , int screenWidth) {
    if (thisBall.y + thisBall.radius >= screenHeight || thisBall.y - thisBall.radius <= 0) {
        thisBall.speedY = -(thisBall.speedY);
    } 
    
    if (thisBall.x + thisBall.radius >= screenWidth || thisBall.x - thisBall.radius <= 0) {
        thisBall.speedX = -(thisBall.speedX);
        Collisions::point += 1;
    }
}