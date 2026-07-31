#pragma once 
#include "../entities/Ball.hpp"

class Collisions {
public:
    void detectCollision(Ball ball , int screenHeight, int screenWidth);
};