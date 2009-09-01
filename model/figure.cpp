#include "figure.h"

namespace stan {

std::ostream& operator<<(std::ostream &os, const node &n)
{
    n.print(os);
    return os;
}

std::ostream& operator<<(std::ostream &os, const edge &e)
{
    e.print(os);
    return os;
}

std::ostream& operator<<(std::ostream &os, const figure &f)
{
    f.print(os);
    return os;
}

void figure::clone(figure* fig, const figure& other)
{
    // copy meta data
    fig->root_ = other.root_;
    fig->selected_ = other.selected_;
    fig->pivot_ = other.pivot_;
    fig->is_enabled_ = other.is_enabled_;

    // copy nodes
    for (unsigned n = 0; n < other.nodes_.size(); n++) {
        node* en = other.get_node(n);
        if (en != NULL) {
            node* n = new node(en->parent_, en->get_x(), en->get_y());
            fig->nodes_.push_back(n);
            BOOST_FOREACH(int c, en->children_) {
                n->children_.push_back(c);
            }
        }
    }

    // copy edges
    for (unsigned e = 0; e < other.edges_.size(); e++) {
        edge* ee = other.get_edge(e);
        if (ee != NULL) {
            // edge *e = new edge(ee->type_, ee->n1_, ee->n2_, ee->color_);
            edge* e = new edge(*ee);
            fig->edges_.push_back(e);
        }
    }

    // copy images
    image_store_ = other.image_store_;
}

void figure::move(double dx, double dy)
{
    for (unsigned n = 0; n < nodes_.size(); n++) {
        node* an = get_node(n);
        if (an != NULL) {
            an->move(dx, dy);
        }
    }
}

void figure::remove_nodes(int nindex)
{
    node* n = get_node(nindex);
    const std::list<int>& children = n->get_children();

    BOOST_FOREACH(int child, children) {
        int eindex = get_edge(nindex, child);
        if (eindex != -1) {
            remove_edge(eindex);
        }
        remove_nodes(child);
    }
    remove_node(nindex);
}

void figure::clone_subtree(figure* other, int s_index, int d_parent)
{
    std::cout << "clone_subtree: cloning " << s_index << std::endl;

    node* s_node = other->get_node(s_index);
    assert(s_node != NULL);
    const std::list<int>& s_children = s_node->get_children();

    node* d_node = new node(d_parent, s_node->get_x(), s_node->get_y());
    nodes_.push_back(d_node);
    int d_index = nodes_.size() - 1;
    if (d_parent != -1) {
        // if we have a parent, we need to add ourself to the parent's child list
        node* d_node_parent = get_node(d_parent);
        assert(d_node_parent != NULL);
        d_node_parent->connect_child(d_index);
    }
    else {
        // this must be the new root node
        root_ = d_index;
    }

    // copy edge from parent to child
    if (d_parent != -1) {   // new root, we don't want the edge
        int s_parent = other->get_node(s_index)->get_parent();
        int s_edge_index = other->get_edge(s_parent, s_index);
        if (s_edge_index != -1) {
            edge* s_edge = other->get_edge(s_edge_index);
            assert(s_edge != NULL);
            edge* d_edge = new edge(s_edge->get_type(), d_parent, d_index);
            edges_.push_back(d_edge);
        }
    }

    // recursively copy children
    BOOST_FOREACH(int s_child, s_children) {
        clone_subtree(other, s_child, d_index);
    }
}

};  // namespace stan

// END of this file -----------------------------------------------------------
