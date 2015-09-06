#ifndef GLOBALS_H
#define GLOBALS_H

/*
 * Globals are declared here.
 * The values of the globals are pulled in from globals.dat by the
 * importGlobals() function.
*/

struct Globals {
    int winH;
    int winL;

    bool mouseLocked;

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

    int crouch;
};

extern Globals glbl;

#endif // GLOBALS_H
