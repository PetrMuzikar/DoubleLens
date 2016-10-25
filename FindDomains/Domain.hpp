#ifndef DOMAIN_HPP
#define DOMAIN_HPP

#include <cmath>
#include <complex>
#include <list>

struct Domain;

typedef int Int;
typedef long long int LLInt;
typedef double Num;
typedef std::complex< Num > CNum;
typedef std::list<Num> NumList;
typedef std::list< NumList > NumLists;
typedef std::list<CNum> CNumList;
typedef std::list< CNumList > CNumLists;
typedef std::list<Domain> DomainList;

const Num PI = 4.0*atan(1);

enum DomainType { RECTANGLE, ANNULUS };

struct Domain
{
    DomainType type;

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

    Num area() const;

    CNum randomPoint(const Num r1, const Num r2) const;

    friend std::ostream& operator<<(std::ostream& os, const Domain& dom);
    friend std::istream& operator>>(std::istream& is, Domain& dom);
};

#endif
