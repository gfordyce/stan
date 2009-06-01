#include "trig.h"
#include <boost/foreach.hpp>

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

/**
 * Calculate angle of pt with pc defined as center of circle
 * relative to vertical (90 degrees as 0)
 */
double calc_angle_vertical(Point& pc, Point& pt)
{
    // Make pt relative to pc
    pt.x -= pc.x;
    pt.y -= pc.y;

    // Our origin
    Point po(0, 0);
    // Create our vertical reference point
    Point pv(0, 1);

    // calculate angle between pv and pt
    return calc_angle(po, pv, pt);
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

void rotate_figure(figure* src_fig, figure *dst_fig, int origin_node, std::list<int> rot_nodes, double angle)
{
    node* on = src_fig->get_node(origin_node);   // pivot node
    BOOST_FOREACH(int n, rot_nodes) {

        std::cout << "Rotate node " << n << std::endl;

        node* sn = src_fig->get_node(n);        // source node
        node* dn = dst_fig->get_node(n);        // dest node

        // define origin at 0,0
        double dx = on->get_x();
        double dy = on->get_y();

        // perform rotation by angle and store into dest node
        dn->set_x( ((sn->get_x() - dx) * cos(angle) - (sn->get_y() - dy) * sin(angle)) + dx );
        dn->set_y( ((sn->get_x() - dx) * sin(angle) + (sn->get_y() - dy) * cos(angle)) + dy );
    }
}

};  // namespace stan

// END of this file -----------------------------------------------------------
