#include "trig.h"

namespace stan {

double rad2deg(double angle)
{
    return ((angle * 180.0) / PI);
}

/**
 * Calculate angle between p1 and p2 with
 * p0 as the origin.
 */
double calc_angle(Point& p0, Point& p1, Point& p2)
{
    // shift p1 to origin
    double x1 = static_cast<double>(p1.x - p0.x);
    double y1 = static_cast<double>(p1.y - p0.y);

    // shift p2 to origin
    double x2 = static_cast<double>(p2.x - p0.x);
    double y2 = static_cast<double>(p2.y - p0.y);

    // calculate the angle
    double a1 = atan2(x1, y1);
    double a2 = atan2(x2, y2);

    double angle = (2 * PI) - (a2 - a1);
    if (angle > (2 * PI))
        angle -= (2 * PI);

    return angle;
}

double calc_angle_old(Point& origin, Point& p)
{
    // calculate from angle
    double dx = static_cast<double>(p.x - origin.x);
    double dy = static_cast<double>(p.y - origin.y);
    std::cout << "dy = " << dy << ", dx = " << dx << std::endl;
    double radius = sqrt((dy * dy) + (dx * dx));
    double angle = asin(dy / radius);

    if (dx == 0 && dy == 0)
        return 0;           // no difference
    else if (dx > 0 && dy > 0)
        return angle;       // angle between 0 and 90 degrees
    else if (dx < 0 && dy > 0)
        return angle + (PI / 2);      // angle between 90 and 180 degrees
    else if (dx < 0 && dy < 0)
        return angle + PI;  // angle between 180 and 270 degrees
    else if (dx >= 0 && dy < 0)        // angle between 270 and 360 degrees
        return angle + (((3 * PI) / 2));

    return 0;
}

double calcRotationAngle(Point& origin, Point& from, Point& to)
{
    double from_angle = calc_angle_old(origin, from);
    double to_angle = calc_angle_old(to, from);

    std::cout << "from_angle is " << rad2deg(from_angle) << std::endl;
    std::cout << "to_angle is " << rad2deg(to_angle) << std::endl;

    if (from.x > to.x)
        return from_angle - to_angle;
    else
        return to_angle - from_angle;
}

void rotate_nodes(int x, int y, std::list<node*>& n1, std::list< node*>& n2)
{
}

};  // namespace stan

// END of this file -----------------------------------------------------------
