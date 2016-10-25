#ifndef VEC_HPP_INCLUDED
#define VEC_HPP_INCLUDED

#include <iostream>

#include "ML.hpp"
#include "Util.hpp"
#include "Point.hpp"

namespace ML
{

template <typename T> class Vec;

typedef Vec<Num> VecNum;
typedef Vec<CNum> VecCNum;
typedef Vec<PointNum> VecPointNum;
typedef Vec<PointInt> VecPointInt;

template <typename T>
std::ostream& operator<<(std::ostream& os, const Vec<T>& vec);

template <typename T>
std::istream& operator>>(std::istream& is, Vec<T>& vec);

template <typename T>
class Vec
{
public:
    Vec() : size_(0), data_(0) {}

    Vec(UInt n) : size_(n), data_(n > 0 ? new T[n] : 0) {}

    Vec(UInt n, const T* init) : size_(n), data_(n > 0 ? new T[n] : 0)
    {
        copyData(n, init, data_);
    }

    Vec(const Vec& init) : size_(init.size_), data_(init.size_ > 0 ? new T[init.size_] : 0)
    {
        copyData(init.size_, init.data_, data_);
    }

    ~Vec()
    {
        delete [] data_;
    }

    Vec& operator=(const Vec& left)
    {
        if (&left == this)
        {
            return *this;
        }

        if (size_ != left.size_)
        {
            delete [] data_;
            size_ = left.size_;
            data_ = left.size_ > 0 ? new T[left.size_] : 0;
        }

        copyData(left.size_, left.data_, data_);

        return *this;
    }

    const T& operator[](UInt i) const
    {
        return data_[i];
    }
    T& operator[](UInt i)
    {
        return data_[i];
    }

    const T& operator()(UInt i) const
    {
        return data_[i];
    }
    T& operator()(UInt i)
    {
        return data_[i];
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec& vec)
    {
        for (UInt i = 0; i < vec.size_; ++i)
        {
            os << vec.data_[i] << " ";
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Vec& vec)
    {
        for (UInt i = 0; i < vec.size_; ++i)
        {
            is >> vec.data_[i];
        }
        return is;
    }

    T* data()
    {
        return data_;
    }

    const T* data() const
    {
        return data_;
    }

    UInt size() const
    {
        return size_;
    }

    void resize(UInt n)
    {
        if (size_ != n)
        {
            delete [] data_;
            size_ = n;
            data_ = n > 0 ? new T[n] : 0;
        }
    }

    void reverse()
    {
        Vec v(*this);
        for (UInt i = 0; i < size_; ++i)
        {
            data_[i] = v[size_-1-i];
        }
    }

private:
    UInt size_;
    T* data_;

};

}

#endif // VEC_HPP_INCLUDED
