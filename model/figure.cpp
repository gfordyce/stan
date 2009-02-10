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
    BOOST_FOREACH(boost::shared_ptr<node> n, nodes_) {
        n->move(dx, dy);
    }
    return true;
}

void figure::traverse_copy( const boost::shared_ptr<node>& sn, boost::shared_ptr<node>& dn, const figure& sf)
{
    // sn and dn are copies of each other. if sn is special,
    // then dn must equivalently special.
    if (sn == sf.selected_)
        selected_ = dn;
    if (sn == sf.pivot_)
        pivot_ = dn;

    // now traverse children
    std::list< boost::shared_ptr< node > >& s_children = sn->get_children();
    BOOST_FOREACH(boost::shared_ptr<node> sc, s_children) {
        // create new dn child with dn as parent
        boost::shared_ptr<node> dc = create_node(dn, sn->get_x(), sn->get_y());
        traverse_copy(sc, dc, sf);
    }
}

};  // namespace stan
