#pragma once 
#include "../entities/Ball.hpp"
#include "../entities/Line.hpp"

class Collisions {
public:
    int point = 0;
    void detectCollision(Ball& ball , int screenHeight, int screenWidth);
    void detectLineCollision(Ball& ball, Line& line);
};