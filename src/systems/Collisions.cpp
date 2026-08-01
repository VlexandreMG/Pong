#include "Collisions.hpp"
#include "../entities/Ball.hpp"
#include "../entities/Line.hpp"

void Collisions::detectCollision(Ball& thisBall, int screenHeight , int screenWidth) {
    if (thisBall.y + thisBall.radius >= screenHeight || thisBall.y - thisBall.radius <= 0) {
        thisBall.speedY = -(thisBall.speedY);
    } 
    
    if (thisBall.x + thisBall.radius >= screenWidth || thisBall.x - thisBall.radius <= 0) {
        
        if (isGameOver) return;

        if (Collisions::point == 3) {
            isGameOver = true;
        } else {
            thisBall.speedX = -(thisBall.speedX);
            Collisions::point += 1;
            thisBall.x = screenWidth / 2;
            thisBall.y = screenHeight / 2;
        }
    
    }
}

void Collisions::detectLineCollision(Ball& ball, Line& line) {
    float halfThick = line.thickness / 2.0f;
    float lineX = line.startPos.x;

    bool inRange = (ball.y + ball.radius >= line.startPos.y) &&
    (ball.y - ball.radius <= line.endPos.y); 

    if (inRange) {
        if (ball.x - ball.radius <= lineX + halfThick && 
                 ball.x + ball.radius > lineX) 
        {
            ball.speedX = -ball.speedX; // Inverser la vitesse X
        }
    }
        
}