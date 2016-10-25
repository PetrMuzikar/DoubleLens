#ifndef DOMAIN_HPP
#define DOMAIN_HPP

#include <cmath>
#include <complex>
#include <list>

typedef int Int;
typedef long long int LLInt;
typedef double Num;
typedef std::complex< Num > CNum;
//typedef std::list<Num> NumList;
//typedef std::list< NumList > NumLists;
//typedef std::list<CNum> CNumList;
//typedef std::list< CNumList > CNumLists;
//typedef std::list<Domain> DomainList;

//const Num PI = 4.0*atan(1);

class Domain
{
public:
    Domain() : n_(0), S_(0) {}
    Domain(Int n, Num S) : n_(n), S_(S) {}
    Int nPar() const { return n_; }
    //virtual Num area() const { return S_; }
    //virtual CNum randomPoint(Num r[]) const;
    Num area() const { return S_; }
    CNum randomPoint(Num r[]) const;
    //CNum randomPoint(const Num r1, const Num r2) const;
    virtual ~Domain() {}
//    friend std::ostream& operator<<(std::ostream& os, const Domain& dom);
//    friend std::istream& operator>>(std::istream& is, Domain& dom);
protected:
    Int n_;
    Num S_;
};

#endif
