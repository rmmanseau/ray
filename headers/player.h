#ifndef PLAYER_H
#define PLAYER_H

#include "vec2.h"
class World;

class Player {
    void move(World& world, vec2d displacement, vec2d padding);

public:
    vec2d pos;
    vec2d dir;
    vec2d camPlane;
    double thickness;

    Player();
    void init();

    void moveForward(World& world, double timeStep, double speed);
    void moveBack(World& world, double timeStep, double speed);
    void moveLeft(World& world, double timeStep, double speed);
    void moveRight(World& world, double timeStep, double speed);
    void rotate(double timeStep, double speed);
};

#endif // PLAYER_H
