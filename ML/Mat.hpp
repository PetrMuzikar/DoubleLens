#ifndef MAT_HPP_INCLUDED
#define MAT_HPP_INCLUDED

#include <iostream>
#include <exception>
#include <stdexcept>

#include "ML.hpp"
#include "Util.hpp"

namespace ML
{

template <typename T> class Mat;

typedef Mat<Num> MatNum;
typedef Mat<CNum> MatCNum;
typedef Mat<Int> MatInt;
typedef Mat<LLInt> MatLLInt;

template <typename T>
std::ostream& operator<<(std::ostream& os, const Mat<T>& mat);

template <typename T>
std::istream& operator>>(std::istream& is, Mat<T>& mat);

template <typename T>
class Mat
{
public:
    Mat() : m_(0), n_(0), data_(0) {}

    Mat(UInt m, UInt n) : m_(m), n_(n), data_(m*n > 0 ? new T[m*n] : 0) {}

    Mat(UInt m, UInt n, T* init) : m_(m), n_(n), data_(m*n > 0 ? new T[m*n] : 0)
    {
        copyData(m*n, init, data_);
    }

    Mat(const Mat& init) : m_(init.m_), n_(init.n_), data_(init.m_*init.n_ > 0 ? new T[init.m_*init.n_] : 0)
    {
        copyData(init.m_*init.n_, init.data_, data_);
    }

    ~Mat()
    {
        delete [] data_;
    }

    Mat& operator=(const Mat& left)
    {
        if (&left == this)
        {
            return *this;
        }

        if (size() != left.size())
        {
            delete [] data_;
            data_ = left.size() > 0 ? new T[left.size()] : 0;
        }

        if (m_ != left.m_)
        {
            m_ = left.m_;
        }

        if (n_ != left.n_)
        {
            n_ = left.n_;
        }

        copyData(left.size(), left.data_, data_);

        return *this;
    }

    const T* operator[](UInt i) const
    {
        return &data_[n_*i];
    }
    T* operator[](UInt i)
    {
        return &data_[n_*i];
    }

    const T& operator()(UInt i, UInt j) const
    {
        return data_[n_*i+j];
    }
    T& operator()(UInt i, UInt j)
    {
        return data_[n_*i+j];
    }

    const Mat& operator+(const Mat& other)
    {
        if ((m_ != other.rows()) || (n_ != other.cols()))
        {
            throw std::runtime_error("ML::Mat::operator+: incompatible matrices!");
        }
        for (UInt i = 0; i < m_; ++i)
        {
            for (UInt j = 0; j < n_; ++j)
            {
                data_[n_*i+j] += other(i, j);
            }
        }

        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Mat& mat)
    {
        for (UInt i = 0; i < mat.m_; ++i)
        {
            for (UInt j = 0; j < mat.n_; ++j)
            {
                os << mat(i,j) << " ";
            }
            os << std::endl;
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Mat& mat)
    {
        for (UInt i = 0; i < mat.size(); ++i)
        {
            is >> mat.data_[i];
        }
        return is;
    }

    UInt size() const
    {
        return m_ * n_;
    }
    UInt rows() const
    {
        return m_;
    }
    UInt cols() const
    {
        return n_;
    }

    void resize(UInt m, UInt n)
    {
        UInt siz = m*n;

        if (size() != siz)
        {
            delete [] data_;
            data_ = siz > 0 ? new T[siz] : 0;
        }

        if (m_ != m)
        {
            m_ = m;
        }

        if (n_ != n)
        {
            n_ = n;
        }
    }

private:
    UInt m_, n_;
    T* data_;
};

}

#endif // MAT_HPP_INCLUDED
