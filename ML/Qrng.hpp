#ifndef VEC_HPP

#include <exception>
#include <stdexcept>
#include "ML.hpp"
#include "Vec.hpp"

namespace ML
{
    
class Qrng
{
public:
    Qrng(UInt dim, UInt seed=0) : dim_(dim), i_(0), x_(dim)
    {
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

    void reset()
    {
        i_ = 0;
        x_ = 0;
    }

protected:
    static const UInt w_ = 32;
    UInt dim_;
    ULLInt i_;
    VecNum x_;

};

class QrngHalton : public Qrng
{
public:
    QrngHalton(UInt dim, UInt seed=0) : Qrng(dim, seed) 
    {
        if (dim > nPrimes_)
        {
            throw std::length_error("QrngHalton: net enough primes, decrease the dimension!");
        }
    }

    virtual void get(VecNum& x);
 
private:
    static const UInt nPrimes_ = 168;
    static const UInt primes_[QrngHalton::nPrimes_];
};


}

#endif
