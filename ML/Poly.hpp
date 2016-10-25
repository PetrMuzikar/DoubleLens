#ifndef POLY_HPP_INCLUDED
#define POLY_HPP_INCLUDED

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <exception>
#include <stdexcept>
#include <complex>
#include <cmath>

#include "ML.hpp"
#include "Vec.hpp"
#include "Util.hpp"

namespace ML
{

template <typename T> class Poly;

typedef Poly<Num> PolyNum;
typedef Poly<CNum> PolyCNum;

template <typename T>
std::ostream& operator<<(std::ostream& os, const Poly<T>& p);

template <typename T>
std::istream& operator>>(std::istream& is, Poly<T>& p);

/**
 * \class Poly
 *
 * A polynomial of type
 * a_0 + a_1*x + a_2*x^2 + ... + a_{n-1}*x^{n-1}
 * It has n coefficients!
 */
template <typename T> class Poly
{
public:
//    typedef T ValueType;
//    typedef Poly Type;

    Poly() {}

    Poly(UInt n) : coeff_(n) {}

    Poly(UInt n, const T* data) : coeff_(n, data) {}

    Poly(const Poly& p) : coeff_(p.coeff_) {}

    Poly& operator=(const Poly& p)
    {
        if (&p == this)
        {
            return *this;
        }
        coeff_ = p.coeff_;
        return *this;
    }

    ~Poly() {}

    const T& operator[](UInt k) const
    {
        return coeff_[k];
    }

    T& operator[](UInt k)
    {
        return coeff_[k];
    }

    T operator()(const T& x) const
    {
        return horner(x);
    }

    std::string operator()(const std::string& var) const
    {
        std::stringstream ss;
        const UInt n = size();

        switch (n)
        {
        case 0:
            return "";
        case 1:
            ss << printNumber(coeff_[0]);
            return ss.str();
        default:
            ss << '(' << printNumber(coeff_[0]) << ") + " << '(' << printNumber(coeff_[1]) << ")*" << var;
            for (UInt k = 2; k < n; ++k)
            {
                ss << " + (" << printNumber(coeff_[k]) << ")*" << var << "^" << k;
            }
            return ss.str();
        }
    }

    T* data()
    {
        return coeff_.data();
    }

    const T* data() const
    {
        return coeff_.data();
    }

    UInt size() const
    {
        return coeff_.size();
    }

    UInt degree() const
    {
        return coeff_.size() - 1;
    }

    void reverse()
    {
        coeff_.reverse();
    }

    void resize(const UInt newSize)
    {
        coeff_.resize(newSize);
    }

    void normalize()
    {
        const UInt n = degree();
        for (UInt k = 0; k < n; ++k)
        {
            coeff_[k] /= coeff_[n];
        }
        coeff_[n] = 1;
    }

//    void roots(Num prec, CVec& roots)
//    {
//        durker(roots, prec);
//    }

    friend std::ostream& operator<<(std::ostream& os, const Poly& p)
    {
        os << p("x");
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Poly& p)
    {
        for (UInt i = 0; i < p.size(); ++i)
        {
            is >> p[i];
        }
        return is;
    }

    void horner(const T& c, T& val, Poly& div) const
    {
        const Int n = degree();
        div = Poly(n); // has n coefficients => degree == n-1

        val = coeff_[n];
        div[n-1] = val;

        for (Int k = n-1; k >= 1; --k)
        {
            val = val * c + coeff_[k];
            div[k-1] = val;
        }
        val = val * c + coeff_[0];
    }

private:
    Vec<T> coeff_;

    T horner(const T& x) const
    {
        const Int n = degree();
        T res = coeff_[n];
        for (Int k = n-1; k >= 0; --k)
        {
            res = res * x + coeff_[k];
        }
        return res;
    }

};

}


#endif // POLY_HPP_INCLUDED
