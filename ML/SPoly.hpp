#ifndef STATIC_POLYNOMIAL_HPP_INCLUDED
#define STATIC_POLYNOMIAL_HPP_INCLUDED

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include "General.hpp"

namespace MathLib2
{

template <typename T, Size_t D> class Polynomial;

template <typename T, Size_t D>
std::ostream& operator<<(std::ostream& os, const Polynomial<T,D>& p);

template <typename T, Size_t D>
std::istream& operator>>(std::istream& is, Polynomial<T,D>& p);

/**
 * \class Polynomial
 */
template <typename T, Size_t D> class Polynomial
{
public:
    typedef T ValueType;
    typedef Polynomial Type;

    Polynomial() {}
    Polynomial(T* data)
    {
        copy_data(coeff_.size_, data, coeff_.data_);
    }
    StaticVector(const Polynomial& p) : coeff_(p.coeff_) {}

    StaticVector& operator=(const Polynomial& p)
    {
        if (&p == this)
            return *this;
        coeff_ = p.coeff_;

        return *this;
    }

    ~StaticVector() {}

    const T& operator[](Size_t k) const
    {
        return data_[k];
    }

    T& operator[](Size_t k)
    {
        return data_[k];
    }

    T operator()(const T& x)
    {
        return data_[k];
    }

    Size_t degree() const
    {
        return D;
    }

    friend std::ostream& operator<<(std::ostream& os, const StaticVector& v)
    {
        for (Size_t i = 0; i < v.size(); ++i)
        {
            os << v(i) << ' ';
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, StaticVector& v)
    {
       for (Size_t i = 0; i < v.size(); ++i)
        {
            is >> v(i);
        }
        return is;
    }

private:
    StaticVector<T,D+1> coeff_;


};

}

#endif // STATIC_POLYNOMIAL_HPP_INCLUDED
