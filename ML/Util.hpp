#ifndef UTIL_HPP_INCLUDED
#define UTIL_HPP_INCLUDED

#include <complex>
#include <sstream>
#include <string>
#include <iomanip>

namespace ML
{

template <typename T>
void copyData(UInt n, const T* from, T* to)
{
    for (UInt i = 0; i < n; ++i)
    {
        to[i] = from[i];
    }
}

inline std::string printNumber(const CNum& num)
{
    std::stringstream ss;
    ss << real(num) << std::showpos << imag(num) << "*I";
    return ss.str();
}

template <typename T>
std::string printNumber(const T& num)
{
    std::stringstream ss;
    ss << num;
    return ss.str();
}

inline Num combNum(const Num a, const Int k)
{
    Num res = 1.0;

    for (Int i = 0; i < k; i++)
    {
        res *= (a - i) / (k - i);
    }
    return res;
}

}

#endif // UTIL_HPP_INCLUDED
