#ifndef GLOBALS_H
#define GLOBALS_H

struct Globals {
    int winH;
    int winL;

    int columnWidth;

    double walkSpeed;
    double runSpeed;
    double rotateSpeed;

    double startX;
    double startY;

    double startDir;
    double startPlane;
    
    int maxBrightness; // Out of 255
    int renderDistance;

    int shadowSide;
    double shadowDarkness;

    int selectedMap;
};

extern Globals glbl;

#endif // GLOBALS_H
