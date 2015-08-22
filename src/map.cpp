#include "../headers/map.h"

Map::Map() {
    grid =
    "000\n"
    "0 0\n"
    "000\n";
    findDimensions();
}
Map::Map(std::string &in_grid) {
    grid = in_grid;
    findDimensions();
}

void Map::findDimensions() {
    for (l = 1; grid[l - 1] != '\n'; ++l) {;}
    h = grid.length() / l;
}

char Map::charAt(int x, int y) {
    return grid.at(y*l + x);
}

void Map::print() {
    std::cout << grid << std::endl;
}