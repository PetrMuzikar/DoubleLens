#ifndef DOMAIN_HPP
#define DOMAIN_HPP

#include <cmath>
#include <complex>

//#include "Constants.hpp"
#ifndef Num
typedef double Num;
#endif

#ifndef CNum
typedef std::complex<Num> CNum;
#endif

const Num PI = 4.0*atan(1);

struct Domain
{
    enum struct Type { RECTANGLE, ANNULUS };
    Type type;

    union 
    {
        Num xLow; 
        Num xC;
    };

    union 
    {
        Num xHigh;
        Num yC;
    };
    
    union 
    {
        Num yLow;
        Num rLow;
    };

    union 
    {
        Num yHigh;
        Num rHigh;
    };

    Num S;

    Num area();

    CNum randomPoint(Num r1, Num r2) const;

    friend std::ostream& operator<<(std::ostream& os, const Domain& dom);
    friend std::istream& operator>>(std::istream& is, Domain& dom);
};

#endif
