#ifndef INPUTRAYS_HPP_INCLUDED
#define INPUTRAYS_HPP_INCLUDED

#include <stdexcept>
#include <ctime>
#include <string>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_qrng.h>

#include "ML/ML.hpp"
#include "ML/Point.hpp"
#include "ML/Qrng.hpp"

#define SOBOL_DIMS 2

//#define DEBUG_RANDOM

//#define RANDOM_RAYS
//#define SOBOL_RANDOM_SEQUENCE

//#define GSL_SOBOL

using namespace ML;

enum DomainType { RECTANGLE, ANNULUS };

class InputRays
{
public:
    InputRays(bool printRays = false);
    InputRays(DomainType t, Num xLow, Num xHigh, Num yLow, Num yHigh, LLInt n,
        const std::string& prefix = "# ", bool printRays = false);

    InputRays(const InputRays& ir);

    ~InputRays()
    {
#if defined(SOBOL_RANDOM_SEQUENCE) && defined(GSL_SOBOL)
        gsl_qrng_free(q_);
#elif defined(RANDOM_RAYS)
        gsl_rng_free(r_);
#endif
    }

    InputRays& operator=(const InputRays& rhs);

    LLInt getNumRays() const
    {
        return n_;
    }
    PointNum getRay(LLInt i);

    void setOutputPrefix(const std::string& pref)
    {
        prefix_ = pref;
    }

    std::string getOutputPrefix() const
    {
        return prefix_;
    }

    bool printRays() const
    {
        return printRays_;
    }

    void printRays(bool what)
    {
        printRays_ = what;
    }

    Num area() const;

    std::string printInput() const;

    friend std::ostream& operator<<(std::ostream& os, const InputRays& ir);
    friend std::istream& operator>>(std::istream& is, InputRays& ir);

#ifdef SOBOL_RANDOM_SEQUENCE
    LLInt getStart() const
    {
        return startOfSequence_;
    }

    void setStart(LLInt start);
#elif defined(RANDOM_RAYS)
    Int getSeed() const
    {
        return seed_;
    }

    void setSeed(Int seed)
    {
        gsl_rng_set(r_, seed);
    }
#endif

private:
    DomainType type_;

    union
    {
        Num xLow_;
        Num xC_;
    };

    union
    {
        Num xHigh_;
        Num yC_;
    };

    union
    {
        Num yLow_;
        Num rLow_;
    };

    union
    {
        Num yHigh_;
        Num rHigh_;
    };

    LLInt n_;

    std::string prefix_;
    bool printRays_;

#if defined(SOBOL_RANDOM_SEQUENCE)
    LLInt startOfSequence_;
#ifdef GSL_SOBOL
    gsl_qrng* q_;
#else
    QrngSobol qs_;
#endif

#elif defined(RANDOM_RAYS)
    Int seed_;
    gsl_rng* r_;
#endif

    void initRandom()
    {
#if defined(SOBOL_RANDOM_SEQUENCE)
#ifdef GSL_SOBOL
        q_ = gsl_qrng_alloc(gsl_qrng_sobol, SOBOL_DIMS);
#else
        qs_ = QrngSobol(SOBOL_DIMS);
#endif
        startOfSequence_ = 0;

#elif defined(RANDOM_RAYS)
        r_ = gsl_rng_alloc(gsl_rng_taus);
#ifdef DEBUG_RANDOM
        seed_ = 0;
#else
        seed_ = Int(time(0));
#endif
        gsl_rng_set(r_, seed_);
#endif
    }

    void copyRandom(const InputRays& ir)
    {
#if defined(SOBOL_RANDOM_SEQUENCE)
#ifdef GSL_SOBOL
        q_ = gsl_qrng_clone(ir.q_);
#else
        qs_ = ir.qs_;
#endif
        startOfSequence_ = ir.startOfSequence_;

#elif defined(RANDOM_RAYS)
        r_ = gsl_rng_clone(ir.r_);
        seed_ = ir.seed_;
#endif
    }

};

#endif // INPUTRAYS_HPP_INCLUDED
