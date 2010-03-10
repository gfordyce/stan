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
#include <vector>

#include <boost/foreach.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include "node.h"
#include "edge.h"
#include "metadata.h"

class test_figure;

namespace stan {

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
    void remove_decendants(int nindex);

    /**
     * Remove a child from the parent's list of children.
     */
    void remove_child(int child);

    /**
     * Fix all figure references to nodes with index >= nindex.
     * This is necessary when a node is delected from the vector and all
     * nodes below the deleted element are shifted up.
     */
    void fix_node_refs(int nindex);

    /*
     * Remove this node, the edge between it and parent,
     * and all children.
     */
    void remove_children(int nindex);

    /**
     * Disconnect a node from it's parent.
     */
    void disconnect(int nindex);

    void remove_edge(int eindex);

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
    int get_edge(int n1, int n2);

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

    int create_node(int parent, double x, double y);

    /**
     * create a line from a given node to a new point
     * (creates a new node for end point)
     * @param parent Index of parent node
     * @param x The x position of the end point
     * @param y The y position of the end point
     * @return The newly created edge index
     */
    int create_line(int parent, double x, double y);

    /**
     * create a circle defined by the given nodes
     * @param n1 The top node
     * @param n2 The bottom node
     * @return The newly created edge index
     */
    int create_circle(int n1, int n2);

    /**
     * create a circle defined by node and a point
     * @param parent The top node
     * @param x The x position of the bottom node
     * @param y The y position of the bottom node
     * @return The newly created edge index
     */
    int create_circle(int parent, double x, double y);

    meta_store* get_meta_store() { return meta_store_; }

    /**
     * create an image defined by node and a point
     * @param parent The top node
     * @param x The x position of the bottom node
     * @param y The y position of the bottom node
     * @param image_index The index of image data in the meta store.
     * @return The newly created edge index
     */
    int create_image(int parent, double x, double y, int image_index);

    /**
     * Determine if a node exists at the specified position and within a given radius.
     * @param an Place to return node if found
     * @param x,y Window position
     */
    bool get_node_at_pos(int& an, double x, double y, int radius);

    bool is_root_node(int n)
    {
        if (n == root_)
            return true;
        return false;
    }

    /**
     * Find the edge defined by n1 and n2
     */
    edge* find_edge(int n1, int n2);

    /**
     * Find all decendants of the given node (e.g. sub-tree).
     */
    void get_decendants(std::list<int>& decendants, int parent);

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
    bool thinner();

    /**
     * Increase line thickness.
     */
    bool thicker();

    virtual void print(std::ostream& os) const;

private:

    /**
     * Remove a single node from the node list
     * NOTE: Accounting for edges which are linked to this
     * node must be done for prior to calling this.
     */
    void remove_node(int nindex);

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
