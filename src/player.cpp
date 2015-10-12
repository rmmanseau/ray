#include "../headers/player.h"
#include "../headers/globals.h"
#include "../headers/world.h"

#include <cmath>

Player::Player() {
    init();
}

void Player::init() {
    pos = vec2d(glbl.startX, glbl.startY);
    // dir = vec2d(glbl.startDir, 0);
    // camPlane = vec2d(0, glbl.startPlane);
    dir = vec2d(1.0/std::tan(glbl.FOV*M_PI/360), 0);
    camPlane = vec2d(0, 1);
    normDir = vec2d(1, 0);
    thickness = 0.15;
}

void Player::move(World& world, vec2d displacement, vec2d padding) {
    vec2d newPos = pos + displacement;
    vec2d newPaddedPos = newPos + padding;

    if (world.map.charAt((int)newPaddedPos.x, (int)pos.y) == ' ') {
        pos.x = newPos.x;
    }
    if (world.map.charAt((int)pos.x, (int)newPaddedPos.y) == ' ') {
        pos.y = newPos.y;
    }
}

void Player::moveForward(World& world, double timeStep, double speed) {
    vec2d displacement = normDir * speed * timeStep;
    vec2d padding((normDir.x > 0 ? thickness : -thickness),
                  (normDir.y > 0 ? thickness : -thickness));
    move(world, displacement, padding);
}
void Player::moveBack(World& world, double timeStep, double speed) {
    vec2d displacement = normDir * speed * timeStep;
    vec2d padding((normDir.x > 0 ? thickness : -thickness),
                  (normDir.y > 0 ? thickness : -thickness));
    displacement = -displacement;
    padding = -padding;
    move(world, displacement, padding);
}
void Player::moveRight(World& world, double timeStep, double speed) {
    vec2d displacement = perpendicularTo(normDir * speed * timeStep);
    vec2d padding = perpendicularTo(vec2d((normDir.x > 0 ? thickness : -thickness),
                                         (normDir.y > 0 ? thickness : -thickness)));
    move(world, displacement, padding);
}
void Player::moveLeft(World& world, double timeStep, double speed) {
    vec2d displacement = perpendicularTo(normDir * speed * timeStep);
    vec2d padding = perpendicularTo(vec2d((normDir.x > 0 ? thickness : -thickness),
                                         (normDir.y > 0 ? thickness : -thickness)));
    displacement = -displacement;
    padding = -padding;
    move(world, displacement, padding);
}
void Player::rotate(double timeStep, double speed) {
    rotateVec2(dir, speed*timeStep);
    rotateVec2(normDir, speed*timeStep);
    rotateVec2(camPlane, speed*timeStep);
}
