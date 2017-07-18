#include <iostream>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <iomanip>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_odeiv2.h>

#include "ML/ML.hpp"
#include "ML/Pow.hpp"
#include "ML/Poly.hpp"
#include "ML/PolyCNumSolver.hpp"
#include "GravLens.hpp"
#include "IntegrationException.hpp"

//#define DEBUG_LENS_EQ
//#define DEBUG_INTEGRATE
//#define PRINT_ROOTS

#define X2M2

void GravLens::recalcDependentParams()
{
    using std::sqrt;
    using std::abs;

    Num z1, z2, z3, cm;

    d2_ = d1_ / (1.0 - (1.0 - d1_) * beta_);

    Num val = GEOM * M_M_Sun_ * M_SUN;

    M1_G_ = val * mu1_;
    M2_G_ = val * (1.0 - mu1_);

    R1_G_ = 2.0 * M1_G_;
    R2_G_ = 2.0 * M2_G_;

    beta_ = (d2_ - d1_) / ((1.0 - d1_) * d2_);
//    m1_ = M1_G_ / (M1_G_ + (1.0 - beta_) * M2_G_);
    m1_ = mu1_ / (mu1_ + (1.0 - beta_) * (1.0 - mu1_));
    Ds_ = Dskpc_ * KPC;
    thetaE_ = sqrt(4.0 * (M1_G_ * (1.0 - d1_) / d1_ + M2_G_ * (1.0 - d2_) / d2_) / Ds_);
    rE_ = d1_ * Ds_ * thetaE_;

    rE1_ = sqrt(4.0 * M1_G_ * (1.0 - d1_) * d1_ * Ds_);
    rE2_ = sqrt(4.0 * M2_G_ * (1.0 - d2_) * d2_ * Ds_);

    cm = d1_ * mu1_ + d2_ * (1.0 - mu1_);

    z1 = d1_ - nrE_ * rE1_ / Ds_;
    z2 = cm - nrE_ * rE_ / Ds_;
    z3 = d2_ - nrE_ * rE2_ / Ds_;
    z1 = (z1 <= z2 ? z1 : z2);
    dp1_ = (z1 <= z3 ? z1 : z3);

    z1 = d1_ + nrE_ * rE1_ / Ds_;
    z2 = cm + nrE_ * rE_ / Ds_;
    z3 = d2_ + nrE_ * rE2_ / Ds_;
    z1 = (z1 >= z2 ? z1 : z2);
    dp2_ = (z1 >= z3 ? z1 : z3);

    numOfLenses_ = 0;
    if (M_M_Sun_ > EPS)
    {
        numOfLenses_ = 1;
        if ((abs(d_) > EPS) || (abs(beta_) > EPS))
        {
            numOfLenses_ = 2;
        }
        else if (abs(mu1_ - 1.0) > EPS)
        {
//            mu1_ = 1.0;
            throw std::runtime_error("GravLens::recalcDependentParams - error: d == 0 & beta == 0 so mu1 shoud be 1!");
        }
    }

    switch (numOfLenses_)
    {
    case 0 :
        break;
    case 1 :
        relativeMasses_.resize(1);
        relativeMasses_[0] = 1.0;
        coords_.resize(1);
        coords_[0] = PointNum(0,0);
        positionOfLenses_.resize(1, 3);
        positionOfLenses_[0][0] = 0.0;
        positionOfLenses_[0][1] = 0.0;
        positionOfLenses_[0][2] = 0.0;
        break;
    case 2 :
        relativeMasses_.resize(2);
        relativeMasses_[0] = m1_;
        relativeMasses_[1] = 1.0 - m1_;
        coords_.resize(2);
        coords_[0] = PointNum(-d_/2.0, 0);
        coords_[1] = PointNum(d_/2.0, 0);
        positionOfLenses_.resize(2, 3);
        positionOfLenses_[0][0] = -d_ / 2.0 * rE_;
        positionOfLenses_[0][1] = 0.0;
        positionOfLenses_[0][2] = 0.0;
        positionOfLenses_[1][0] = d_ / 2.0 * rE_ / d1_ * d2_;
        positionOfLenses_[1][1] = 0.0;
        positionOfLenses_[1][2] = (d2_ - d1_) * Ds_;
        break;
    }

    prefix_ = "# ";

    initializedIC_ = false;
    computedFC_ = false;
}

Num GravLens::phi(const Num r[]) const
{
    using std::sqrt;

    Num pot = 0, absr;
    const Num MG[] = {M1_G_, M2_G_};

    for (UInt i = 0; i < numOfLenses_; ++i)
    {
        absr = sqrt(SQR(r[0] - positionOfLenses_[i][0]) + SQR(r[1] - positionOfLenses_[i][1]) + SQR(r[2] - positionOfLenses_[i][2]));
        pot -= MG[i] / absr;
    }

    return pot;
}

void GravLens::setupInitCond(const PointNum& init)
{
    initializedIC_ = true;
    computedFC_ = false;

    Num impactFactor = rE_ * dp1_ / d1_;

    initCond_[0] = init.x() * impactFactor;
    initCond_[1] = init.y() * impactFactor;
    initCond_[2] = -(d1_ - dp1_) * Ds_;

    Num v0[3] = {initCond_[0], initCond_[1], dp1_ * Ds_};
    Num absv0 = sqrt(SQR(v0[0]) + SQR(v0[1]) + SQR(v0[2]));

    initCond_[3] = v0[0] / absv0;
    initCond_[4] = v0[1] / absv0;
    initCond_[5] = v0[2] / absv0;
}

Int GravLens::odeSystem(Num t, const Num Xi[], Num Fi[], void* params)
{
    PassedData* data = (PassedData*)params;
    Num* IC = data->IC;
    Num x, y, z, vx, vy, vz, c1, c2, r, r3;
    Num phix, phiy, phiz;
    Num xL, yL, zL;
    const Num MG[] = {data->lens->M1_G_, data->lens->M2_G_};

    x = IC[0] + IC[3] * t + Xi[0];
    y = IC[1] + IC[4] * t + Xi[1];
    z = IC[2] + IC[5] * t + Xi[2];
    vx = IC[3] + Xi[3];
    vy = IC[4] + Xi[4];
    vz = IC[5] + Xi[5];

    phix = 0;
    phiy = 0;
    phiz = 0;
    for (UInt i = 0; i < data->lens->numOfLenses_; ++i)
    {
        xL = data->lens->positionOfLenses_[i][0];
        yL = data->lens->positionOfLenses_[i][1];
        zL = data->lens->positionOfLenses_[i][2];

        r = sqrt(SQR(x - xL) + SQR(y - yL) + SQR(z - zL));
        r3 = r * r * r;
        phix += MG[i] * (x - xL) / r3;
        phiy += MG[i] * (y - yL) / r3;
        phiz += MG[i] * (z - zL) / r3;
    }

    c1 = vx*vx + vy*vy + vz*vz + 1.0;
    c2 = 4.0 * (phix*vx + phiy*vy + phiz*vz);

    Fi[0] = Xi[3];
    Fi[1] = Xi[4];
    Fi[2] = Xi[5];

    Fi[3] = -c1 * phix + c2 * vx;
    Fi[4] = -c1 * phiy + c2 * vy;
    Fi[5] = -c1 * phiz + c2 * vz;

    return GSL_SUCCESS;
}

void GravLens::integrate()
{
    using std::abs;

    if (!initializedIC_)
    {
        throw std::runtime_error("GravLens::integrate: Initial conditions have not been set!");
    }

    Num delta[Dim];
    Num IC[Dim];
    for (Int i = 0; i < Dim; ++i)
    {
        delta[i] = 0.0;
        IC[i] = initCond_[i];
    }
    // Conversion from the local components of velocity to the global ones:
    Num factor = 2.0 * phi(IC) + 1.0; // 1 + 2 phi
    IC[3] *= factor;
    IC[4] *= factor;
    IC[5] *= factor;

    Num t = 0.0;
    Num stepMult = 1e-4;
    Num tMax = (dp2_ - dp1_) * Ds_;
    Num eps = prec_;
    Num h = stepMult * tMax;

    PassedData data;
    data.lens = this;
    data.IC = &IC[0];

    gsl_odeiv2_system sys = {GravLens::odeSystem, 0, Dim, &data};

    //const gsl_odeiv2_step_type* T = gsl_odeiv2_step_rkf45;
    const gsl_odeiv2_step_type* T = gsl_odeiv2_step_rk8pd;
    gsl_odeiv2_step* step = gsl_odeiv2_step_alloc(T, Dim);
    gsl_odeiv2_control* cont = gsl_odeiv2_control_y_new(0.0, eps);
    gsl_odeiv2_evolve* evol = gsl_odeiv2_evolve_alloc(Dim);

    Int nSteps = 0;
    Int maxSteps = 10000;
#ifdef DEBUG_INTEGRATE
    Num x, y, z, vx, vy, vz, xL, yL, zL, c1, c2, r, r3, phix, phiy, phiz;
    const Num MG[] = {M1_G_, M2_G_};
    Int wid = 18;
    std::cerr.precision(10);
    std::cerr << "# Debug info =============================================\n";
    std::cerr << "# tMax = " << tMax << std::endl;
    std::cerr << std::scientific;
    std::cerr << "# Step " << nSteps << ":\n";
    std::cerr << "#";
    std::cerr << std::setw(wid-1) << "tNow / tMax";
    std::cerr << std::setw(wid) << "tNow";
    std::cerr << std::setw(wid) << "x";
    std::cerr << std::setw(wid) << "y";
    std::cerr << std::setw(wid) << "z";
    std::cerr << std::setw(wid) << "vx";
    std::cerr << std::setw(wid) << "vy";
    std::cerr << std::setw(wid) << "vz";
    std::cerr << std::setw(wid) << "phix";
    std::cerr << std::setw(wid) << "phiy";
    std::cerr << std::setw(wid) << "phiz";
    std::cerr << std::setw(wid) << "c1";
    std::cerr << std::setw(wid) << "c2";
    std::cerr << std::endl;
    std::cerr << std::flush;
#endif
//    while (odesolver::odesolveriteration<Precision>(state))
    while (t < tMax)
    {
        nSteps++;
        if (nSteps >= maxSteps)
        {
            std::cerr << "# Integration failed:\n";
            std::cerr << "# tMax = " << tMax << std::endl;
            std::cerr << "# t before exit of integration = " << t << std::endl;
//            std::cerr << "# last step size h = " << state.h << std::endl;
            std::cerr << "# " << nSteps << " integration steps\n";
            std::cerr << std::flush;
            gsl_odeiv2_step_free(step);
            gsl_odeiv2_control_free(cont);
            gsl_odeiv2_evolve_free(evol);
#ifdef DEBUG_INTEGRATE
            throw IntegrationException("# GravLens::integrate: Maximum number of steps reached!\n\n");
#else
            throw IntegrationException("# GravLens::integrate: Maximum number of steps reached!\n");
#endif
        }

        Int state = gsl_odeiv2_evolve_apply(evol, cont, step, &sys, &t, tMax, &h, delta);
        if (state != GSL_SUCCESS)
        {
            gsl_odeiv2_step_free(step);
            gsl_odeiv2_control_free(cont);
            gsl_odeiv2_evolve_free(evol);
#ifdef DEBUG_INTEGRATE
            throw IntegrationException("# GSL: An error occured during the integration!\n\n");
#else
            throw IntegrationException("# GSL: An error occured during the integration!\n");
#endif
        }

#ifdef DEBUG_INTEGRATE

        x = IC[0] + IC[3] * t + delta[0];
        y = IC[1] + IC[4] * t + delta[1];
        z = IC[2] + IC[5] * t + delta[2];
        vx = IC[3] + delta[3];
        vy = IC[4] + delta[4];
        vz = IC[5] + delta[5];

        phix = 0;
        phiy = 0;
        phiz = 0;
        for (UInt i = 0; i < numOfLenses_; ++i)
        {
            xL = positionOfLenses_[i][0];
            yL = positionOfLenses_[i][1];
            zL = positionOfLenses_[i][2];

            r = sqrt(SQR(x - xL) + SQR(y - yL) + SQR(z - zL));
            r3 = r * r * r;
            phix += MG[i] * (x - xL) / r3;
            phiy += MG[i] * (y - yL) / r3;
            phiz += MG[i] * (z - zL) / r3;
        }

        c1 = vx*vx + vy*vy + vz*vz + 1.0;
        c2 = 4.0 * (phix*vx + phiy*vy + phiz*vz);

        //std::cerr << "# Step " << nSteps << ":\n";
        std::cerr << std::setw(wid) << t / tMax;
        std::cerr << std::setw(wid) << t;
        std::cerr << std::setw(wid) << x;
        std::cerr << std::setw(wid) << y;
        std::cerr << std::setw(wid) << z;
        std::cerr << std::setw(wid) << vx;
        std::cerr << std::setw(wid) << vy;
        std::cerr << std::setw(wid) << vz;
        std::cerr << std::setw(wid) << phix;
        std::cerr << std::setw(wid) << phiy;
        std::cerr << std::setw(wid) << phiz;
        std::cerr << std::setw(wid) << c1;
        std::cerr << std::setw(wid) << c2;
        std::cerr << std::endl;
        std::cerr << std::flush;
#endif
    }

#ifdef DEBUG_INTEGRATE
    std::cerr << "# t after integration = " << t << std::endl;
    std::cerr << "# " << nSteps << " integration steps.\n\n\n";
//    std::cerr << "# " << nSteps << " integration steps.\n";
//    throw IntegrationException("# The integration was successful.\n\n");
#endif

    finishCond_[0] = IC[0] + IC[3] * t + delta[0];
    finishCond_[1] = IC[1] + IC[4] * t + delta[1];
    finishCond_[2] = IC[2] + IC[5] * t + delta[2];
    finishCond_[3] = IC[3] + delta[3];
    finishCond_[4] = IC[4] + delta[4];
    finishCond_[5] = IC[5] + delta[5];

    Num factorF = -2.0 * phi(finishCond_) + 1.0;
    finishCond_[3] *= factorF;
    finishCond_[4] *= factorF;
    finishCond_[5] *= factorF;
    computedFC_ = true;

    gsl_odeiv2_step_free(step);
    gsl_odeiv2_control_free(cont);
    gsl_odeiv2_evolve_free(evol);
}

PointNum GravLens::propagateToSourcePlane()
{
    if (!computedFC_)
    {
        throw std::runtime_error("GravLens::propagateToSourcePlane: Initial conditions have not been set!");
    }
    Num t = (1.0 - d1_) * Ds_ - finishCond_[2];
    Num x = (finishCond_[0] + finishCond_[3] * t) * d1_ / rE_;
    Num y = (finishCond_[1] + finishCond_[4] * t) * d1_ / rE_;

    return PointNum(x, y);
}

PointNum GravLens::rayInteg(const PointNum& init)
{
    if (numOfLenses_ == 0)
    {
        return init;
    }

    PointNum res;
    try
    {
        setupInitCond(init);
        integrate();
        res = propagateToSourcePlane();
    }
    catch (const IntegrationException& iex)
    {
//        std::cerr << prefix_ << "Integration of ray " << init;
//        std::cerr << " was not successful.\n";
        std::cerr << prefix_ << "This was a ray ";
        Int prec = std::cerr.precision();
        std::cerr << std::setprecision(15) << init << ".\n";
        std::cerr.precision(prec);
        throw;
    }
    catch (const std::exception& ex)
    {
        throw;
    }

    return res;
}

PointNum GravLens::raySimple(const PointNum& xi) const
{
//    if (rejectRay(xi, 1e-8))
//    {
//        throw std::domain_error("Too close to a lens!");
//    }

    PointNum res = xi;
    PointNum sh(d_/2.0, 0.0);
    // In the reference frame where the first lens is in the origin:
    PointNum xi1 = xi + sh;
    PointNum xi2 = xi1 - m1_ * beta_ * xi1 / xi1.dot(xi1);
    PointNum xiVec[] = {xi1, xi2};

#ifdef X2M2
    Num x2m2 = d_;
#else
    Num x2m2 = d_ - m1_ * beta_ / d_;
#endif

    PointNum lensVec[] = {PointNum(0.0, 0.0), PointNum(x2m2, 0.0)};

    PointNum temp;
    for (UInt i = 0; i < numOfLenses_; ++i)
    {
        temp = xiVec[i] - lensVec[i];
        if (abs(temp) < EPS)
        {
            throw std::runtime_error("raySimple: division by 0!");
        }
        res -= relativeMasses_[i] * temp / temp.dot(temp);
    }

    return res;
}

void GravLens::xCoeff(const CNum yy, CNum coeff[], Int& deg) const
{
    // Coefficients of the lens equation are computed in the reference frame where
    // the first lens is located in the origin.
    using std::abs;

	Num m1, d, beta, beta2;
	CNum y, cy, y2, cy2, sh;
    Num m13, m12, d2;
	PointNum differ;
	Int k = 0;

    m1 = m1_;
    d = d_;
    beta = beta_;
    beta2 = beta * beta;

    sh = d / 2.0;
    y = yy + sh;
	cy = conj(y);
	y2 = y * y;
	cy2 = cy * cy;
	m12 = m1 * m1;
	m13 = m12 * m1;
	d2 = d * d;

#ifndef X2M2
    Num beta3 = beta2 * beta;
    Num beta4 = beta3 * beta;
    Num m14 = m13 * m1;
    Num m15 = m14 * m1;
    Num d3 = d2 * d;
#endif

    switch (numOfLenses_)
    {
    case 0 :
        deg = 1;
        coeff[0] = -y;
        coeff[1] = 1.0;
        return;
    case 1 :
        // d == 0 and beta == 0
        deg = 2;
        coeff[0] = -1.0;
        coeff[1] = -y;
        coeff[2] = 1.0;
        return;
    case 2 :
        // d != 0 or beta != 0
        if (abs(beta) <= EPS) // d != 0
        {
            deg = 5;
            k = 0;
        }
        else if (abs(d) <= EPS) // beta != 0
        {
            deg = 4;
            k = 0;
        }
        else // d != 0 and beta != 0
        {
            deg = 6;
            k = 1;
#ifdef X2M2
            coeff[0] = d*((-1. + beta)*beta*d*m13*y + (1. - 1.*beta)*beta2*m13*y2);
#else
            coeff[0] = d2*((1. - 1.*beta)*beta3*m15*y + d*((1. - 1.*beta)*beta3*m14*y2 + d*(beta2*(-2. + 2.*beta)*m14*y + d*((1. - 1.*beta)*beta*d*m13*y + (-1. + beta)*beta2*m13*y2))));
#endif
            //coeff[0] = beta*(-d2*m13*y + beta*(-beta*d*m13*y2 + d*(d*m13*y + m13*y2)));
        }
        break;
    }

#ifdef X2M2
    coeff[k] = m12*(-1.*beta2*y2 + beta2*m1*y2) + d*(m12*((-3. + beta)*beta2*m1*y + y*(beta*(1. + beta) + beta2*(2.*cy - 1.*beta*cy)*y)) + d*((-1. + 2.*beta)*d*m12*y + m12*(-1.*beta + beta*m1 + y*(beta*(-2.*cy + beta*cy) + (1. - 2.*beta)*beta*y))));

    coeff[k+1] = m1*(-1.*beta2*cy*y2 + beta2*cy*m1*y2) + d*(m1*(m1*y*(beta*(2. + beta*(-6. + 2.*beta))*cy + beta2*y) + y*(2.*beta2*cy + (-1.*beta + beta2*cy2)*y)) + d*(m1*(-1.*beta*cy + m1*((2. - 1.*beta)*beta*cy + beta*(-4. + 2.*beta)*y) + y*(2. - 1.*beta*cy2 + (2. - 2.*beta)*beta*cy*y)) + d*(d*m1*y + m1*(-1. + m1 + y*((-2. + 2.*beta)*cy - 1.*beta*y)))));

    coeff[k+2] = cy*(beta*y + m1*((-1. + beta)*beta*y - 1.*beta2*m1*y)) + d*(-1.*y + (2. - 1.*beta)*beta*m12*y + cy*(-1.*beta - 1.*beta*y2 + cy*(-1.*beta*y + (2. - 2.*beta)*beta*m1*y) + m1*((2. - 2.*beta)*beta + beta*(-2. + (4. - 1.*beta)*beta)*m1 + (-1. + beta)*beta*y2)) + d*(1. + m1*(-1. + beta - 1.*beta*m1 + beta*y2) + d*(-1.*y + cy*d*y + (-1. + beta)*m1*y + cy*(-1. + (2. - 2.*beta)*m1 - 1.*cy*y - 1.*beta*y2)) + cy*((2. + 2.*beta)*y + (2. + beta*(-8. + 4.*beta))*m1*y + cy*(beta*m1 + beta*y2))));

    coeff[k+3] = cy2*(beta*y - 1.*beta*m1*y) + d*(cy*(-2.*y + (4. - 2.*beta)*beta*m1*y + cy*(beta + (-2. + beta)*beta*m1 - 1.*beta*y2)) + d*(y - 1.*beta*m1*y + cy*(-2.*beta + (-2. + (6. - 2.*beta)*beta)*m1 + (2. - 2.*beta)*cy*y + beta*y2) + d*(1. - 1.*cy*d - 1.*m1 + cy*(cy + (-2. + 2.*beta)*y))));

    coeff[k+4] = d2*(-1. + m1) + cy*(d*(1. + beta + (-3. + beta)*beta*m1 + d*((2. - 1.*beta)*d + (1. - 2.*beta)*y)) + cy*(-1.*beta + beta*m1 + d*((-2. + beta)*d + (-1. + 2.*beta)*y)));

    coeff[k+5] = cy*((1. - 1.*beta)*cy*d + (-1. + beta)*d2);

#else
    coeff[k] = d*(beta3*(-1. + 2.*beta)*m15*y + d*(m14*(beta3 - 1.*beta3*m1 + y*(beta3*(2.*cy - 1.*beta*cy) + beta3*(-1. + 2.*beta)*y)) + d*(m13*(beta2*(3. + (-9. + beta)*beta)*m1*y + y*(beta2*(1. + beta) + beta3*(2.*cy - 1.*beta*cy)*y)) + d*(m12*(beta2*y2 + m1*(-2.*beta2 + 2.*beta2*m1 + y*(beta2*(-4.*cy + 2.*beta*cy) + (1. - 4.*beta)*beta2*y))) + d*(m12*(beta*(-3. + (9. - 1.*beta)*beta)*m1*y + y*((-1. - 1.*beta)*beta + beta2*(-2.*cy + beta*cy)*y)) + d*((1. - 2.*beta)*d*m12*y + m12*(beta - 1.*beta*m1 + y*(beta*(2.*cy - 1.*beta*cy) + beta*(-1. + 2.*beta)*y))))))));

    coeff[k+1] = -1.*beta4*m15*y + d*(m14*(-1.*beta3 + beta3*m1 + y*(beta3*(-2. + 2.*beta)*cy - 1.*beta4*y)) + d*(m13*(beta3*cy + m1*((-2. + beta)*beta3*cy + (8. - 2.*beta)*beta3*y) + y*(-2.*beta2 + beta3*cy2 + beta3*(-2. + 2.*beta)*cy*y)) + d*(m12*(y*(2.*beta3*cy + (-1.*beta2 + beta3*cy2)*y) + m1*(3.*beta2 - 3.*beta2*m1 + y*(beta2*(8. + beta*(-12. + 2.*beta))*cy + 4.*beta3*y))) + d*(m1*(beta2*cy*y2 + m1*(-2.*beta2*cy + m1*((4. - 2.*beta)*beta2*cy + beta2*(-14. + 4.*beta)*y) + y*(4.*beta - 2.*beta2*cy2 + (3. - 4.*beta)*beta2*cy*y))) + d*(m1*(y*(-2.*beta2*cy + (beta - 1.*beta2*cy2)*y) + m1*(-3.*beta + 3.*beta*m1 + y*(beta*(-8. + (12. - 2.*beta)*beta)*cy - 4.*beta2*y))) + d*(m1*(beta*cy + m1*((-2. + beta)*beta*cy + (8. - 2.*beta)*beta*y) + y*(-2. + beta*cy2 + beta*(-2. + 2.*beta)*cy*y)) + d*(-1.*d*m1*y + m1*(1. - 1.*m1 + y*((2. - 2.*beta)*cy + beta*y)))))))));

    coeff[k+2] = -1.*beta4*cy*m14*y + d*(m13*(-1.*beta3*y + (-1. + beta)*beta3*m1*y) + cy*(-1.*beta3*cy*m13*y + m13*(-1.*beta3 + (2. - 2.*beta)*beta3*m1 - 1.*beta4*y2)) + d*(cy*(m12*((-2. - 2.*beta)*beta2*y + beta2*(-2. + (12. - 4.*beta)*beta)*m1*y) + cy*m12*(-1.*beta3*m1 - 1.*beta3*y2)) + m12*(-1.*beta2 + m1*((1. - 1.*beta)*beta2 + beta3*m1 - 1.*beta3*y2)) + d*(m1*(-1.*beta*y + m1*(3.*beta2*y + (5. - 4.*beta)*beta2*m1*y)) + cy*(cy*m1*(-1.*beta2*y + (5. - 2.*beta)*beta2*m1*y) + m1*(-1.*beta2 - 1.*beta2*y2 + m1*((5. - 2.*beta)*beta2 + beta2*(-8. + (10. - 1.*beta)*beta)*m1 + beta2*(-1. + 4.*beta)*y2))) + d*(m1*(2.*beta + m1*(beta*(-2. + 2.*beta) - 2.*beta2*m1 + 2.*beta2*y2)) + cy*(-1.*beta*y + m1*(beta*(5. + 3.*beta)*y + beta*(4. + beta*(-21. + 8.*beta))*m1*y) + cy*m1*(2.*beta2*m1 + 2.*beta2*y2)) + d*(y + m1*(-3.*beta*y + beta*(-5. + 4.*beta)*m1*y) + cy*(beta + beta*y2 + cy*(beta*y + beta*(-5. + 2.*beta)*m1*y) + m1*(beta*(-5. + 2.*beta) + beta*(8. + (-10. + beta)*beta)*m1 + (1. - 4.*beta)*beta*y2)) + d*(-1. + m1*(1. - 1.*beta + beta*m1 - 1.*beta*y2) + cy*((-2. - 2.*beta)*y + (-2. + (12. - 4.*beta)*beta)*m1*y + cy*(-1.*beta*m1 - 1.*beta*y2)) + d*(y - 1.*cy*d*y + (1. - 1.*beta)*m1*y + cy*(1. + (-2. + 2.*beta)*m1 + cy*y + beta*y2))))))));

    coeff[k+3] = beta4*cy*m14 + d*(m13*(beta3 - 1.*beta3*m1) + cy*(beta3*cy*m13 + beta3*(-2. + 2.*beta)*m13*y) + d*(m12*(-1.*beta2*y + beta3*m1*y) + cy*(beta2*(-2. + 2.*beta)*cy*m12*y + m12*(2.*beta3 + beta2*(2. + beta*(-10. + 2.*beta))*m1 - 1.*beta3*y2)) + d*(m12*(-3.*beta2 + 3.*beta2*m1) + cy*(m1*(-2.*beta*y + (10. - 8.*beta)*beta2*m1*y) + cy*m1*(beta2 + (-5. + beta)*beta2*m1 - 1.*beta2*y2)) + d*(m1*(2.*beta*y - 2.*beta2*m1*y) + cy*(cy*(-1.*beta*y + (5. - 4.*beta)*beta*m1*y) + m1*(-4.*beta2 + beta*(-4. + (18. - 4.*beta)*beta)*m1 + 2.*beta2*y2)) + d*(m1*(3.*beta - 3.*beta*m1) + cy*(2.*y + beta*(-10. + 8.*beta)*m1*y + cy*(-1.*beta + (5. - 1.*beta)*beta*m1 + beta*y2)) + d*(-1.*y + beta*m1*y + cy*(2.*beta + (2. + beta*(-10. + 2.*beta))*m1 + (-2. + 2.*beta)*cy*y - 1.*beta*y2) + d*(-1. + cy*d + m1 + cy*(-1.*cy + (2. - 2.*beta)*y))))))));

    coeff[k+4] = d2*(m12*(beta2 - 1.*beta2*m1) + d2*(d2*(1. - 1.*m1) + m1*(-2.*beta + 2.*beta*m1))) + cy*(cy*d2*((2. - 1.*beta)*beta2*m12 + d*(beta*(-1. + 2.*beta)*m1*y + d*(beta + beta*(-5. + 2.*beta)*m1 + d*((2. - 1.*beta)*d + (1. - 2.*beta)*y)))) + d*((2. - 1.*beta)*beta3*m13 + d*(beta2*(-1. + 2.*beta)*m12*y + d*(m1*(beta*(1. + beta) + beta2*(-9. + 4.*beta)*m1) + d*((2. - 4.*beta)*beta*m1*y + d*(-1. - 1.*beta + (9. - 4.*beta)*beta*m1 + d*((-2. + beta)*d + (-1. + 2.*beta)*y)))))));

    coeff[k+5] = cy*(cy*d3*((-1. + beta)*d2 + beta*(m1 - 1.*beta*m1)) + d2*(beta2*(m12 - 1.*beta*m12) + d2*((1. - 1.*beta)*d2 + beta*(-2.*m1 + 2.*beta*m1))));
#endif

//    coeff[k] = -d3*m12*y + beta*(d*(m12*y + d*(2.*d*m12*y + m12*(-1. + m1 +
//    y*(-2.*cy + y)))) + beta*(beta*d*m12*(m1*y - cy*y2) + m12*(-y2 + m1*y2) +
//    d*(d*m12*(cy - 2.*y)*y + m12*(-3.*m1*y + y*(1. + 2.*cy*y)))));
//
//    coeff[k+1] = -2.*cy*d3*m1*y + d2*(2.*m1*y + d*((-1. + m1)*m1 + d*m1*y)) +
//    beta*(d*(-m1*y2 + d*(-4.*m12*y - d*m1*y2)) + beta*(2.*beta*cy*d*m12*y +
//    d*(2.*d*m12*y + m12*y2) + cy*(cy*d*m1*y2 + m1*(-y2 + m1*y2) + d*(m1*(2.*y
//    - 6.*m1*y) + d*m1*(-m1 - 2.*y2)))) + cy*(-cy*d2*m1*y + d*(2.*m12*y +
//    d*(2.*d*m1*y + m1*(-1. + 2.*m1 + 2.*y2)))));
//
//    coeff[k+2] = cy*(-cy*d3*y + d2*(2.*y + 2.*m1*y + d*(-1. + 2.*m1 + d*y))) + d*(-y
//    + d*(1. - m1 + d*(-y - m1*y))) + beta*(d*(2.*m12*y + d*(d*m1*y + m1*(1. -
//    m1 + y2))) + cy*(y - m1*y + cy*d*(-y + 2.*m1*y + d*(m1 + y2)) + d*(-1. -
//    y2 + m1*(2. - 2.*m1 - y2) + d*(2.*y - 8.*m1*y + d*(-2.*m1 - y2)))) +
//    beta*(-beta*cy*d*m12 - d*m12*y + cy*(-2.*cy*d*m1*y + m1*(y - m1*y) +
//    d*(4.*d*m1*y + m1*(-2. + 4.*m1 + y2)))));
//
//    coeff[k+3] = d2*(d*(1. - m1) + y) + cy*(d*(d*(-2.*m1 + d*(-d - 2.*y)) - 2.*y) +
//    cy*d2*(d + 2.*y)) + beta*(-d2*m1*y + beta*cy*(cy*d*m1 + d*(-2.*d*m1 -
//    2.*m1*y)) + cy*(cy*(y - m1*y + d*(1. - 2.*m1 - 2.*d*y - y2)) + d*(4.*m1*y
//    + d*(-2. + 6.*m1 + 2.*d*y + y2))));
//
//    coeff[k+4] = d2*(-1. + m1) + cy*(cy*d*(-2.*d - y) + d*(1. + d*(2.*d + y))) +
//    beta*(beta*cy*d*m1 + cy*(d*(1. - 3.*m1 + d*(-d - 2.*y)) + cy*(-1. + m1 +
//    d*(d + 2.*y))));
//
//    coeff[k+5] = cy*((1. - beta)*cy*d + (-1. + beta)*d2);

#ifdef DEBUG_LENS_EQ
    std::cout << "Coefficients of the lens equation:\n";
    for (Int k = 0; k <= deg; ++k)
    {
        std::cout << std::setw(35) << coeff[k] << std::endl;
    }

#endif
}

void GravLens::imagesComplex(const CNum y, CNum images[], Int& nRoots) const
{
    // The origin of the reference frame is the geometric centre between the lenses.

    using std::abs;

    Num s;

//    if (numOfLenses_ == 0)
//    {
//
//    }
    switch (numOfLenses_)
    {
    case 0 :
        images[0] = y;
        nRoots = 1;
        return;
    case 1 :
        s = sqrt(0.25 + 1.0 / SQR(abs(y)));
        images[0] = (0.5 + s) * y;
        images[1] = (0.5 - s) * y;
        nRoots = 2;
        return;
    }

    // numOfLenses_ == 2

    CNum shift = d_ / 2.0;
//    CNum yy = y - shift;
	CNum coeff[7];
	Int deg;
	xCoeff(y, coeff, deg);
//	deg = 6; //!!!

	Int j = 0;
	const Int np1 = deg+1;
    CNum rem = ZERO;
    CNum x;

//	Num prec = 1e-9;
//	Num precRem = 1e-5;

//	Num prec = 1e-10;
//	Num precRem = 1e-5;
//    Num prec = 1e-8;
//    Num precRem = 1e-4;

    PolyCNum poly(np1, coeff);
    PolyCNumSolver pcs(poly);
    VecCNum roots(deg);

	try
	{
        pcs.roots(absPrec_, relPrec_, PolyCNumSolver::DURAND_KERNER, roots);
        //pcs.roots(prec, prec, PolyCNumSolver::DURAND_KERNER, roots);
//        pcs.roots(prec, prec, PolyCNumSolver::COMBINED, roots);
        //pcs.roots(absPrec, relPrec, PolyCNumSolver::LEHMER_SCHUR, roots);
    }
    catch (const std::exception& ex)
    {
        std::cerr << prefix_ << ex.what() << std::endl;
        std::cerr << prefix_ << "y = " << y << std::endl;
        std::cerr.precision(12);
        std::cerr << "poly = " << std::scientific << poly << std::endl;
        std::cerr << roots << std::endl;
    }

//    std::cout << std::setw(30) << "x";
//    std::cout << std::setw(30) << "y\n";
	for (Int i = 0; i < deg; ++i)
	{
        roots[i] -= shift; // back to the "central" reference frame
		x = roots[i];
#ifdef PRINT_ROOTS
		std::cout << std::scientific << std::setprecision(5);
		std::cout << std::setw(30) << x;
		std::cout << std::setw(30) << lensMapComplex(x);
#endif
//        if ((abs(coeff[0]) < EPS) && (abs(x) < EPS))
//        {
//            continue;
//        }
        try
        {
            rem = y - lensMapComplex(x);
        }
        catch (std::runtime_error& e)
        {
            std::cerr << prefix_ << e.what() << std::endl;
            continue;
        }
#ifdef PRINT_ROOTS
		std::cout << std::setw(15) << abs(rem) << std::endl;
#endif
		if (abs(rem) < remPrec_)
		{
	  		images[j] = x;
	  		j++;
		}
	}
	nRoots = j;
}

Num GravLens::jacComplex(const CNum x) const
{
    using std::abs;

	CNum cx, num, p, q;
    CNum shift = d_ / 2.0;
    CNum xx = x + shift;
    Num res;

//    switch (this->numOfLenses_)
//    {
//    case 2:
//        m1 = this->relativeMasses_(1);
//        d = abs(this->coords_(1) - this->coords_(0));
//        break;
//    case 1:
//        m1 = 1;
//        d = 0;
//        break;
//    case 0:
//        m1 = 0;
//        d = 0;
//        break;
//    default:
//        throw std::runtime_error("jac: Cannot deal with more than 2 lenses!");
//    }
//
//    // Only in the case beta == 0!!!
//    beta = 0;

	cx = conj(xx);
	if (abs(xx) < EPS)
	{
		std::cerr << "GravLens::jac: division by zero (x = 0)!" << std::endl;
		return 0.0;
	}

//	num = SQR(-beta_ * m1_ / xx - d_ + cx);
//    num = (1.0 - m1_) / SQR(x*(cx - d_) - m1_ * beta_);

#ifdef X2M2
    // x^{(2)}_{m_2} = d
    num = (1.0 - m1_) / SQR(xx * (cx - d_) - m1_ * beta_);
#else
    // x^{(2)}_{m_2} = d - m1*beta/d
    num = (1.0 - m1_) * SQR(d_ / (d_ * (d_ * xx + m1_ * beta_) - xx * (d_ * cx + m1_ * beta_)));
#endif

    p = m1_ * beta_ * num + 1.0;
    q = m1_ / SQR(cx) + SQR(xx) * num;

	if (abs(num) < EPS)
	{
		std::cerr << "GravLens::jac: division by zero!" << std::endl;
	}

//	p = beta_ * (1.0 - m1_) * m1_ / num / SQR(xx) + 1.0;
//	q = (1.0 - m1_) / num + m1_ / SQR(cx);
//    p = m1_ * beta_ * num + 1.0;
//    q = m1_ / SQR(cx) + SQR(x) * num;

//	return (SQR(abs(p))  - SQR(abs(q)));
    res = real(p * conj(p) - q * conj(q));

//    std::cout << "x = " << x << std::endl;
//    std::cout << "xx = " << xx << std::endl;
//    std::cout << "cx = " << cx << std::endl;
//    std::cout << "num = " << num << std::endl;
//    std::cout << "p = " << p << std::endl;
//    std::cout << "q = " << q << std::endl;
//    std::cout << "res = " << res << std::endl;

    return res;
}

Num GravLens::magExpr(const PointNum& b) const
{
    Num u2 = b.dot(b);
    Num A = (u2 + 2.0) / (sqrt(u2 * (u2 + 4.0)));
    return A;
}

Num GravLens::magSimple(const PointNum& b) const
{
    using std::abs;

    switch (numOfLenses_)
    {
    case 0 :
        return 1.0;
    case 1 :
    {
        Num u2 = b.dot(b);
        Num A = (u2 + 2.0) / (sqrt(u2 * (u2 + 4.0)));
        return A;
    }
    case 2 :
        break;
    default :
        throw std::runtime_error("GravLens::magSimple: Cannot deal with more than 2 lenses!");
    }

    CNum y(b.x(), b.y());
	CNum roots[6], jacobian;
//	int *pocet = new int;
    Int n;
	Num A = 0.0;

    imagesComplex(y, roots, n);

	for (Int i = 0; i < n; ++i)
	{
		jacobian = jacComplex(roots[i]);
		if (abs(jacobian) < EPS)
		{
	  		std::cerr << "GravLens::magSimple: jac = 0!" << std::endl;
	  		return 0.0;
		}
		A += 1.0 / abs(jacobian);
	}

	return A;
}

std::ostream& operator<<(std::ostream& os, const GravLens& l)
{
    Int wid1 = 25;
    Int wid2 = 25;
    Int pr1 = os.precision();
    Int pr2 = 12;

    os << l.prefix_ << "DoubleLens:\n";
    os << l.prefix_ << "    M = " << l.M_M_Sun_ << " M_Sun = " << l.M1_G_ + l.M2_G_ << " m\n";
    os << l.prefix_ << "    mu1 = M1 / M = " << l.mu1_ << std::endl;
    os << l.prefix_ << "    Ds = " << l.Dskpc_ << " kpc = " << l.Ds_ << " m\n";
    os << l.prefix_ << "    d1 = " << l.d1_ << " Ds" << std::endl;
    os << l.prefix_ << "    d2 = " << l.d2_ << " Ds" << std::endl;
    os << l.prefix_ << "    (d2 - d1) * Ds / rE = " << (l.d2_ - l.d1_) * l.Ds_ / l.rE_ << std::endl;
    os.precision(pr2);
    os << l.prefix_ << "    dp1 = " << l.dp1_ << " Ds" << std::endl;
    os << l.prefix_ << "    dp2 = " << l.dp2_ << " Ds" << std::endl;
    os.precision(pr1);
    os << l.prefix_ << "    w1 = " << l.d1_ - l.dp1_ << " Ds = " << (l.d1_ - l.dp1_) * l.Ds_ / l.rE1_ << " rE1" << std::endl;
    os << l.prefix_ << "    w2 = " << l.dp2_ - l.d2_ << " Ds = " << (l.dp2_ - l.d2_) * l.Ds_ / l.rE2_ << " rE2" << std::endl;
    os << l.prefix_ << "    nrE = " << l.nrE_ << std::endl;
//    os << l.prefix_ << "    w = " << l.w_ << " Ds = " << l.w_ * l.Ds_ / l.rE_ << " rE = "<< l.w_ * l.Ds_ << " m\n";
    os << l.prefix_ << "    rE1 = " << l.rE1_ << " m\n";
    os << l.prefix_ << "    rE2 = " << l.rE2_ << " m\n";
    os << l.prefix_ << "    rE = " << l.rE_ << " m\n";
    os << l.prefix_ << "    thetaE = " << l.thetaE_ << std::endl;
    os << l.prefix_ << "    m1 = " << l.m1_ << std::endl;
    os << l.prefix_ << "    d = " << l.d_ << std::endl;
    os << l.prefix_ << "    beta = " << l.beta_ << std::endl;
#ifdef X2M2
    os << l.prefix_ << "    x^{(2)}_{m_2} = d\n";
#else
    os << l.prefix_ << "    x^{(2)}_{m_2} = d - m_1 * beta / d = " << l.d_ - l.m1_ * l.beta_ / l.d_ << std::endl;
#endif
    os << l.prefix_ << "    integration: prec = " << l.prec_ << std::endl;
    os << l.prefix_ << "    polynomial solver:\n";
    os << l.prefix_ << "      absPrec = " << l.absPrec_ << std::endl;
    os << l.prefix_ << "      relPrec = " << l.relPrec_ << std::endl;
    os << l.prefix_ << "      remPrec = " << l.remPrec_ << std::endl;
    os << l.prefix_ << "    number of lenses = " << l.numOfLenses_ << std::endl;
    os << l.prefix_ << "    lenses:\n";
    os << l.prefix_ << "    " << std::setw(wid2) << "coords";
    os << std::setw(wid1) << "scaled rel. masses" << std::endl;
    for (UInt i = 0; i < l.numOfLenses_; ++i)
    {
        os << l.prefix_ << "    ";
        os << std::setw(wid2) << std::string(l.coords_[i]);
        os << std::setw(wid1) << l.relativeMasses_[i];
        if (i < l.numOfLenses_ - 1)
        {
            os << std::endl;
        }
    }

    os << std::flush;

    return os;
}

std::istream& operator>>(std::istream& is, GravLens& gl)
{
    is >> gl.M_M_Sun_ >> gl.mu1_ >> gl.Dskpc_ >> gl.d_;
    is >> gl.d1_ >> gl.beta_ >> gl.nrE_ >> gl.prec_;
    is >> gl.absPrec_ >> gl.relPrec_ >> gl.remPrec_;

    gl.recalcDependentParams();

    return is;
}

std::string GravLens::printInput() const
{
    std::stringstream ss;

    ss << M_M_Sun_ << " ";
    ss << mu1_ << " ";
    ss << Dskpc_ << " ";
    ss << d_ << " ";
    ss << d1_ << " ";
    ss << beta_ << " ";
    ss << nrE_ << " ";
    ss << prec_ << " ";
    ss << absPrec_ << " ";
    ss << relPrec_ << " ";
    ss << remPrec_ << " ";

    return ss.str();
}
