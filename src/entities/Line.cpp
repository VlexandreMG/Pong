#include "Line.hpp"

void Line::detectLineCollision(Ball& ball, Line& line) {
    float halfThickness = line.thickness / 2.0f;
    float lineX = line.startPos.x;

    // Mi atteindre hauteur ambony be de tsy raharahina
    bool inYRange = (ball.y + ball.radius >= line.startPos.y) && 
    (ball.y - ball.radius <= line.endPos.y);

    if (inYRange) {
        // Impact venant de la GAUCHE de la ligne
        if (ball.speedX > 0 && 
            ball.x + ball.radius >= lineX - halfThick && 
            ball.x - ball.radius < lineX) 
        {
            ball.speedX = -ball.speedX; // Inverser la vitesse X
            ball.x = lineX - halfThick - ball.radius; // Replacer la balle hors du filet pour éviter qu'elle ne s'y coince
        }
        // Impact venant de la DROITE de la ligne
        else if (ball.speedX < 0 && 
                 ball.x - ball.radius <= lineX + halfThick && 
                 ball.x + ball.radius > lineX) 
        {
            ball.speedX = -ball.speedX; // Inverser la vitesse X
            ball.x = lineX + halfThick + ball.radius; // Replacer la balle hors du filet
        }
    }
}