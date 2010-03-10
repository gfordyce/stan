/**
 * @file figure.cpp
 * @brief Implementation of figure class
 * @date 2-20-10
 * @author G. Fordyce
 */

#include "figure.h"

namespace stan {

std::ostream& operator<<(std::ostream &os, const figure &f)
{
    f.print(os);
    return os;
}

void figure::disconnect(int nindex)
{
    node* n = get_node(nindex);
    assert(n != NULL);

    int pindex = n->get_parent();
    node* p = get_node(pindex);

    std::list<int> children = p->get_children();
    children.remove(nindex);
}

void figure::remove_edge(int eindex)
{
    edge* e = get_edge(eindex);
    assert(e != NULL);
    edges_.erase(edges_.begin() + eindex);  // remove vector entry
    delete e;
}

int figure::get_edge(int n1, int n2)
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

int figure::create_node(int parent, double x, double y)
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

int figure::create_line(int parent, double x, double y)
{
    int child = create_node(parent, x, y);
    edge* e = new edge(edge::edge_line, parent, child);
    edges_.push_back(e);
    int eindex = static_cast<int>(edges_.size()) - 1;
    return eindex;
}

int figure::create_circle(int n1, int n2)
{
    edge* e = new edge(edge::edge_circle, n1, n2);
    edges_.push_back(e);
    int eindex = static_cast<int>(edges_.size()) - 1;
    return eindex;
}

int figure::create_circle(int parent, double x, double y)
{
    int child = create_node(parent, x, y);
    edge* e = new edge(edge::edge_circle, parent, child);
    edges_.push_back(e);
    int eindex = static_cast<int>(edges_.size()) - 1;
    return eindex;
}

int figure::create_image(int parent, double x, double y, int image_index)
{
    int child = create_node(parent, x, y);
    edge* e = new edge(edge::edge_image, parent, child);
    e->set_meta_index(image_index);
    edges_.push_back(e);
    int eindex = static_cast<int>(edges_.size()) - 1;
    return eindex;
}

bool figure::get_node_at_pos(int& an, double x, double y, int radius)
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

edge* figure::find_edge(int n1, int n2)
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

void figure::get_decendants(std::list<int>& decendants, int parent)
{
    node* pn = get_node(parent);
    const std::list<int>& children = pn->get_children();
    BOOST_FOREACH(int c, children) {
        decendants.push_back(c);
        get_decendants(decendants, c);
    }
}

bool figure::thinner()
{
    bool success = false;
    if (weight_ > MIN_WEIGHT) {
        weight_--;
        success = true;
    }
    return success;
}

bool figure::thicker()
{
    bool success = false;
    if (weight_ < MAX_WEIGHT ) {
        weight_++;
        success = true;
    }
    return success;
}

void figure::print(std::ostream& os) const
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

void figure::clone(figure* fig, const figure& other)
{
    // copy meta data
    fig->root_ = other.root_;
    fig->selected_ = other.selected_;
    fig->weight_ = other.weight_;
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
    meta_store_ = new meta_store(*other.meta_store_);
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

void figure::scale(double scale)
{
    node* rn = get_node(root_);
    double rx = rn->get_x();
    double ry = rn->get_y();

    for (unsigned n = 0; n < nodes_.size(); n++) {
        node* an = get_node(n);
        if (an != NULL) {
            an->set_x(an->get_x() * scale);
            an->set_y(an->get_y() * scale);
        }
    }

    // scaling will move all points, reposition to match last root node position
    double dx = rx - rn->get_x();
    double dy = ry - rn->get_y();
    move(dx, dy);
}

void figure::remove_decendants(int nindex)
{
    node* n = get_node(nindex);
    assert (n != NULL);
    const std::list<int>& children = n->get_children();

    BOOST_FOREACH(int child, children) {
        if (child != -1) {
            remove_decendants(child);    // remove all nodes below this one
            int eindex = get_edge(nindex, child);
            if (eindex != -1) {
                remove_edge(eindex);    // no nodes below, cut the edge
            }
            remove_node(child);
        }
    }

    // now clear the child list
    n->clear_children();

    // correct node references starting at child index (erase() moves them down by 1 index)
    fix_node_refs(nindex);
}

void figure::fix_node_refs(int nindex)
{
    // since nindex was deleted, all refs to nodes in the vector from nindex and up must be
    // decremented (edges)
    for(unsigned e = 0; e < edges_.size(); e++) {
        edge* en = get_edge(e);
        // assert(en != NULL);
        if (en != NULL) {
            int n1 = en->get_n1();
            if (n1 >= nindex) {
                en->set_n1(n1 - 1);
            }
            int n2 = en->get_n2();
            if (n2 >= nindex) {
                en->set_n2(n2 - 1);
            }
        }
    }

    // also need to fix node references
    for(unsigned n = 0; n < nodes_.size(); n++) {
        node* pn = get_node(n);
        assert(pn != NULL);
        if (pn != NULL) {
            // fix parent reference
            int parent = pn->get_parent();
            if (parent >= nindex) {
                pn->set_parent(parent - 1);
            }

            // fix child reference
            for (std::list<int>::iterator iter = pn->children_.begin(); iter != pn->children_.end(); iter++) {
                int c = *iter;
                if (c > nindex) {
                    *iter = c - 1;
                }
            }
        }
    }
}

void figure::remove_child(int child)
{
    node* n = get_node(child);
    assert(n != NULL);

    int parent = n->get_parent();
    node* pn = get_node(parent);
    if (pn != NULL) {
        pn->remove_child(child);
        nodes_.erase(nodes_.begin() + child);
    }

    int eindex = get_edge(parent, child);
    if (eindex != -1) {
        remove_edge(eindex);    // no nodes below, cut the edge
    }

    // correct node references starting at child index (erase() moves them down by 1 index)
    fix_node_refs(child);
}

void figure::remove_children(int nindex)
{
    remove_decendants(nindex);
    remove_child(nindex);
}

void figure::clone_subtree(figure* other, int s_index, int d_parent)
{
    std::cout << "clone_subtree: cloning " << s_index << std::endl;

    node* s_node = other->get_node(s_index);
    assert(s_node != NULL);
    const std::list<int>& s_children = s_node->get_children();

    node* d_node = new node(d_parent, s_node->get_x(), s_node->get_y());
    nodes_.push_back(d_node);
    int d_index = static_cast<int>(nodes_.size()) - 1;
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

/*
 * Private methods
 */

void figure::remove_node(int nindex)
{
    node* n = get_node(nindex);
    assert(n != NULL);
    disconnect(nindex); // remove it from the parent's child list
    // nodes_[nindex] = NULL;  // TODO: this should be erased from the vector!!!
    nodes_.erase(nodes_.begin() + nindex);  // Note: an immediate call to figure::fix_node_refs() is now required!
    delete n;
}

};  // namespace stan

// END of this file -----------------------------------------------------------
