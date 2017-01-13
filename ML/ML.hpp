#ifndef ML_HPP_INCLUDED
#define ML_HPP_INCLUDED

#include <complex>
#include <cmath>

namespace ML
{

typedef int Int;
typedef size_t UInt;
typedef unsigned long ULong;
typedef long long int LLInt;
typedef unsigned long long int ULLInt;
typedef double Num;
typedef std::complex<Num> CNum;

const CNum I(0,1);
const CNum ZERO(0,0);
const CNum ONE(1,0);
const Num PI = 4.0 * atan(1.0);
const Num EPS = 1e-12;

using std::exp;
using std::abs;

}

#endif // MATHLIB_HPP_INCLUDED
