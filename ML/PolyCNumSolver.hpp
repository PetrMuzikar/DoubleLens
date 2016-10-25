#ifndef CPOLYSOLVER_HPP_INCLUDED
#define CPOLYSOLVER_HPP_INCLUDED

#include <stdexcept>

#include "ML.hpp"
#include "Poly.hpp"
#include "SVec.hpp"
#include "Mat.hpp"
#include "Util.hpp"

namespace ML
{

class PolyCNumSolver
{
public:
    enum Method
    {
        LEHMER_SCHUR,
        DURAND_KERNER,
        COMBINED
    };

    PolyCNumSolver() {}

    PolyCNumSolver(const UInt n, const CNum* p) : p_(n, p)
    {
        normalize();
    }

    PolyCNumSolver(const UInt n, const Num* p) : p_(n)
    {
        for (UInt i = 0; i < n; ++i)
        {
            p_[i] = CNum(p[i]);
        }
        normalize();
    }

    PolyCNumSolver(const PolyCNum& p) : p_(p) {}

    PolyCNumSolver(const PolyCNumSolver& pcns) : p_(pcns.p_) {}

    ~PolyCNumSolver() {}

    PolyCNumSolver& operator=(const PolyCNumSolver& p)
    {
        if (&p == this)
        {
            return *this;
        }

        p_ = p.p_;

        return *this;
    }

    void setPoly(const PolyCNum& p)
    {
        p_ = p;
        normalize();
    }

    void roots(const Num absPrec, const Num relPrec, const Method m, VecCNum& roots)
    {
        Num precInit = 1e-5;

        switch (m)
        {
        case LEHMER_SCHUR :
            rootsLehSch(absPrec, relPrec, roots);
            break;
        case DURAND_KERNER :
            rootsDurKer(absPrec, relPrec, roots);
            break;
        case COMBINED :
        {
            rootsLehSch(precInit, precInit, roots);
            VecCNum rootsInit(roots);
            rootsDurKer(absPrec, relPrec, rootsInit, roots);
            break;
        }
        default:
            throw std::runtime_error("PolyCNumSolver::roots: error - Bad method!");
        }
    }

private:
    PolyCNum p_;

    void normalize(const Int deg, CNum p[])
    {
        using std::abs;

        Num maxi = abs(p[0]);
        for (Int i = 1; i <= deg; ++i)
        {
            if (abs(p[i]) > maxi)
            {
                maxi = abs(p[i]);
            }
        }
        for (Int i = 0; i <= deg; ++i)
        {
            p[i] /= maxi;
        }
    }
    void normalize()
    {
        normalize(p_.degree(), &p_[0]);
    }

    CNum horner(const CNum p[], const Int deg, const CNum c, CNum div[]) const
    {
        if (deg < 0)
        {
            throw std::runtime_error("PolyCNumSolver::horner: error - The degree of the polynomial cannot be < 0!");
        }
        else if (deg == 0)
        {
            return p[0];
        }

        div[deg-1] = p[deg];
        for (Int i = deg - 2; i >= 0; --i)
        {
            div[i] = c * div[i+1] + p[i+1];
        }

        return c * div[0] + p[0];
    }

    CNum horner(const CNum p[], const Int deg, const CNum c) const
    {
        CNum now, old;

        if (deg < 0)
        {
            throw std::runtime_error("PolyCNumSolver::horner: error - The degree of the polynomial cannot be < 0!");
        }
        else if (deg == 0)
        {
            return p[0];
        }

        old = p[deg];
        for (Int i = deg - 2; i >= 0; --i)
        {
            now = c * old + p[i+1];
            old = now;
        }

        return c * old + p[0];
    }

    void transf(const CNum p[], const Int deg, CNum pTr[], Int& degTr) const;

    void linTransf(const PolyCNum& p, const CNum rho, const CNum c, PolyCNum& pTr) const;
    void linTransf(const CNum p[], const Int deg, const CNum rho, const CNum c, CNum pTr[]) const
    {
        const PolyCNum poly(deg+1, p);
        PolyCNum q(deg+1);
        linTransf(poly, rho, c, q);
        for (Int i = 0; i <= deg; ++i)
        {
            pTr[i] = q[i];
        }
    }

    Int decide(const PolyCNum& p, const Num rho, const CNum c) const;
    Int decide(const CNum p[], const Int deg, const Num rho, const CNum c) const
    {
        const PolyCNum poly(deg+1, p);
        return decide(poly, rho, c);
    }

    CNum rootLehSch(const PolyCNum& p, const Num absPrec, const Num repPrec) const;
    void rootsLehSch(const Num absPrec, const Num relPrec, VecCNum& roots) const;

    void rootsDurKer(const Num absPrec, const Num relPrec, const VecCNum& rootsInit, VecCNum& roots);
    void rootsDurKer(const Num absPrec, const Num relPrec, VecCNum& roots)
    {
        VecCNum rootsInit(p_.degree());
        for (UInt i = 0; i < p_.degree(); i++)
        {
            rootsInit[i] = CNum(i)*(0.01 + 0.01*I);
        }
        rootsDurKer(absPrec, relPrec, rootsInit, roots);
    }
};

}

#endif // CPOLYSOLVER_HPP_INCLUDED
