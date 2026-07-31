#pragma once 
#include "../entities/Ball.hpp"

class Collisions {
public:
    int point;
    void detectCollision(Ball& ball , int screenHeight, int screenWidth);
};