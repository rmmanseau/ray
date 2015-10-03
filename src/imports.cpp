#include "../headers/imports.h"
#include "../headers/globals.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>


void importGlobals()
{
    std::ifstream varFile;
    varFile.open("../config/globals.dat");
    
    std::string tmp;

    std::vector<double> vars;

    while (getline(varFile, tmp, ':'))
    {
        double currentVar;

        varFile >> currentVar;
        vars.push_back(currentVar);
    }

    varFile.close();

    glbl.columnWidth = vars[0];

    glbl.walkSpeed = vars[1];
    glbl.runSpeed = vars[2];
    glbl.rotateSpeed = vars[3];

    glbl.startX = vars[4];
    glbl.startY = vars[5];

    glbl.FOV = vars[6];

    glbl.maxBrightness = vars[7];
    glbl.renderDistance = vars[8];

    glbl.shadowSide = vars[9];
    glbl.shadowDarkness = vars[10];

    glbl.selectedMap = vars[11];
}

void importMaps(World& world)
{
    std::ifstream mapFile;

    char c;
    char prev;

    mapFile.open("../config/worlds.dat");
    while (mapFile.get(c))
    {
        if (c == '%')
        {
            int baseLength = -1;
            int length = 0;
            std::stringstream world_in;

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

            std::string mapString = world_in.str();
            world.addMap(Map(mapString));
        }
    }
    mapFile.close(); 
}