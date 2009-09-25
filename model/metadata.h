#ifndef _META_DATA_H
#define _META_DATA_H       1

/**
 * @file meta_data.h
 * @brief Manage meta data associated with figures and animations.
 * @date 7-13-09
 * @author G. Fordyce
 */

#include <iostream>
#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>

namespace stan {

/**
 * Metadata types
 */
typedef enum {
    META_NONE,
    META_IMAGE,
    META_SOUND,
} meta_type;

/**
 * Store meta metadata such as path and opaque meta pointer.
 */
class meta_data
{
public:

    meta_data() :
        path_(),
        meta_ptr_(NULL),
        type_(META_NONE)
    {
    }

    meta_data(std::string path, void* ptr, meta_type type) :
        path_(path),
        meta_ptr_(ptr),
        type_(type)
    {
    }

    // copy constructor
    meta_data(const meta_data& other)
    {
        clone(this, other);
    }

    // assignment operator
    meta_data& operator=(const meta_data& other)
    {
        if (this != &other)
        {
            clone(this, other);
        }
        return *this;
    }

    void clone(meta_data* md, const meta_data& other)
    {
        md->path_ = other.path_;
        md->meta_ptr_ = other.meta_ptr_;
        md->type_ = other.type_;
    }

    virtual ~meta_data() {}

    // accessors
    void set_path(std::string& path) { path_ = path; }
    std::string get_path() { return path_; }
    void set_meta_ptr(void *ptr) { meta_ptr_ = ptr; }
    void* get_meta_ptr() { return meta_ptr_; }
    void set_type(meta_type type) { type_ = type; }
    meta_type get_type() { return type_; }

    virtual void print(std::ostream& os) const
    {
        os << "Meta data:" << std::endl;
        os << "   path_ = " << path_ << std::endl;
        os << "   meta_ptr_ = " << meta_ptr_ << std::endl;
        os << "   type_ = " << type_ << std::endl;
    }

protected:
    friend class boost::serialization::access;
    friend std::ostream& operator<<(std::ostream &os, const meta_data &imd);

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
	{
        ar & BOOST_SERIALIZATION_NVP(path_);
        ar & BOOST_SERIALIZATION_NVP(type_);
    }

private:
    std::string path_;
    void *meta_ptr_;
    meta_type type_;    // image, sound, etc...
};

/**
 * Manage storage and caching of meta data for figures and animations.
 * Meta data can be referenced by an index from model elements
 */
class meta_store
{
public:
    meta_store() :
        meta_data_()
    {
    }

    // copy constructor
    meta_store(const meta_store& other)
    {
        clone(this, other);
    }

    // assignment operator
    meta_store& operator=(const meta_store& other)
    {
        if (this != &other) {
            clone(this, other);
        }
        return *this;
    }

    void clone(meta_store* meta, const meta_store& other)
    {
        std::cout << "meta_store::clone() called." << std::endl;
        for (unsigned i = 0; i < other.meta_data_.size(); i++) {
            meta_data* data = other.meta_data_[i];
            if (data != NULL) {
                meta_data* new_data = new meta_data(*data);
                meta->meta_data_.push_back(new_data);
            }
        }
    }

    virtual ~meta_store()
    {
        meta_data_.clear();
    }

    /**
     * Register a new meta for this figure. Image path
     * and meta ptr will be added to the meta data table.
     * @param path Full path to meta file.
     * @param meta_ptr Opaque meta pointer used by view.
     * @param type The metadata type.
     * @return int Index associated with this meta.
     */
    int add_meta_data(std::string& path, void* meta_ptr, meta_type type)
    {
        meta_data* data = new meta_data(path, meta_ptr, type);
        int index = static_cast<int>(meta_data_.size());
        meta_data_.push_back(data);
        return index;
    }

    /**
     * Return meta data associated with given index.
     * @note Figure model references an meta by this
     *       index.
     * @param index The meta index.
     * @return Image data on success or NULL if not found.
     */
    meta_data* get_meta_data(int index)
    {
        meta_data* data = NULL;

        if (index < static_cast<int>(meta_data_.size())) {
            data = meta_data_[index];
            // printf("get_meta_data (this = 0x%08X, meta_ptr = 0x%08X)\n", this, data->get_meta_ptr());
        }

        return data;
    }

    std::vector<meta_data*>& get_meta_table()
    {
        return meta_data_;
    }

    virtual void print(std::ostream& os) const
    {
        os << "Image store with address: " << this << std::endl;
        BOOST_FOREACH(meta_data* md, meta_data_) {
            os << *md << std::endl;
        }
    }

protected:
    friend class boost::serialization::access;
    friend std::ostream& operator<<(std::ostream &os, const meta_store &ms);

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
	{
        ar & BOOST_SERIALIZATION_NVP(meta_data_);
    }

private:
    std::vector<meta_data*> meta_data_;
};

};  // namespace stan

#endif  // _META_DATA_H
