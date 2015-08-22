#include "../headers/world.h"

#include <iostream>

World::World(std::string in_map) {
    std::cout << maps.size() << std::endl;
    addMap(in_map);
    std::cout << maps.size() << std::endl;
    switchToMap(0);
}
World::World(Map in_map) {
    addMap(in_map);
    switchToMap(0);
}

void World::addMap(std::string in_map) {
    maps.push_back(Map(in_map));
}
void World::addMap(Map in_map) {
    maps.push_back(in_map);
}

void World::switchToMap(int i) {
    if (i < maps.size()) {
        map = maps.at(i);
    }
}

int World::totalMaps() {
    return maps.size();
}