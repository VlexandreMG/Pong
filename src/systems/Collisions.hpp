#pragma once 
#include "../entities/Ball.hpp"

class Collisions {
public:
    int point = 0;
    void detectCollision(Ball& ball , int screenHeight, int screenWidth);
};