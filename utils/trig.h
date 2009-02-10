#ifndef _TRIG_H
#define _TRIG_H     1

#include <list>
#include "figure.h"

/**
 * @file trig.h
 * @brief Math utilities used in stan.
 */

namespace stan {

    class Point
    {
    public:
        Point() :
            x(0),
            y(0)
        {}

        Point(int ix, int iy) :
            x(ix),
            y(iy)
        {}

        int x;
        int y;
    };

    const double PI = 3.14159265;

    double rad2deg(double angle);

    /**
     * Calculate angle between p1 and p2 with
     * p0 as the origin.
     */
    double calc_angle(Point& p0, Point& p1, Point& p2);

    double calc_angle_old(Point& origin, Point& p);

    double calcRotationAngle(Point& origin, Point& from, Point& to);

    void rotate_nodes(int x, int y, std::list<node*>& n1, std::list<node*>& n2);

};  // namespace stan

#endif  // _TRIG_H
