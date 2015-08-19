#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

namespace C {
    int winL = 960;
    int winH = 540;

    int columnWidth = 1;

    double walkSpeed = 0.02;
    double runSpeed = 0.05;
    double rotateSpeed = 0.016;

    double startX = 15;
    double startY = 10;

    double startDir = 1;
    double startPlane = 0.88;
    
    int maxBrightness = 75; // Out of 255
    int renderDistance = 20;

    int shadowSide = 0;
    double shadowDarkness = 0.7;
};

template <class T>
struct vec2
{
  T x;
  T y;

  vec2(T _x, T _y) : x(_x), y(_y) {}
  vec2() : x(0), y(0) {}
};

typedef vec2<double> vec2d;
typedef vec2<int> vec2i;

struct Player {
    vec2d pos;
    vec2d dir;
    vec2d camPlane;
};

template <typename T>
T sqr(T x) {
    return x*x;
}

template <typename T>
void rotate(vec2<T> &vec, double rotateSpeed)
{
    double oldX = vec.x;
    vec.x = oldX * cos(rotateSpeed) - vec.y * sin(rotateSpeed);
    vec.y = oldX * sin(rotateSpeed) + vec.y * cos(rotateSpeed);
}

void initPlayer(Player &player) {
    player.pos = vec2d(C::startX, C::startY);
    player.dir = vec2d(-C::startDir, 0);
    player.camPlane = vec2d(0, C::startPlane);
}

void importVariables()
{
    std::ifstream varFile;
    varFile.open("config.dat");
    
    std::string tmp;

    std::vector<double> vars;

    while (getline(varFile, tmp, ':'))
    {
        double currentVar;

        varFile >> currentVar;
        vars.push_back(currentVar);
    }

    varFile.close();

    C::columnWidth = vars[0];

    C::walkSpeed = vars[1];
    C::runSpeed = vars[2];
    C::rotateSpeed = vars[3];

    C::startX = vars[4];
    C::startY = vars[5];

    C::startDir = vars[6];
    C::startPlane = vars[7];

    C::maxBrightness = vars[8];
    C::renderDistance = vars[9];

    C::shadowSide = vars[10];
    C::shadowDarkness = vars[11];
}

void playerInput(Player &player) {
    double moveSpeed = C::walkSpeed;
    double rotateSpeed = C::rotateSpeed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
        importVariables();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
        initPlayer(player);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        moveSpeed = C::runSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        player.pos.x += player.dir.x * moveSpeed;
        player.pos.y += player.dir.y * moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        player.pos.x -= player.dir.x * moveSpeed;
        player.pos.y -= player.dir.y * moveSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        player.pos.x += player.dir.y * moveSpeed;
        player.pos.y -= player.dir.x * moveSpeed;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        rotate(player.dir, -rotateSpeed);
        rotate(player.camPlane, -rotateSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        player.pos.x -= player.dir.y * moveSpeed;
        player.pos.y += player.dir.x * moveSpeed;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        rotate(player.dir, rotateSpeed);
        rotate(player.camPlane, rotateSpeed);
    }
}

double castRay(vec2d rayPos, vec2d rayDir, int map[][20], double &finalDist, int &side) {
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
        
        hit = map[mapPos.x][mapPos.y];
    }

    if (side == 0) {
        finalDist = std::abs((mapPos.x - rayPos.x + (1 - step.x) / 2.0) / rayDir.x);
    }
    else {
        finalDist = std::abs((mapPos.y - rayPos.y + (1 - step.y) / 2.0) / rayDir.y);
    }
}

void drawColumn(sf::RenderWindow &window, double distance, int side, int column) {
    int height = std::abs(C::winH / distance);

    sf::RectangleShape bar;
    bar.setSize(sf::Vector2f(C::columnWidth, height));
    bar.setPosition(sf::Vector2f(column, C::winH/2 - height/2));

    int color = C::maxBrightness * C::renderDistance / sqr(distance);
    if (color > C::maxBrightness) {
        color = C::maxBrightness;
    }
    if (side == C::shadowSide) {
        color *= C::shadowDarkness;
    }

    bar.setFillColor(sf::Color(color, color, color));

    window.draw(bar);
}

void castRays(sf::RenderWindow &window, Player player, int map[][20]) {
    
    for (int col = 0; col < C::winL; col += C::columnWidth) {
        double camColumn = 2*col / double(C::winL) - 1;
        
        vec2d rayPos = player.pos;
        vec2d rayDir(player.dir.x + player.camPlane.x * camColumn,
                     player.dir.y + player.camPlane.y * camColumn);

        double distance;
        int side;
        
        castRay(rayPos, rayDir, map, distance, side);
        drawColumn(window, distance, side, col);
    }
}


int main()
{
    sf::RenderWindow window(sf::VideoMode(C::winL, C::winH, 32),
                            "SFML Raycasting!", 
                            sf::Style::Close);

    int map[20][20] =
    {
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
      {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
      {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };


    Player player;
    initPlayer(player);

    for(;;)
    {   
        playerInput(player);

        window.clear(sf::Color(0, 0, 0));

        castRays(window, player, map);
        
        window.display();
    }
}
