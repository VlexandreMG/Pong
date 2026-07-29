#pragma once 
#include "../entities/Ball.hpp"

class Collisions {
public:
    Ball ball;

    Collisions(Ball ball);

    void detectCollision(Ball ball , int screenHeight);
};