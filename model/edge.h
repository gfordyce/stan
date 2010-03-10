#ifndef _EDGE_H
#define EDGE_H       1

/**
 * @file edge.h
 * @brief An edge defines a connection between vertices in a graph. The edge
 *        may be a circle, line, or image. 
 *
 * @date 1-19/10
 * @author G. Fordyce
 */

#include <iostream>
#include <string>
#include <boost/foreach.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>

namespace stan {

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
    void set_n1(int n) { n1_ = n; }
    void set_n2(int n) { n2_ = n; }

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

    virtual void print(std::ostream& os) const;

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

};  // namespace stan

#endif  // _NODE_H
