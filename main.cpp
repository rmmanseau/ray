#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

/*
    16:9
    1920x1080
    1600x900
    1280x720
    960x540

    4:3
    960x720
*/

std::vector<std::string> worlds;
std::string world;
int worldL;
int worldH;

namespace C {
    int winL = 960;
    int winH = 540;

    int columnWidth = 1;

    double walkSpeed = 0.02;
    double runSpeed = 0.05;
    double rotateSpeed = 0.016;

    double startX = 1.5;
    double startY = 1.5;

    double startDir = 1;
    double startPlane = 0.88;
    
    int maxBrightness = 75; // Out of 255
    int renderDistance = 20;

    int shadowSide = 0;
    double shadowDarkness = 0.7;

    int selectedWorld = 0;
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

void findWorldDimensions() {
    worldL = 1;
    for (int i = 0; world[i] != '\n'; i++) {
        worldL++;
    }
    
    worldH = world.length() / worldL;
}

char charAt(int x, int y) {
    return world[y * worldL + x];
}

void initPlayer(Player &player) {
    player.pos = vec2d(C::startX, C::startY);
    player.dir = vec2d(C::startDir, 0);
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

    C::selectedWorld = vars[12];
}

void importMaps(std::vector<std::string> &worlds)
{
    std::ifstream mapFile;

    char c;
    char prev;

    mapFile.open("worlds.dat");
    while (mapFile.get(c))
    {
        if (c == '%')
        {
            int baseLength = -1;
            int length = 0;
            std::stringstream world_in;
            std::string world;

            mapFile >> std::ws;

            while (mapFile.get(c))
            {
                if (c == '%')
                    break;
                else {
                    ++length;
                    world_in << c;
                }

                if (c == '\n')
                {
                    if (baseLength == -1)
                    {
                        baseLength = length;
                        length = 0;
                    }
                    else
                    {
                        if (length != baseLength)
                        {
                            std::cout << "ERROR: rows are not all the same length";
                            if (c == '\n' && prev == '\n')
                                std::cout << " (did you forget \% at the end?)\n";
                            else
                                std::cout << '\n';
                            exit(1);
                        }

                        length = 0;
                    }
                }

                prev = c;
            }

            world = world_in.str();
            worlds.push_back(world);
        }
    }
    mapFile.close(); 
}

void initWorld() {
    importMaps(worlds);
    world = worlds[C::selectedWorld];
    findWorldDimensions();
}

template <typename T>
void rotate(vec2<T> &vec, double rotateSpeed)
{
    double oldX = vec.x;
    vec.x = oldX * cos(rotateSpeed) - vec.y * sin(rotateSpeed);
    vec.y = oldX * sin(rotateSpeed) + vec.y * cos(rotateSpeed);
}

void move(Player &player, vec2d newPos, vec2d padding) {
    if (charAt((int)padding.x, (int)player.pos.y) == ' ') {
        player.pos.x = newPos.x;
    }
    if (charAt((int)player.pos.x, (int)padding.y) == ' ') {
        player.pos.y = newPos.y;
    }
}

void playerInput(Player &player) {
    double moveSpeed = C::walkSpeed;
    double rotateSpeed = C::rotateSpeed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
        importVariables();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
        initPlayer(player);
        initWorld();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        moveSpeed = C::runSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        vec2d newPos(player.pos.x + player.dir.x * moveSpeed,
                     player.pos.y + player.dir.y * moveSpeed);
        vec2d padding(newPos.x + (player.dir.x > 0 ? 0.15 : -0.15),
                      newPos.y + (player.dir.y > 0 ? 0.15 : -0.15));

        move(player, newPos, padding);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        vec2d newPos(player.pos.x - player.dir.x * moveSpeed,
                     player.pos.y - player.dir.y * moveSpeed);
        vec2d padding(newPos.x - (player.dir.x > 0 ? 0.15 : -0.15),
                      newPos.y - (player.dir.y > 0 ? 0.15 : -0.15));
        move(player, newPos, padding);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        vec2d newPos(player.pos.x + player.dir.y * moveSpeed,
                     player.pos.y - player.dir.x * moveSpeed);
        vec2d padding(newPos.x + (player.dir.y > 0 ? 0.15 : -0.15),
                      newPos.y - (player.dir.x > 0 ? 0.15 : -0.15));
        move(player, newPos, padding);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        rotate(player.dir, -rotateSpeed);
        rotate(player.camPlane, -rotateSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        vec2d newPos(player.pos.x - player.dir.y * moveSpeed,
                     player.pos.y + player.dir.x * moveSpeed);
        vec2d padding(newPos.x - (player.dir.y > 0 ? 0.15 :  -0.15),
                      newPos.y + (player.dir.x > 0 ? 0.15 :  -0.15));
        move(player, newPos, padding);
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        rotate(player.dir, rotateSpeed);
        rotate(player.camPlane, rotateSpeed);
    }
}

double castRay(vec2d rayPos, vec2d rayDir, double &finalDist, int &side) {
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
        if (charAt(mapPos.x, mapPos.y) != ' ') {
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
    // bar.setFillColor(sf::Color(0, color/1.5, color));

    window.draw(bar);
}

void castRays(sf::RenderWindow &window, Player player) {
    
    for (int col = 0; col < C::winL; col += C::columnWidth) {
        double camColumn = 2*col / double(C::winL) - 1;
        
        vec2d rayPos = player.pos;
        vec2d rayDir(player.dir.x + player.camPlane.x * camColumn,
                     player.dir.y + player.camPlane.y * camColumn);

        double distance;
        int side;
        
        castRay(rayPos, rayDir, distance, side);
        drawColumn(window, distance, side, col);
    }
}

void drawGround(sf::RenderWindow &window) {
    sf::RectangleShape ground;
    ground.setSize(sf::Vector2f(C::winL, C::winH/2));
    ground.setPosition(sf::Vector2f(0, C::winH/2));
    ground.setFillColor(sf::Color(70, 130, 60));

    window.draw(ground);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(C::winL, C::winH, 32),
                            "SFML Raycasting!", 
                            sf::Style::Close);

    importVariables();
    Player player;
    initPlayer(player);
    initWorld();

    for(;;)
    {   
        playerInput(player);
        window.clear(sf::Color(135, 206, 235));
        drawGround(window);
        castRays(window, player);
        window.display();
    }
}
