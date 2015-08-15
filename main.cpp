#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <iostream>
#include <cmath>

struct vec2d
{
    double x;
    double y;

    vec2d(double _x, double _y) : x(_x), y(_y) {}
    vec2d() : x(0), y(0) {}
};

struct vec2i
{
    int x;
    int y;

    vec2i(int _x, int _y) : x(_x), y(_y) {}
    vec2i() : x(0), y(0) {}
};

template <typename T>
T sqr(T x) {
    return x*x;
}

void rotate(vec2d &vec, double rotateSpeed)
{
    double oldX = vec.x;
    vec.x = oldX * cos(rotateSpeed) - vec.y * sin(rotateSpeed);
    vec.y = oldX * sin(rotateSpeed) + vec.y * cos(rotateSpeed);
}

int main()
{
    const int win_length = 800;
    const int win_height = 600;
    sf::RenderWindow window(sf::VideoMode(win_length, win_height, 32),
                            "SFML Raycasting!", 
                            sf::Style::Close);

    int world[10][10] =
    {
      {1,1,1,1,1,1,1,1,1,1},
      {1,0,0,0,0,0,0,0,0,1},
      {1,0,1,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,1},
      {1,1,1,1,1,1,1,1,1,1},
    };

    vec2d pos(8, 8);
    vec2d dir(-1, 0);
    vec2d plane(0, 0.66);

    double curTime = 0;
    double oldTime = 0;

    for(;;)
    {
        double cameraX;
        vec2d rayPos;
        vec2d rayDir;

        window.clear(sf::Color::Black);

        double moveSpeed = 0.01;
        double rotateSpeed = 0.005;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            pos.x += dir.x * moveSpeed;
            pos.y += dir.y * moveSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            pos.x -= dir.x * moveSpeed;
            pos.y -= dir.y * moveSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            rotate(dir, -rotateSpeed);
            rotate(plane, -rotateSpeed);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            rotate(dir, rotateSpeed);
            rotate(plane, rotateSpeed);
        }

        for(int x = 0; x < win_length; ++x)
        {
            cameraX = 2*x / (2.0*win_length) - 1;
            rayPos = pos;
            rayDir.x = dir.x + plane.x * cameraX;
            rayDir.y = dir.y + plane.y * cameraX;

            //which box of map we are in
            vec2i map((int)rayPos.x, (int)rayPos.y);

            //length of ray from current pos to next x or y side
            vec2d sideDist;

            //length of ray from one x or y side to next
            vec2d deltaDist;
            deltaDist.x = (double)sqrt(1 + sqr(rayDir.y) / sqr(rayDir.x));
            deltaDist.y = (double)sqrt(1 + sqr(rayDir.x) / sqr(rayDir.y));
            double perpWallDist;

            //what direction to step in x or y (+1 or -1);
            vec2i step;

            int hit = 0; //was a wall hit?
            int side; //was a NS or a EW wall hit?

            //calculate step and initial sideDist
            if (rayDir.x < 0) {
                step.x = -1;
                sideDist.x = (rayPos.x - map.x) * deltaDist.x;
            }
            else {
                step.x = 1;
                sideDist.x = (map.x + 1.0 - rayPos.x) * deltaDist.x;
            }
            if (rayDir.y < 0) {
                step.y = -1;
                sideDist.y = (rayPos.y - map.y) * deltaDist.y;
            }
            else {
                step.y = 1;
                sideDist.y = (map.y + 1.0 - rayPos.y) * deltaDist.y;
            }

            //perform DDA
            while (hit == 0)
            {
                //jump to next map sqaure
                if (sideDist.x < sideDist.y)
                {
                    sideDist.x += deltaDist.x;
                    map.x += step.x;
                    side = 0;
                }
                else
                {
                    sideDist.y += deltaDist.y;
                    map.y += step.y;
                    side = 1;
                }
                if (world[map.x][map.y] > 0)
                    hit = 1;
            }

            if (side == 0)
                perpWallDist = std::abs((map.x - rayPos.x + (1 - step.x) / 2.0) / rayDir.x);
            else
                perpWallDist = std::abs((map.y - rayPos.y + (1 - step.y) / 2.0) / rayDir.y);

            std::cout << "X: d: " << dir.x << " p: " << plane.x << std::endl;
            std::cout << "Y: d: " << dir.y << " p: " << plane.y << std::endl;

            int lineHeight = abs((win_height / perpWallDist));

            sf::RectangleShape line;
            line.setSize(sf::Vector2f(1, lineHeight));
            line.setPosition(sf::Vector2f(x, (-lineHeight/2.0) + (win_height/2.0)));

            if (side == 1)
                line.setFillColor(sf::Color(200, 0, 0));
            else
                line.setFillColor(sf::Color(255, 0, 0));

            window.draw(line);
        }

        window.display();
    }
}
