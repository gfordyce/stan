#ifndef _FIGURE_H
#define _FIGURE_H       1

/**
 * @file figure.h
 * @brief A figure is the primary element in a stick figure animation.
 * @date 9-23-08
 * @author G. Fordyce
 */

#include <iostream>
#include <string>
#include <list>
#include <ext/hash_map>
#include <boost/foreach.hpp>

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/is_abstract.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/type_info_implementation.hpp>
#include <boost/serialization/extended_type_info_no_rtti.hpp>
#include <boost/serialization/export.hpp>

namespace stan {

/**
 * A node is a control point in a figure. An edge requires two nodes for construction.
 */
class node
{
public:
    node() :
        parent_(),
        children_(),
        x_(0),
        y_(0)
    {
    }

    node(int x, int y) :
        parent_(),
        children_(),
        x_(x),
        y_(y)
    {
    }

    node(node* parent, double x, double y) :
        parent_(parent),
        children_(),
        x_(x),
        y_(y)
    {
    }

    // accessors
    node* get_parent() { return parent_; }
    const std::list<node*>& get_children() { return children_; }
    double get_x() const { return x_; }
    double get_y() const { return y_; }
    void set_x(double x) { x_ = x; }
    void set_y(double y) { y_ = y; }

    /**
     * Pivot (rotate) this node about another
     * defined as the origin.
     */
    void pivot(node& pv, double angle)
    {
    }

    void move(int dx, int dy)
    {
        x_ += dx;
        y_ += dy;
    }

    void move_to(int x, int y)
    {
        x_ = x;
        y_ = y;
    }

    void connect_child(node* n)
    {
        children_.push_back(n);
    }

    virtual void print(std::ostream& os) const
    {
        os << "node at (" << get_x() << "," << get_y() << "):" << std::endl;
        if (parent_ != NULL) {
            os << "   parent at (" << parent_->get_x() << "," << parent_->get_y() << ")" << std::endl;
        }
        else {
            os << "   no parent" << std::endl;
        }
        if (children_.size() > 0) {
            BOOST_FOREACH(node* child, children_)
            {
                os << "   child at (" << child->get_x() << "," << child->get_y() << ")" << std::endl;
            }
        }
        else {
            os << "   no children" << std::endl;
        }
    }

    // TODO: use iterator to find n in list, then delete it.
    // void disconnect(node* n)

    // copy constructor
    node(const node& other)
    {
        parent_ = other.parent_;
        children_ = other.children_;
        x_ = other.x_;
        y_ = other.y_;
    }

    // assignment operator
    node& operator=(const node& other)
    {
        if (this != &other)
        {
            parent_ = other.parent_;
            children_ = other.children_;
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

private:
    node* parent_;
    std::list<node*> children_;
    double x_;
    double y_;
};

/**
 * An edge is defined by two nodes and a type (line, circle, ...)
 */
class edge
{
public:
    // types of edges supported
    typedef enum { edge_line, edge_circle, } edge_type;

    edge() :
        color_(0),
        type_(edge_line),
        n1_(new node(0, 0)),
        n2_(new node(0, 0))
    {
    }

    edge(edge_type type, node* n1, node* n2, int color = 0) :
        color_(color),
        type_(type),
        n1_(n1),
        n2_(n2)
    {
    }

    virtual ~edge() {}

    // accessors for member data
    int get_color() const { return color_; }
    edge_type get_type() const { return type_; }
    node* get_n1() { return n1_; }
    node* get_n2() { return n2_; }

    // copy constructor
    edge(const edge& other)
    {
        color_ = other.color_;
        type_ = other.type_;
        n1_ = other.n1_;
        n2_ = other.n2_;
    }

    // assignment operator
    edge& operator=(const edge& other)
    {
        if (this != &other) {
            color_ = other.color_;
            type_ = other.type_;
            n1_ = other.n1_;
            n2_ = other.n2_;
        }
        return *this;
    }

    virtual void print(std::ostream& os) const
    {
        // os << "Edge: " << *n1_ << ", " << *n2_ << " of type ";
        if (type_ == edge_line) {
            os << "line";
        }
        else if (type_ == edge_circle) {
            os << "circle";
        }
        os << " with color " << color_ << std::endl;
    }

protected:
    friend class boost::serialization::access;
    friend std::ostream& operator<<(std::ostream &os, const edge &e);

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
	{
        ar & BOOST_SERIALIZATION_NVP(color_);
        ar & BOOST_SERIALIZATION_NVP(type_);
        ar & BOOST_SERIALIZATION_NVP(n1_);
		ar & BOOST_SERIALIZATION_NVP(n2_);
    }

private:
    int color_;
    edge_type type_;
    node* n1_, *n2_; // an edge requires two vertices (i.e. nodes)
};

BOOST_CLASS_EXPORT(edge);

class figure
{
public:
    figure() :
        root_(new node(0, 0)),
        edges_(),
        nodes_(),
        selected_(),
        pivot_()
    {
        nodes_.push_back(root_);
    }

    figure(double x, double y) :
        root_(new node(x, y)),
        edges_(),
        nodes_(),
        selected_(),
        pivot_()
    {
        nodes_.push_back(root_);
    }

    // Accessors
    node* get_root() { return root_; }
    std::vector<edge*>& get_edges() { return edges_; }
    std::vector<node*>& get_nodes() { return nodes_; }
    double get_xpos() const { return root_->get_x(); }
    double get_ypos() const { return root_->get_y(); }
    node* get_selected() { return selected_; }
    node* get_pivot() { return pivot_; }

    // copy constructor
    figure(const figure& other)
    {
        // create new root
        node* np;
        root_ = create_node(np, other.root_->get_x(), other.root_->get_y());

        // traverse the node graph beginning at root
        traverse_copy(other.root_, root_, const_cast<figure&>(other));

        // copy edges
    }

    /**
     * Traver source graph and create a copy (dest)
     * Also maintain selected and pivot.
     */
    void traverse_copy( node* sn, node* dn, figure& sf);

    // assignment operator
    figure& operator=(const figure& other)
    {
        if (this != &other)
        {
            root_ = other.root_;
            edges_ = other.edges_;
            nodes_ = other.nodes_;
        }
        return *this;
    }

    node* create_node(node* parent, double x, double y)
    {
        node* child(new node(parent, x, y));
        if (parent != NULL)
            parent->connect_child(child);
        nodes_.push_back(child);
        return child;
    }

    edge* create_line(node* parent, double x, double y)
    {
        node* child = create_node(parent, x, y);
        edge* e(new edge(edge::edge_line, parent, child));
        edges_.push_back(e);
        return e;
    }

    edge* create_circle(node* n1, node* n2)
    {
        edge* e(new edge(edge::edge_circle, n1, n2));
        edges_.push_back(e);
        return e;
    }

    edge* create_circle(node* parent, double x, double y)
    {
        node* child = create_node(parent, x, y);
        edge* e(new edge(edge::edge_circle, parent, child));
        edges_.push_back(e);
        return e;
    }

    /**
     * Determine if a node exists at the specified position and within a given radius.
     * @param an Place to return node if found
     * @param x,y Window position
     */
    bool get_node_at_pos(node* an, double x, double y, int radius)
    {
        bool found = false;
        // simple method: model node as a square of (radius x radius)
        double xl = x - radius;
        double xr = x + radius;
        double yt = y - radius;
        double yb = y + radius;
        BOOST_FOREACH(node* n, nodes_)
        {
            if ( (n->get_x() > xl) && (n->get_x() < xr) && (n->get_y() > yt) && (n->get_y() < yb) )
            {
                found = true;
                an = n;
                break;
            }
        }
        return found;
    }

    bool is_root_node(node* n)
    {
        if (n == root_)
            return true;
        return false;
    }

    /**
     * Find all decendants of the given node (e.g. sub-tree).
     */
    void get_decendants(std::list<node*>& decendants_, node* parent)
    {
        const std::list<node*>& children = parent->get_children();
        BOOST_FOREACH(node* child, children)
        {
            std::cout << "Found decendant " << *child << std::endl;
            decendants_.push_back(child);
            get_decendants(decendants_, child);
        }
    }

    /**
     * move the figure by delta
     */
    bool move(double dx, double dy);

    virtual void print(std::ostream& os) const
    {
        os << "Nodes:" << std::endl;
        BOOST_FOREACH(node* n, nodes_)
        {
            os << n << ": " << *n << std::endl;
        }

        os << "Edges:" << std::endl;
        BOOST_FOREACH(edge* e, edges_)
        {
            os << e << ": " << *e << std::endl;
            // e->print(os);
        }
    }

protected:
    friend class boost::serialization::access;
    friend std::ostream& operator<<(std::ostream &os, const figure &f);

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
	{
        // teach the archive how to handle shape pointers
        // ar.register_type(static_cast< line* >(NULL));
        // ar.register_type(static_cast< circle* >(NULL));

        ar & BOOST_SERIALIZATION_NVP(root_);
        ar & BOOST_SERIALIZATION_NVP(edges_);
        ar & BOOST_SERIALIZATION_NVP(nodes_);
    }

private:
    node* root_;
    std::vector<edge*> edges_;
    std::vector<node*> nodes_;

    node* selected_;
    node* pivot_;
};

BOOST_CLASS_EXPORT(figure);

};  // namespace stan

#endif  // _FIGURE_H
