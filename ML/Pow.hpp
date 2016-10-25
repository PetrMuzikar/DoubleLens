#ifndef POW_HPP_INCLUDED
#define POW_HPP_INCLUDED

#include "ML.hpp"

namespace ML
{

template <UInt N> struct Pow;

template <>
struct Pow<0>
{
    template <typename T>
    static T of(const T number)
    {
        return 1;
    }
};

template <>
struct Pow<1>
{
    template <typename T>
    static T of(const T number)
    {
        return number;
    }
};

template <>
struct Pow<2>
{
    template <typename T>
    static T of(const T number)
    {
        return number * number;
    }
};

template <>
struct Pow<3>
{
    template <typename T>
    static T of(const T number)
    {
        return number * number * number;
    }
};

template <UInt N>
struct Pow
{
    template <typename T>
    static T of(const T number)
    {
        return Pow<2>::of(Pow<(N>>1)>::of(number)) * Pow<N%2>::of(number);
    }
};

template <typename T>
inline T SQR(const T x)
{
    return x*x;
}

template <typename T>
inline T CUB(const T x)
{
    return x*x*x;
}

template <typename T>
inline T QUA(const T x)
{
    return Pow<4>::of(x);
}

template <typename T>
inline T QUI(const T x)
{
    return Pow<5>::of(x);
}

template <typename T>
inline T SEX(const T x)
{
    return Pow<6>::of(x);
}

}

#endif // POW_HPP_INCLUDED
