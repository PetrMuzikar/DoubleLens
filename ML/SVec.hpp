#ifndef SVEC_INCLUDED
#define SVEC_INCLUDED

#include <iostream>

#include "ML.hpp"

namespace ML
{

template <typename T, UInt N> class SVec;

template <typename T, UInt N>
std::ostream& operator<<(std::ostream& os, const SVec<T,N>& vec);

template <typename T, UInt N>
std::istream& operator>>(std::istream& is, SVec<T,N>& vec);

template <typename T, UInt N>
class SVec
{
public:
    SVec() {}

    SVec(const T* init)
    {
        copyData(N, init, data_);
    }

    SVec(const SVec& init)
    {
        copyData(N, init.data_, data_);
    }

    ~SVec() {}

    SVec& operator=(const SVec& left)
    {
        if (&left == this)
        {
            return *this;
        }

        copyData(N, left.data_, data_);

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

    friend std::ostream& operator<<(std::ostream& os, const SVec& vec)
    {
        for (UInt i = 0; i < N; ++i)
        {
            os << vec.data_[i] << " ";
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, SVec& vec)
    {
        for (UInt i = 0; i < N; ++i)
        {
            is >> vec.data_[i];
        }
        return is;
    }

    T* data()
    {
        return data_;
    }

    UInt size() const
    {
        return N;
    }

    void reverse()
    {
        SVec v(*this);
        for (UInt i = 0; i < N; ++i)
        {
            data_[i] = v[N-1-i];
        }
    }

private:
    T data_[N];

};

}

#endif // SVEC_INCLUDED
