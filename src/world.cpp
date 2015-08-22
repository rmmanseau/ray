#include "../headers/world.h"

World::World(Map in_map) {
    addMap(in_map);
    switchToMap(0);
}
World::World() {}

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