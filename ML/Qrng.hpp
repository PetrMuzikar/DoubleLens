#ifndef QRNG_HPP

#include <exception>
#include <stdexcept>
#include "ML.hpp"
#include "Vec.hpp"
#include "Mat.hpp"

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
    Qrng(Int dim, Int seed=0) : dim_(dim), i_(0)
    {
        if (dim < 0)
        {
            throw std::length_error("Qrng: dimension could not be negative.");
        }
    }

    Qrng(const Qrng& q) : dim_(q.dim_), i_(q.i_) {}

    Qrng& operator=(const Qrng& q)
    {
        dim_ = q.dim_;
        i_ = q.i_;

        return *this;
    }

    virtual void get(VecNum& x) = 0;
    virtual void get(Num x[]) = 0;

    void reset()
    {
        i_ = 0;
    }

protected:
    Int dim_;
    QrngInt i_;
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

    QrngHalton(const QrngHalton& rhs) : Qrng(rhs) {}

    QrngHalton& operator=(const QrngHalton& rhs)
    {
        if (&rhs == this)
        {
            return *this;
        }
        dim_ = rhs.dim_;
        i_ = rhs.i_;

        return *this;
    }        

    virtual void get(VecNum& x);
    virtual void get(Num x[]);
 
private:
    static const Int nPrimes_ = 168;
    static const Int primes_[QrngHalton::nPrimes_];

    Num put(QrngInt i, Int d);
};

class QrngSobol : public Qrng
{
public:
    QrngSobol(Int dim, Int seed=0) : Qrng(dim, seed)
    {
        if (dim > maxDim_)
        {
            throw std::length_error("QrngSobol: too much dimensions!");
        }
        x_ = Vec< QrngInt >(dim_);
        m_ = Mat< QrngInt >(dim_, w_);

        init();
    }

    QrngSobol(const QrngSobol& rhs) : Qrng(rhs), m_(rhs.m_) {}

    QrngSobol& operator=(const QrngSobol& rhs)
    {
        if (&rhs == this)
        {
            return *this;
        }
        dim_ = rhs.dim_;
        i_ = rhs.i_;

        x_.resize(rhs.dim_);
        x_ = rhs.x_;
        m_ = rhs.m_;

        return *this;
    }        

    virtual void get(Num x[]);
    virtual void get(VecNum& x)
    {
        get(x.data());
    }

    void reset();

private:
    static const Int w_ = 32;
    static const Int mStart_[];
    static const Int s_[];
    static const Int a_[];
    static const Int maxDim_;

    Vec< QrngInt > x_;
    Mat< QrngInt > m_;

    void init();

    //static QrngInt gray(QrngInt i)
    //{
    //    return i ^ (i >> 1);
    //}
};

}

#endif
