#ifndef _IMAGE_DATA_H
#define _IMAGE_DATA_H       1

/**
 * @file image_data.h
 * @brief Manage image data associated with figures and animations.
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
 * Store image metadata such as path and opaque image pointer.
 */
class image_data
{
public:
    image_data() :
        path_(),
        image_ptr_(NULL)
    {
    }

    image_data(std::string path, void* ptr) :
        path_(path),
        image_ptr_(ptr)
    {
    }

    // copy constructor
    image_data(const image_data& other)
    {
        clone(this, other);
    }

    // assignment operator
    image_data& operator=(const image_data& other)
    {
        if (this != &other)
        {
            clone(this, other);
        }
        return *this;
    }

    void clone(image_data* imgd, const image_data& other)
    {
        imgd->path_ = other.path_;
        imgd->image_ptr_ = other.image_ptr_;
    }

    virtual ~image_data() {}

    // accessors
    void set_path(std::string& path) { path_ = path; }
    std::string get_path() { return path_; }
    void set_image_ptr(void *ptr) { image_ptr_ = ptr; }
    void* get_image_ptr() { return image_ptr_; }

    virtual void print(std::ostream& os) const
    {
        os << "Image data:" << std::endl;
        os << "   path_ = " << path_ << std::endl;
        os << "   image_ptr_ = " << image_ptr_ << std::endl;
    }

protected:
    friend class boost::serialization::access;
    friend std::ostream& operator<<(std::ostream &os, const image_data &imd);

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
	{
        ar & BOOST_SERIALIZATION_NVP(path_);
    }

private:
    std::string path_;
    void *image_ptr_;
};

/**
 * Manage storage and caching of image data for figures and animations.
 * Images can be referenced by an index from model elements and metadata
 * for an image is accessible through this class.
 */
class image_store
{
public:
    image_store() :
        image_data_()
    {
    }

    // copy constructor
    image_store(const image_store& other)
    {
        clone(this, other);
    }

    // assignment operator
    image_store& operator=(const image_store& other)
    {
        if (this != &other) {
            clone(this, other);
        }
        return *this;
    }

    void clone(image_store* imgs, const image_store& other)
    {
        std::cout << "image_store::clone() called." << std::endl;
        for (unsigned i = 0; i < other.image_data_.size(); i++) {
            image_data* data = other.image_data_[i];
            if (data != NULL) {
                image_data* new_data = new image_data(*data);
                imgs->image_data_.push_back(new_data);
            }
        }
    }

    virtual ~image_store()
    {
        image_data_.clear();
    }

    /**
     * Register a new image for this figure. Image path
     * and image ptr will be added to the image data table.
     * @param path Full path to image file.
     * @param image_ptr Opaque image pointer used by view.
     * @return int Index associated with this image.
     */
    int add_image_data(std::string& path, void* image_ptr)
    {
        image_data* data = new image_data(path, image_ptr);
        int index = static_cast<int>(image_data_.size());
        image_data_.push_back(data);
        return index;
    }

    /**
     * Return image data associated with given index.
     * @note Figure model references an image by this
     *       index.
     * @param index The image index.
     * @return Image data on success or NULL if not found.
     */
    image_data* get_image_data(int index)
    {
        image_data* data = NULL;

        if (index < static_cast<int>(image_data_.size())) {
            data = image_data_[index];
            // printf("get_image_data (this = 0x%08X, image_ptr = 0x%08X)\n", this, data->get_image_ptr());
        }

        return data;
    }

    std::vector<image_data*>& get_image_table()
    {
        return image_data_;
    }

    virtual void print(std::ostream& os) const
    {
        os << "Image store with address: " << this << std::endl;
        BOOST_FOREACH(image_data* imd, image_data_) {
            os << *imd << std::endl;
        }
    }

protected:
    friend class boost::serialization::access;
    friend std::ostream& operator<<(std::ostream &os, const image_store &ims);

	template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
	{
        ar & BOOST_SERIALIZATION_NVP(image_data_);
    }

private:
    std::vector<image_data*> image_data_;
};

};  // namespace stan

#endif  // _IMAGE_DATA_H
