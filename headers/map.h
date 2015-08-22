#ifndef MAP_H
#define MAP_H

#include <string>
#include <iostream>

class Map {
    std::string grid;

    void findDimensions();

public:
    int l;
    int h;

    Map(std::string &in_grid);
    Map();
    char charAt(int x, int y);
    void print();
};

#endif // MAP_H