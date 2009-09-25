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

#include <boost/foreach.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include "metadata.h"

class test_figure;

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
    const std::list<int>& get_children() { return children_; }
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

    virtual void print(std::ostream& os) const
    {
        os << "node at (" << get_x() << "," << get_y() << "):" << std::endl;
        os << "   parent: " << parent_ << std::endl;

        if (children_.size() > 0) {
            BOOST_FOREACH(int c, children_) {
                os << "   child: " << c << std::endl;
            }
        }
        else {
            os << "   no children" << std::endl;
        }
    }

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

/**
 * An edge is defined by two nodes and a type (line, circle, image, ...)
 */
class edge
{
public:
    // types of edges supported
    typedef enum { edge_line, edge_circle, edge_image} edge_type;

    edge() :
        color_(0),
        type_(edge_line),
        name_(),
        meta_index_(-1),
        n1_(-1),
        n2_(-1)
    {
    }

        edge(edge_type type, int n1, int n2, int color = 0) :
        color_(color),
        type_(type),
        name_(),
        meta_index_(-1),
        n1_(n1),
        n2_(n2)
    {
    }

    virtual ~edge() {}

    // accessors for member data
    int get_color() const { return color_; }
    void set_color(int color) { color_ = color; }

    edge_type get_type() const { return type_; }

    std::string get_name() { return name_; }
    void set_name(std::string name) { name_ = name; }

    /*
     * The meta index is used to reference images for image type
     * edges.
     */
    void set_meta_index(int index) { meta_index_ = index; }
    int get_meta_index() { return meta_index_; }

    int get_n1() { return n1_; }
    int get_n2() { return n2_; }

    // copy constructor
    edge(const edge& other)
    {
        color_ = other.color_;
        type_ = other.type_;
        name_ = other.name_;
        n1_ = other.n1_;
        n2_ = other.n2_;
        meta_index_ = other.meta_index_;
    }

    // assignment operator
    edge& operator=(const edge& other)
    {
        if (this != &other) {
            color_ = other.color_;
            type_ = other.type_;
            name_ = other.name_;
            n1_ = other.n1_;
            n2_ = other.n2_;
            meta_index_ = other.meta_index_;
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
        else if (type_ == edge_image) {
            os << "Image";
        }
        os << " with color " << color_ << " and name " << name_ << std::endl;
    }

protected:
    friend class boost::serialization::access;
    friend std::ostream& operator<<(std::ostream &os, const edge &e);

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
	{
        ar & BOOST_SERIALIZATION_NVP(color_);
        ar & BOOST_SERIALIZATION_NVP(type_);
        ar & BOOST_SERIALIZATION_NVP(name_);
        ar & BOOST_SERIALIZATION_NVP(n1_);
		ar & BOOST_SERIALIZATION_NVP(n2_);
        ar & BOOST_SERIALIZATION_NVP(meta_index_);
    }

public:
    int color_;
    edge_type type_;
    std::string name_;
    int meta_index_;
    int n1_, n2_; // an edge requires two vertices (i.e. nodes)
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(edge)

class figure
{
private:
    static const int MIN_WEIGHT = 1;
    static const int MAX_WEIGHT = 100;

public:
    figure() :
        root_(-1),
        edges_(),
        nodes_(),
        weight_(1),
        selected_(-1),
        pivot_(-1),
        is_enabled_(true),
        meta_store_(new meta_store())
    {
    }

    figure(double x, double y) :
        edges_(),
        nodes_(),
        weight_(1),
        selected_(-1),
        pivot_(-1),
        is_enabled_(true),
        meta_store_(new meta_store())
    {
        root_ = create_node(-1, x, y);
    }

    virtual ~figure() { delete meta_store_; }

    /**
     * Clone the subtree from the specified node in the tree.
     * This creates a new figure which is composed of a copy of the
     * nodes and edges defined in the subtree of the specified figure.
     */
    void clone_subtree(figure* other, int s_index, int d_parent);

    /**
     * Remove all decendants of a given node and edges
     */
    void remove_nodes(int nindex);

    /**
     * Remove a single node from the node list
     * NOTE: Accounting for edges which are linked to this
     * node must be accounted for prior to calling this.
     */
    void remove_node(int nindex)
    {
        node* n = get_node(nindex);
        assert(n != NULL);
        disconnect(nindex); // remove it from the parent's child list
        nodes_[nindex] = NULL;  // TODO: this should be erased from the vector!!!
        delete n;
    }

    /**
     * Disconnect a node from it's parent.
     */
    void disconnect(int nindex)
    {
        node* n = get_node(nindex);
        assert(n != NULL);

        int pindex = n->get_parent();
        node* p = get_node(pindex);

        std::list<int> children = p->get_children();
        children.remove(nindex);
#if 0
        std::list<int>::iterator iter = children.begin();
        while (iter != children.end()) {
            int cindex = *iter;
            if (cindex == nindex) {
                iter.remove();
                break;
            }
            else
                iter++;
        }
#endif
    }

    void remove_edge(int eindex)
    {
        edge* e = get_edge(eindex);
        assert(e != NULL);
        edges_[eindex] = NULL;  // TODO: this should be erased from the vector!!!
        delete e;
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
    bool is_enabled() { return is_enabled_; }
    void set_enabled(bool enabled) { is_enabled_ = enabled; }

    /**
     * The weight defines the line thickness when the figure is drawn.
     */
    void set_weight(int weight) { weight_ = weight; }
    int get_weight() { return weight_; }

    /**
     * Find the edge which has the given nodes as endoints
     */
    int get_edge(int n1, int n2)
    {
        for(unsigned eindex = 0; eindex < edges_.size(); eindex++) {
            edge* en = get_edge(eindex);
			if (en != NULL) {
				if ( (en->get_n1() == n1) && (en->get_n2() == n2) ) {
					return eindex;
				}
            }
        }
        return -1;
    }

    /**
     * Clone the figure.
     */
    void clone(figure* fig, const figure& other);

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
        int child = static_cast<int>(nodes_.size()) - 1;

        // if child has a parent, look it up and add child
        if (parent != -1) {
            node* pn = get_node(parent);
            pn->connect_child(child);
        }
        else {
            // no parent, we can safely assume this is the first node and
            // make it the root node
            root_ = child;
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
        int eindex = static_cast<int>(edges_.size()) - 1;
        return eindex;
    }

    /**
     * create a circle defined by the given nodes
     * @param n1 The top node
     * @param n2 The bottom node
     * @return The newly created edge index
     */
    int create_circle(int n1, int n2)
    {
        edge* e = new edge(edge::edge_circle, n1, n2);
        edges_.push_back(e);
        int eindex = static_cast<int>(edges_.size()) - 1;
        return eindex;
    }

    /**
     * create a circle defined by node and a point
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
        int eindex = static_cast<int>(edges_.size()) - 1;
        return eindex;
    }

    meta_store* get_meta_store() { return meta_store_; }

    /**
     * create an image defined by node and a point
     * @param parent The top node
     * @param x The x position of the bottom node
     * @param y The y position of the bottom node
     * @param image_index The index of image data in the meta store.
     * @return The newly created edge index
     */
    int create_image(int parent, double x, double y, int image_index)
    {
        int child = create_node(parent, x, y);
        edge* e = new edge(edge::edge_image, parent, child);
        e->set_meta_index(image_index);
        edges_.push_back(e);
        int eindex = static_cast<int>(edges_.size()) - 1;
        return eindex;
    }

    /**
     * Determine if a node exists at the specified position and within a given radius.
     * @param an Place to return node if found
     * @param x,y Window position
     */
    bool get_node_at_pos(int& an, double x, double y, int radius)
    {
        bool found = false;
        // simple method: model node as a square of (radius x radius)
        double xl = x - radius;
        double xr = x + radius;
        double yt = y - radius;
        double yb = y + radius;
        for (unsigned n = 0; n < nodes_.size(); n++) {
            node *pn = get_node(n);
            if (pn != NULL) {
                if ( (pn->get_x() > xl) && (pn->get_x() < xr) && (pn->get_y() > yt) && (pn->get_y() < yb) ) {
                    found = true;
                    an = n;
                    break;
                }
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
     * Find the edge defined by n1 and n2
     */
    edge* find_edge(int n1, int n2)
    {
        edge* e = NULL;

        for(unsigned eindex = 0; eindex < edges_.size(); eindex++) {
            edge* en = get_edge(eindex);
            if ( (en->get_n1() == n1) && (en->get_n2() == n2) ) {
                e = en;
                break;
            }
        }
        return e;
    }

    /**
     * Find all decendants of the given node (e.g. sub-tree).
     */
    void get_decendants(std::list<int>& decendants, int parent)
    {
        node* pn = get_node(parent);
        const std::list<int>& children = pn->get_children();
        BOOST_FOREACH(int c, children) {
            decendants.push_back(c);
            get_decendants(decendants, c);
        }
    }

    /**
     * move the figure by delta
     */
    void move(double dx, double dy);

    /**
     * Scale the figure by specified factor.
     */
    void scale(double factor);

    /**
     * Decrease line thickness.
     */
    bool thinner()
    {
        bool success = false;
        if (weight_ > MIN_WEIGHT) {
            weight_--;
            success = true;
        }
        return success;
    }

    /**
     * Increase line thickness.
     */
    bool thicker()
    {
        bool success = false;
        if (weight_ < MAX_WEIGHT ) {
            weight_++;
            success = true;
        }
        return success;
    }

    virtual void print(std::ostream& os) const
    {
        os << "Nodes:" << std::endl;
        for(unsigned n = 0; n < nodes_.size(); n++) {
            node* pn = get_node(n);
			if (pn != NULL)
				os << n << ": " << *pn << std::endl;
        }

        os << "Edges:" << std::endl;
        for(unsigned e = 0; e < edges_.size(); e++) {
            edge* en = get_edge(e);
			if (en != NULL)
				os << e << ": " << *en << std::endl;
        }

        os << "Images:" << std::endl;
        os << *meta_store_ << std::endl;
    }

protected:
    friend class boost::serialization::access;
    friend std::ostream& operator<<(std::ostream &os, const figure &f);

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
	{
        ar & BOOST_SERIALIZATION_NVP(root_);
        ar & BOOST_SERIALIZATION_NVP(edges_);
        ar & BOOST_SERIALIZATION_NVP(nodes_);
        ar & BOOST_SERIALIZATION_NVP(weight_);
        ar & BOOST_SERIALIZATION_NVP(meta_store_);
    }

public:
    int root_;
    std::vector<edge*> edges_;
    std::vector<node*> nodes_;

    int weight_;
    int selected_;
    int pivot_;

    bool is_enabled_;
    meta_store* meta_store_;   // metadata lookup for images
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(figure)

};  // namespace stan

#endif  // _FIGURE_H
