#ifndef _NODE_H
#define _NODE_H       1

/**
 * @file node.h
 * @brief A node represents a vertice in a graph. Nodes may form graphs
 *        through parent / child relations.
 *
 * @date 1-19/10
 * @author G. Fordyce
 */

#include <iostream>
#include <list>
#include <boost/foreach.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>

namespace stan {

/**
 * A node is a control point in a figure. An edge requires two nodes for construction.
 * Nodes are organized into DAGs through parent / child associations.
 */
class node
{
    friend class test_figure;   // unit test class

public:
    node() :
        parent_(-1),
        children_(),
        x_(0),
        y_(0)
    {
    }

    node(int x, int y) :
        parent_(-1),
        children_(),
        x_(x),
        y_(y)
    {
    }

    node(int parent, double x, double y) :
        parent_(parent),
        children_(),
        x_(x),
        y_(y)
    {
    }

    virtual ~node() {}

    /**
     * accessors
     */
    int get_parent() { return parent_; }
    void set_parent(int p) { parent_ = p; }
    const std::list<int>& get_children() { return children_; }
    void clear_children() { children_.clear(); }
    double get_x() const { return x_; }
    double get_y() const { return y_; }
    void set_x(double x) { x_ = x; }
    void set_y(double y) { y_ = y; }

    /**
     * Move a node by a delta
     */
    void move(double dx, double dy)
    {
        x_ += dx;
        y_ += dy;
    }

    /**
     * Move a node to absolute position
     */
    void move_to(double x, double y)
    {
        x_ = x;
        y_ = y;
    }

    void connect_child(int n)
    {
        children_.push_back(n);
    }

    void remove_child(int child)
    {
        children_.remove(child);
    }

    virtual void print(std::ostream& os) const;

    // copy constructor
    node(const node& other)
    {
        // new node does not inherit parent or children, just position
        parent_ = -1;
        x_ = other.x_;
        y_ = other.y_;
    }

    // assignment operator
    node& operator=(const node& other)
    {
        if (this != &other)
        {
            parent_ = -1;
            x_ = other.x_;
            y_ = other.y_;
        }
        return *this;
    }
    

protected:
    friend class boost::serialization::access;
    friend std::ostream& operator<<(std::ostream &os, const node &n);

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & BOOST_SERIALIZATION_NVP(parent_);
        ar & BOOST_SERIALIZATION_NVP(children_);
        ar & BOOST_SERIALIZATION_NVP(x_);
        ar & BOOST_SERIALIZATION_NVP(y_);
    }

public:
    int parent_;
    std::list<int> children_;
    double x_;
    double y_;
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(node)


};  // namespace stan

#endif  // _NODE_H
