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
sf::Texture wolfTextures;


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

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad9)) {
        player.camPlane *= 1.001;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad8)) {
        player.camPlane *= 0.999;
    }    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad6)) {
        player.dir *= 1.001;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad5)) {
        player.dir *= 0.999;
    }    

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
    sf::Vector2i middle(glbl.winW/2, glbl.winH/2);
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

double pythagorian(double a, double b) {
    return (double)sqrt(sqr(a) + sqr(b));
}

double castRay(World &world, vec2d rayPos, vec2d rayDir, double &finalDist, double &wallPos, int &side, char &wallType) {
    vec2i mapPos((int)rayPos.x, (int)rayPos.y);

    double scaledY = rayDir.y/rayDir.x;
    double scaledX = rayDir.x/rayDir.y;

    vec2d delta(pythagorian(1, scaledY),
                pythagorian(1, scaledX));

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
        wallType = world.map.charAt(mapPos.x, mapPos.y);
        if (wallType != ' ') {
            hit = 1;
        }
    }

    if (side == 0) {
        finalDist = std::abs((mapPos.x - rayPos.x + (1 - step.x) / 2.0) / rayDir.x);
        wallPos = rayPos.y + ((mapPos.x - rayPos.x + (1 - step.x) / 2) / rayDir.x) * rayDir.y;
    }
    else {
        finalDist = std::abs((mapPos.y - rayPos.y + (1 - step.y) / 2.0) / rayDir.y);
        wallPos = rayPos.x + ((mapPos.y - rayPos.y + (1 - step.y) / 2) / rayDir.y) * rayDir.x;
    }

    wallPos -= floor(wallPos);
}

void drawColumn(sf::RenderWindow &window, double distance, int side, int column, double wallPos, char wallType) {
    
    int textureNum = (int)(wallType - '0');

    int texX = (int)(wallPos * (double)glbl.texW);
    int height = std::abs(glbl.winH / distance);

    sf::Sprite bar(wolfTextures, sf::IntRect((textureNum*glbl.texW)+texX,0,1,glbl.texH));
    bar.scale(sf::Vector2f(glbl.columnWidth, (double)glbl.winH/(glbl.texH*distance)));
    bar.setPosition(sf::Vector2f(column, glbl.crouch - height/2));

    if (side == 1) {
        bar.setColor(sf::Color(165, 165, 165));
    }

    window.draw(bar);
}

void castRays(sf::RenderWindow &window, World& world, Player player) {
    
    for (int col = 0; col < glbl.winW; col += glbl.columnWidth) {
        double camColumn = 2*col / (double)glbl.winW - 1;
        
        vec2d rayPos = player.pos;
        vec2d rayDir(player.dir.x + player.camPlane.x * camColumn,
                     player.dir.y + player.camPlane.y * camColumn);

        double distance;
        double wallPos;
        int side;
        char wallType;
        
        castRay(world, rayPos, rayDir, distance, wallPos, side, wallType);
        drawColumn(window, distance, side, col, wallPos, wallType);
    }
}

void drawGround(sf::RenderWindow &window) {
    sf::RectangleShape ground;
    ground.setSize(sf::Vector2f(glbl.winW, glbl.winH*2.5));
    ground.setPosition(sf::Vector2f(0, glbl.crouch));
    ground.setFillColor(sf::Color(70, 130, 60));

    window.draw(ground);
}

void drawCrossheir(sf::RenderWindow &window) {
    double size = 2;
    sf::Vector2f center(glbl.winW/2.0 - size/2.0, glbl.winH/2.0 - size/2.0);

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

    glbl.winW = 960;
    glbl.winH = 540;
    glbl.texW = 8;
    glbl.texH = 8;
    glbl.mouseLocked = false;
    glbl.crouch = glbl.winH/2;
    sf::RenderWindow window(sf::VideoMode(glbl.winW, glbl.winH, 32),
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

    wolfTextures.loadFromFile("../assets/textures/16bittextures.png");

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
