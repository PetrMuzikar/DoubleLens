#ifndef QRNG_HPP

#include <exception>
#include <stdexcept>
#include "ML.hpp"
#include "Vec.hpp"

#define QRNG_LONG

#define QRNG_LONG

namespace ML
{
    
#ifdef QRNG_LONG
typedef LLInt QrngInt;
#else
typedef Int QrngInt;
#endif

class Qrng
{
public:
    Qrng(Int dim, Int seed=0) : dim_(dim), i_(0), x_(dim)
    {
        if (dim < 0)
        {
            throw std::length_error("Qrng: dimension could not be negative.");
        }
        x_ = 0;
    }

    Qrng(const Qrng& q) : dim_(q.dim_), i_(q.i_), x_(q.x_) {}

    Qrng& operator=(const Qrng& q)
    {
        dim_ = q.dim_;
        i_ = q.i_;
        x_.resize(q.dim_);
        x_ = q.x_;

        return *this;
    }

    virtual void get(VecNum& x) = 0;
    virtual void get(Num x[]) = 0;

    void reset()
    {
        i_ = 0;
        x_ = 0;
    }

protected:
    static const Int w_ = 32;
    Int dim_;
    QrngInt i_;
    VecNum x_;

};

class QrngHalton : public Qrng
{
public:
    QrngHalton(Int dim, Int seed=0) : Qrng(dim, seed) 
    {
        if (dim > nPrimes_)
        {
            throw std::length_error("QrngHalton: not enough primes, decrease the dimension!");
        }
    }

    virtual void get(VecNum& x);
    virtual void get(Num x[]);
 
private:
    static const Int nPrimes_ = 168;
    static const Int primes_[QrngHalton::nPrimes_];

    Num put(QrngInt i, Int d);
};


}

#endif
