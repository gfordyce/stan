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

        double x;
        double y;
    };

    const double PI = 3.14159265;

    double rad2deg(double angle);

    /**
     * Calculate angle between p1 and p2 with
     * p0 as the origin.
     */
    double calc_angle(Point& p0, Point& p1, Point& p2);

    double calc_angle_vertical(Point& pc, Point& pt);

    double calc_angle_old(Point& origin, Point& p);

    double calcRotationAngle(Point& origin, Point& from, Point& to);

    /**
     * rotate nodes from source figure positions into a destination figure (must be structually equivalent)
     */
    void rotate_figure(figure* src_fig, figure *dst_fig, int origin_node, std::list<int> rot_nodes, double angle);

    void midpoint(Point& p1, Point& p2, Point& mp);

};  // namespace stan

#endif  // _TRIG_H
