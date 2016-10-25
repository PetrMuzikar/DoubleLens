#ifndef SMAT_HPP_INCLUDED
#define SMAT_HPP_INCLUDED

#include <iostream>

#include "ML.hpp"
#include "Util.hpp"

namespace ML
{

template <typename T, UInt M, UInt N> class SMat;

template <typename T, UInt M, UInt N>
std::ostream& operator<<(std::ostream& os, const SMat<T,M,N>& mat);

template <typename T, UInt M, UInt N>
std::istream& operator>>(std::istream& is, SMat<T,M,N>& mat);

template <typename T, UInt M, UInt N>
class SMat
{
public:
    SMat() {}

    SMat(const T* init)
    {
        copyData(size(), init, data_);
    }

    SMat(const SMat& init)
    {
        copyData(size(), init.data_, data_);
    }

    ~SMat() {}

    SMat& operator=(const SMat& left)
    {
        if (&left == this)
        {
            return *this;
        }

        copyData(left.size(), left.data_, data_);

        return *this;
    }

    const T* operator[](UInt i) const
    {
        return &data_[M*i];
    }
    T* operator[](UInt i)
    {
        return &data_[M*i];
    }

    const T& operator()(UInt i, UInt j) const
    {
        return data_[M*i+j];
    }
    T& operator()(UInt i, UInt j)
    {
        return data_[M*i+j];
    }

    friend std::ostream& operator<<(std::ostream& os, const SMat& mat)
    {
        for (UInt i = 0; i < mat.rows(); ++i)
        {
            for (UInt j = 0; j < mat.cols(); ++j)
            {
                os << mat(i,j) << " ";
            }
            os << std::endl;
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, SMat& mat)
    {
        for (UInt i = 0; i < mat.size(); ++i)
        {
            is >> mat.data_[i];
        }
        return is;
    }

    UInt size() const
    {
        return M * N;
    }
    UInt rows() const
    {
        return M;
    }
    UInt cols() const
    {
        return N;
    }

private:
    T data_[M*N];

};

}


#endif // SMAT_HPP_INCLUDED
