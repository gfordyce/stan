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

bool figure::move(double dx, double dy)
{
    BOOST_FOREACH(node* n, nodes_) {
        n->move(dx, dy);
    }
    return true;
}

void figure::traverse_copy( node* sn, node* dn, figure& sf)
{
    std::cout << "traverse_copy" << std::endl;

    // sn and dn are copies of each other. if sn is special,
    // then dn must equivalently special.
    if (sn == sf.selected_)
        selected_ = dn;
    if (sn == sf.pivot_)
        pivot_ = dn;

    // now traverse children
    const std::list<node*>& s_children = sn->get_children();
    BOOST_FOREACH(node* sc, s_children) {
        // create new dn child with dn as parent
        node* dc = create_node(dn, sn->get_x(), sn->get_y());
        traverse_copy(sc, dc, sf);
    }
}

};  // namespace stan
