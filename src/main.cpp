#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <deque>
#include <cmath>

#include "../headers/vec2.h"
#include "../headers/world.h"
#include "../headers/globals.h"
#include "../headers/imports.h"
#include "../headers/player.h"

Globals glbl;

template <typename T>
T sqr(T x) {
    return x*x;
}

/*
    Mouse control is weird and jumpy. Probably unfixable because
    SFML doesn't care to deal with it.
    Ended up being caused by spikes of lag that resulted in large timeSteps.
    Using average
*/
void playerInput(sf::RenderWindow &window, World& world, Player &player, double timeStep) {
    double moveSpeed = glbl.walkSpeed;
    double rotateSpeed = glbl.rotateSpeed;

    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
    //     glbl.crouch = glbl.winH/8;
    // }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        moveSpeed = glbl.runSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        player.moveForward(world, timeStep, moveSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        player.moveBack(world, timeStep, moveSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) &&
        (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
         glbl.mouseLocked)) {
        player.moveLeft(world, timeStep, moveSpeed);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        player.rotate(timeStep, -rotateSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
        (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
         glbl.mouseLocked)) {
        player.moveRight(world, timeStep, moveSpeed);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        player.rotate(timeStep, rotateSpeed);
    }

    sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);
    sf::Vector2i middle(glbl.winL/2, glbl.winH/2);
    if (glbl.mouseLocked && currentMousePos != middle) {
        rotateSpeed = (currentMousePos.x - middle.x) * 0.5;
        player.rotate(timeStep, rotateSpeed);
        sf::Mouse::setPosition(middle, window);

        double crouchOffset = (currentMousePos.y - middle.y) * 0.5;
        if ((glbl.crouch > -glbl.winH && crouchOffset > 0) ||
            (glbl.crouch < 2*glbl.winH && crouchOffset < 0)) {
            glbl.crouch -= crouchOffset;
        }
    }
}

double castRay(World &world, vec2d rayPos, vec2d rayDir, double &finalDist, int &side) {
    vec2i mapPos((int)rayPos.x, (int)rayPos.y);

    vec2d delta((double)sqrt(1 + sqr(rayDir.y) / sqr(rayDir.x)),
                (double)sqrt(1 + sqr(rayDir.x) / sqr(rayDir.y)));

    vec2d nextSide;
    vec2d step;

    int hit = 0;

    // find step direction and initial nextSide.
    if (rayDir.x < 0) {
        step.x = -1;
        nextSide.x = (rayPos.x - mapPos.x) * delta.x;
    }
    else {
        step.x = 1;
        nextSide.x = (mapPos.x + 1 - rayPos.x) * delta.x;
    }
    if (rayDir.y < 0) {
        step.y = -1;
        nextSide.y = (rayPos.y - mapPos.y) * delta.y;
    }
    else {
        step.y = 1;
        nextSide.y = (mapPos.y + 1 - rayPos.y) * delta.y;
    }

    while (hit == 0) {
        if (nextSide.x < nextSide.y) {
            nextSide.x += delta.x;
            mapPos.x += step.x;
            side = 0;
        }
        else
        {
            nextSide.y += delta.y;
            mapPos.y += step.y;
            side = 1;
        }
        if (world.map.charAt(mapPos.x, mapPos.y) != ' ') {
            hit = 1;
        }
    }

    if (side == 0) {
        finalDist = std::abs((mapPos.x - rayPos.x + (1 - step.x) / 2.0) / rayDir.x);
    }
    else {
        finalDist = std::abs((mapPos.y - rayPos.y + (1 - step.y) / 2.0) / rayDir.y);
    }
}

void drawColumn(sf::RenderWindow &window, double distance, int side, int column) {
    int height = std::abs(glbl.winH / distance);

    sf::RectangleShape bar;
    bar.setSize(sf::Vector2f(glbl.columnWidth, height));
    bar.setPosition(sf::Vector2f(column, glbl.crouch - height/2));

    int color = glbl.maxBrightness * glbl.renderDistance / sqr(distance);
    if (color > glbl.maxBrightness) {
        color = glbl.maxBrightness;
    }
    if (side == glbl.shadowSide) {
        color *= glbl.shadowDarkness;
    }

    bar.setFillColor(sf::Color(color, color, color));
    // bar.setFillColor(sf::Color(0, color/1.5, color));

    window.draw(bar);
}

void castRays(sf::RenderWindow &window, World& world, Player player) {
    
    for (int col = 0; col < glbl.winL; col += glbl.columnWidth) {
        double camColumn = 2*col / double(glbl.winL) - 1;
        
        vec2d rayPos = player.pos;
        vec2d rayDir(player.dir.x + player.camPlane.x * camColumn,
                     player.dir.y + player.camPlane.y * camColumn);

        double distance;
        int side;
        
        castRay(world, rayPos, rayDir, distance, side);
        drawColumn(window, distance, side, col);
    }
}

void drawGround(sf::RenderWindow &window) {
    sf::RectangleShape ground;
    ground.setSize(sf::Vector2f(glbl.winL, glbl.winH*2.5));
    ground.setPosition(sf::Vector2f(0, glbl.crouch));
    ground.setFillColor(sf::Color(70, 130, 60));

    window.draw(ground);
}

void drawCrossheir(sf::RenderWindow &window) {
    double size = 2;
    sf::Vector2f center(glbl.winL/2.0 - size/2.0, glbl.winH/2.0 - size/2.0);

    sf::RectangleShape dot;
    dot.setFillColor(sf::Color::Black);
    dot.setSize(sf::Vector2f(size, size));
    dot.setPosition(center);
    
    dot.move(-size, 0);
    window.draw(dot);
    dot.move(size, -size);
    window.draw(dot);
    dot.move(size, size);
    window.draw(dot);
    dot.move(-size, size);
    window.draw(dot);
}

void handleEvents(sf::RenderWindow &window, sf::Event &event, World& world, Player& player) {
    while(window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::G) {
                window.setMouseCursorVisible(glbl.mouseLocked);
                glbl.mouseLocked = !glbl.mouseLocked;
            }
            if (event.key.code == sf::Keyboard::Num1) {
                importGlobals();
            }
            if (event.key.code == sf::Keyboard::Num2) {
                player.init();
                world.switchToMap(glbl.selectedMap);
            }
        }
    }
}

/*
    Deals with tiny lagspikes that caused jitters in rotation. 
    Using an average timeStep over the last few frames remains
    decently accurate to the actual timeStep, while dampening
    any spikes.
*/
double averageStep(std::deque<double> &lastFrames, double timeStep) {
    lastFrames.push_front(timeStep);
    
    if (lastFrames.size() > 20)
        lastFrames.pop_back();
    
    double average = 0;
    for (int i = 0; i < lastFrames.size(); ++i)
        average += lastFrames[i];

    average /= lastFrames.size();
    return average;
}

int main()
{    
    /*
    16:9
    1920x1080
    1600x900
    1280x720
    960x540

    4:3
    960x720
    */

    glbl.winL = 960;
    glbl.winH = 540;
    glbl.mouseLocked = false;
    glbl.crouch = glbl.winH/2;
    sf::RenderWindow window(sf::VideoMode(glbl.winL, glbl.winH, 32),
                            "SFML Raycasting!", 
                            sf::Style::Close);

    importGlobals();
    Player player;
    // initPlayer(player);
    
    World world;
    importMaps(world);
    world.switchToMap(glbl.selectedMap);
    
    sf::Clock gameClock;
    std::deque<double> timeSteps;
    sf::Event event;

    while (window.isOpen())
    {   
        handleEvents(window, event, world, player);
        double timeStep = averageStep(timeSteps, gameClock.restart().asSeconds());
        playerInput(window, world, player, timeStep);
        window.clear(sf::Color(135, 206, 235));
        drawGround(window);
        castRays(window, world, player);
        drawCrossheir(window);
        window.display();
    }
}
