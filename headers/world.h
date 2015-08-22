#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "map.h"

class World {
    std::vector<Map> maps;

public:
    Map map;

    World(std::string in_map);
    World(Map in_map);
    World();
    void addMap(std::string in_map);
    void addMap(Map in_map);
    void switchToMap(int i);
    int totalMaps();
};

#endif // WORLD_H