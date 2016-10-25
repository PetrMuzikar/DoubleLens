#ifndef GRAVLENS_HPP_INCLUDED
#define GRAVLENS_HPP_INCLUDED

#include <iostream>
#include <iomanip>

#include "ML/ML.hpp"
#include "ML/Point.hpp"
#include "ML/Vec.hpp"
#include "ML/Mat.hpp"
#include "Constants.hpp"

using namespace ML;

class GravLens
{

public:
    GravLens() : M_M_Sun_(1.0), mu1_(0.5), Dskpc_(10.0), d_(1.0), d1_(0.5), beta_(0.1),
        nrE_(500), prec_(1e-5)
    {
        recalcDependentParams();
    }

    GravLens(Num M_M_Sun, Num mu1, Num Dskpc, Num d, Num d1, Num beta, Num nrE, Num prec) :
        M_M_Sun_(M_M_Sun), mu1_(mu1), Dskpc_(Dskpc), d_(d), d1_(d1), beta_(beta), nrE_(nrE), prec_(prec)
    {
        recalcDependentParams();
    }

    GravLens(const GravLens& gl) : M_M_Sun_(gl.M_M_Sun_), mu1_(gl.mu1_), Dskpc_(gl.Dskpc_),
        d_(gl.d_), d1_(gl.d1_), beta_(gl.beta_), nrE_(gl.nrE_), prec_(gl.prec_)
    {

        recalcDependentParams();
    }

    ~GravLens() {}

    GravLens& operator=(const GravLens& gl)
    {
        if (&gl == this)
        {
            return *this;
        }

        M_M_Sun_ = gl.M_M_Sun_;
        mu1_ = gl.mu1_;
        Dskpc_ = gl.Dskpc_;
        d_ = gl.d_;
        d1_ = gl.d1_;
        beta_ = gl.beta_;
        nrE_ = gl.nrE_;
        prec_ = gl.prec_;

        recalcDependentParams();

        return *this;
    }

    bool rejectRay(const PointNum& init, Num eps) const
    {
        for (UInt i = 0; i < numOfLenses_; ++i)
        {
            if (norm(init - coords_(i)) < eps)
            {
                return true;
            }
        }
        return false;
    }

    //PointNum ray(const PointNum& init);
    PointNum rayInteg(const PointNum& init);
    PointNum raySimple(const PointNum& init) const;

    void images(const PointNum& y, VecPointNum& images, const Num absPrec=1e-8, const Num relPrec=1e-8,
        const Num remPrec=1e-4) const
    {
        CNum yy(y.x(), y.y());
        CNum z[6];
        Int n;
        imagesComplex(yy, z, n, absPrec, relPrec, remPrec);
        images.resize(n);
        for (Int i = 0; i < n; ++i)
        {
            images[i] = PointNum(real(z[i]), imag(z[i]));
        }
    }

    Num jac(const PointNum& s) const
    {
        CNum xx(s.x(), s.y());
        return jacComplex(xx);
    }

    Num magSimple(const PointNum& b, const Num absPrec=1e-8, const Num relPrec=1e-8, const Num remPrec=1e-4) const;

    Num magExpr(const PointNum& b) const;

    void setOutputPrefix(const std::string& pref)
    {
        prefix_ = pref;
    }

    std::string getOutputPrefix() const
    {
        return prefix_;
    }

    struct PassedData
    {
        GravLens* lens;
        Num* IC;
    };

    std::string printInput() const;

    friend std::ostream& operator<<(std::ostream& os, const GravLens& gl);
    friend std::istream& operator>>(std::istream& is, GravLens& gl);

private:
    Num M_M_Sun_;   // total mass of the lens in units of M_SUN
    Num mu1_;       // relative mass of the first lens
    Num Dskpc_;     // distance between the source and the observer in kpc
    Num d_;         // angular distance between the lenses
    Num d1_;        // relative distance beween the observer and the second lens
    Num beta_;      // the parameter beta from Erdl & Schneider
//    Num w_;         // relative distance from the begining of the integration domain to the first lens and
                    // from the second lens to the end of the integration domain
    Num nrE_;
    Num prec_;      // local precision of the integration procedure

    Num d2_;        // relative distance beween the observer and the second lens

    Num M1_G_, M2_G_; // masses of lenses in geometrical units of length
    Num R1_G_, R2_G_; // Schwarzschild radii of the lenses

    Num m1_;          // the parameter m1 from Erdl & Schneider
    Num Ds_;          // distance between the source and the observer
    Num thetaE_;      // angular Einstein's radius
    Num rE_;          // Einstein's radius of the double lens
    Num rE1_, rE2_;    // Einstein's radii of single lenses
    Num dp1_, dp2_;   // relative distances from the observer to the planes
                      // between which we will integrate the path ofa photon

    UInt numOfLenses_;      // number of lenses
    VecNum relativeMasses_; // vector of scaled relative masses
    VecPointNum coords_;    // coordinates of lenses

    MatNum positionOfLenses_;

    std::string prefix_;  // prefix (comment symbols)

    bool initializedIC_;  // initial conditions set?
    bool computedFC_;     // final conditions computed?
    Num initCond_[Dim];   // coordinates at the begining of the integration domain
    Num finishCond_[Dim]; // coordinates at the end of the integration domain

    void recalcDependentParams();

    Num phi(const Num r[]) const;

    void setupInitCond(const PointNum& init);
    static Int odeSystem(Num t, const Num X[], Num F[], void* params);
    void integrate();
    PointNum propagateToSourcePlane();

    PointNum lensMap(const PointNum x) const
    {
        return raySimple(x);
    }
    CNum lensMapComplex(const CNum x) const
    {
        PointNum px(x.real(), x.imag());
        PointNum py = raySimple(px);
        return CNum(py.x(), py.y());
    }

    Num jacComplex(const CNum x) const;

    void xCoeff(const CNum y, CNum coeff[], Int& deg) const;
    void imagesComplex(const CNum y, CNum images[], Int& nRoots, const Num absPrec=1e-8,
        const Num relPrec=1e-8, const Num remPrec=1e-4) const;

};


#endif // GRAVLENS_HPP_INCLUDED
