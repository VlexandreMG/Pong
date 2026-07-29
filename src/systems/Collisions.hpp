#pragma once 
#include "Ball.hpp";

class Collisions {
public:
    Ball ball;

    Collisions(Ball ball);

    void collision();
};