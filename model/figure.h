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

#include <boost/serialization/export.hpp>
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

namespace stan {

/**
 * A node is a control point in a figure. An edge requires two nodes for construction.
 */
class node
{
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

    // accessors
    int get_parent() { return parent_; }
    const std::list<int>& get_children() { return children_; }
    double get_x() const { return x_; }
    double get_y() const { return y_; }
    void set_x(double x) { x_ = x; }
    void set_y(double y) { y_ = y; }

    /**
     * Pivot (rotate) this node about another
     * defined as the origin.
     */
    void pivot(int pv, double angle)
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

    void connect_child(int n)
    {
        children_.push_back(n);
    }

    virtual void print(std::ostream& os) const
    {
        os << "node at (" << get_x() << "," << get_y() << "):" << std::endl;
        os << "   parent: " << parent_ << std::endl;

        if (children_.size() > 0) {
            for(unsigned c = 0; c < children_.size(); c++) {
                os << "   child: " << c << std::endl;
            }
        }
        else {
            os << "   no children" << std::endl;
        }
    }

    // TODO: use iterator to find n in list, then delete it.
    // void disconnect(int n)

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
        n1_(-1),
        n2_(-1)
    {
    }

    edge(edge_type type, int n1, int n2, int color = 0) :
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
    int get_n1() { return n1_; }
    int get_n2() { return n2_; }

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
        os << "Edge: " << n1_ << ", " << n2_ << " of type ";
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

public:
    int color_;
    edge_type type_;
    int n1_, n2_; // an edge requires two vertices (i.e. nodes)
};

BOOST_CLASS_EXPORT(edge);

class figure
{
public:
    figure() :
        edges_(),
        nodes_(),
        selected_(-1),
        pivot_(-1)
    {
        root_ = create_node(-1, 0, 0);
    }

    figure(double x, double y) :
        edges_(),
        nodes_(),
        selected_(),
        pivot_(-1)
    {
        root_ = create_node(-1, x, y);
    }

    // Accessors
    node* get_node(int n) const { return nodes_[n]; }
    edge* get_edge(int e) const { return edges_[e]; }
    int get_root() const { return root_; }
    std::vector<edge*>& get_edges() { return edges_; }
    std::vector<node*>& get_nodes() { return nodes_; }
    double get_xpos() { node* rn = get_node(root_); return rn->get_x(); }
    double get_ypos() { node* rn = get_node(root_); return rn->get_y(); }
    int get_selected() { return selected_; }
    int get_pivot() { return pivot_; }

    void clone(figure* fig, const figure& other)
    {
        fig->root_ = other.root_;
        fig->selected_ = other.selected_;
        fig->pivot_ = other.pivot_;

        // copy nodes
        for (unsigned n = 0; n < other.nodes_.size(); n++) {
            node* en = other.get_node(n);
            node* n = new node(en->parent_, en->get_x(), en->get_y());
            fig->nodes_.push_back(n);
            for (unsigned c = 0; c < en->children_.size(); c++) {
                n->children_.push_back(c);
            }
        }

        // copy edges
        for (unsigned e = 0; e < other.edges_.size(); e++) {
            edge* ee = other.get_edge(e);
            edge *e = new edge(ee->type_, ee->n1_, ee->n2_, ee->color_);
            fig->edges_.push_back(e);
        }
    }

    // copy constructor
    figure(const figure& other)
    {
        clone(this, other);
    }

    // assignment operator
    figure& operator=(const figure& other)
    {
        if (this != &other)
        {
            clone(this, other);
        }
        return *this;
    }

    int create_node(int parent, double x, double y)
    {
        node* cn = new node(parent, x, y);
        nodes_.push_back(cn);
        int child = nodes_.size() - 1;

        // if child has a parent, look it up and add child
        if (parent != -1) {
            node* pn = get_node(parent);
            pn->connect_child(child);
        }

        return child;
    }

    /**
     * create a line from a given node to a new point
     * (creates a new node for end point)
     * @param parent Index of parent node
     * @param x The x position of the end point
     * @param y The y position of the end point
     * @return The newly created edge index
     */
    int create_line(int parent, double x, double y)
    {
        int child = create_node(parent, x, y);
        edge* e = new edge(edge::edge_line, parent, child);
        edges_.push_back(e);
        int eindex = edges_.size() - 1;
        return eindex;
    }

    /**
     * create a circle defines by the given nodes
     * @param n1 The top node
     * @param n2 The bottom node
     * @return The newly created edge index
     */
    int create_circle(int n1, int n2)
    {
        edge* e = new edge(edge::edge_circle, n1, n2);
        edges_.push_back(e);
        int eindex = edges_.size() - 1;
        return eindex;
    }

    /**
     * create a circle defines by node and a point
     * @param parent The top node
     * @param x The x position of the bottom node
     * @param y The y position of the bottom node
     * @return The newly created edge index
     */
    int create_circle(int parent, double x, double y)
    {
        int child = create_node(parent, x, y);
        edge* e = new edge(edge::edge_circle, parent, child);
        edges_.push_back(e);
        int eindex = edges_.size() - 1;
        return eindex;
    }

    /**
     * Determine if a node exists at the specified position and within a given radius.
     * @param an Place to return node if found
     * @param x,y Window position
     */
    bool get_node_at_pos(int an, double x, double y, int radius)
    {
        bool found = false;
        // simple method: model node as a square of (radius x radius)
        double xl = x - radius;
        double xr = x + radius;
        double yt = y - radius;
        double yb = y + radius;
        for (unsigned n = 0; n < nodes_.size(); n++) {
            node *pn = get_node(n);
            if ( (pn->get_x() > xl) && (pn->get_x() < xr) && (pn->get_y() > yt) && (pn->get_y() < yb) )
            {
                found = true;
                an = n;
                break;
            }
        }
        return found;
    }

    bool is_root_node(int n)
    {
        if (n == root_)
            return true;
        return false;
    }

    /**
     * Find all decendants of the given node (e.g. sub-tree).
     */
    void get_decendants(std::list<int>& decendants, int parent)
    {
        node* pn = get_node(parent);
        const std::list<int>& children = pn->get_children();
        for (unsigned c = 0; c < children.size(); c++) {
            std::cout << "Found decendant " << c << std::endl;
            decendants.push_back(c);
            get_decendants(decendants, c);
        }
    }

    /**
     * move the figure by delta
     */
    void move(double dx, double dy);

    virtual void print(std::ostream& os) const
    {
        os << "Nodes:" << std::endl;
        for(unsigned n = 0; n < nodes_.size(); n++) {
            node* pn = get_node(n);
            os << n << ": " << *pn << std::endl;
        }

        os << "Edges:" << std::endl;
        for(unsigned e = 0; e < edges_.size(); e++) {
            edge* en = get_edge(e);
            os << e << ": " << *en << std::endl;
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
    int root_;
    std::vector<edge*> edges_;
    std::vector<node*> nodes_;

    int selected_;
    int pivot_;
};

BOOST_CLASS_EXPORT(figure);

};  // namespace stan

#endif  // _FIGURE_H
