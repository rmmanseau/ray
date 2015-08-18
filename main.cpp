#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <iostream>
#include <cmath>

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


void playerInput(Player &player) {
    double moveSpeed = 0.01;
    double rotateSpeed = 0.005;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        moveSpeed = 0.03;
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

double castRay(vec2d rayPos, vec2d rayDir, int map[][20], double &finalDist, int side) {
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

    if (side == 0)
        finalDist = std::abs((mapPos.x - rayPos.x + (1 - step.x) / 2.0) / rayDir.x);
    else
        finalDist = std::abs((mapPos.y - rayPos.y + (1 - step.y) / 2.0) / rayDir.y);
}

void drawColumn(sf::RenderWindow &window, int winH, double distance, int side, int column) {
    int height = std::abs(winH / distance);

    sf::RectangleShape bar;
    bar.setSize(sf::Vector2f(1, height));
    bar.setPosition(sf::Vector2f(column, winH/2 - height/2));

    int color = 500 / distance;
    if (color > 200) {
        color = 200;
    }
    if (side = 1) {
        color * 0.8;
    }

    bar.setFillColor(sf::Color(color, color, color));

    window.draw(bar);
}

void castRays(sf::RenderWindow &window, int winL, int winH, Player player, int map[][20]) {
    
    for (int col = 0; col < winL; ++col) {
        double camColumn = 2*col / double(winL) - 1;
        
        vec2d rayPos = player.pos;
        vec2d rayDir(player.dir.x + player.camPlane.x * camColumn,
                     player.dir.y + player.camPlane.y * camColumn);

        double distance;
        double side;
        
        castRay(rayPos, rayDir, map, distance, side);
        drawColumn(window, winH, distance, side, col);
    }
}


int main()
{
    const int winL = 800;
    const int winH = 600;
    sf::RenderWindow window(sf::VideoMode(winL, winH, 32),
                            "SFML Raycasting!", 
                            sf::Style::Close);

    int map[20][20] =
    {
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
      {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
      {1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
      {1,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1},
      {1,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,1},
      {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1},
      {1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    Player player;
    player.pos = vec2d(10, 10);
    player.dir = vec2d(-1, 0);
    player.camPlane = vec2d(0, 0.66);

    for(;;)
    {   
        playerInput(player);

        window.clear(sf::Color(20, 20, 20));

        castRays(window, winL, winH, player, map);
        
        window.display();
    }
}