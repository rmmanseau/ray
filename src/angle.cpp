#include "../headers/angle.h"
#include <cmath>

void Angle::fix(double &theta) {
    if (theta >= 360.0)
        theta -= 360.0;
    else if (theta < 0)
        theta += 360.0;
}

Angle::Angle()
    : angle(0)
{}
Angle::Angle(double in_angle)
    : angle(in_angle)
{
    fix(angle);
}

double Angle::rad() {
    return angle * M_PI / 180.0;
}
double Angle::deg() {
    return angle;
}
int Angle::quadrant() {
    if (angle >= 0 && angle < 90) {
        return 1;
    } else if (angle >= 90 && angle < 180) {
        return 2;
    } else if (angle >= 180 && angle < 270) {
        return 3;
    } else if (angle >= 270 && angle < 360) {
        return 4;
    }
}

void Angle::rotate(double theta) {
    angle += theta;
    fix(angle);
}
double Angle::rotated(double theta) {
    double newAngle = angle + theta;
    fix(newAngle);
    return newAngle;
}