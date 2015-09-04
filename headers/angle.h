#ifndef ANGLE_H
#define ANGLE_H

/* Angle class that defaults to Degrees */
class Angle {
    double angle;

    void fix(double &theta);

public:
    Angle();
    Angle(double in_angle);

    double rad();
    double deg();
    int quadrant();

    void rotate(double theta);
    double rotated(double theta);
};

#endif // ANGLE_H
