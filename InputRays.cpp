/// \file InputRays.cpp

#include <iostream>
#include <cstdlib>
#include <cmath>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_qrng.h>

#include "ML/ML.hpp"
#include "ML/Point.hpp"
#include "InputRays.hpp"

//#define DEBUG_INPUT_RAYS

InputRays::InputRays(bool printRays) : prefix_("# "), printRays_(printRays)
{
#ifdef DEBUG_INPUT_RAYS
    std::cerr << "InputRays: warning - using default constructor which doesn\'t set numerical vaues.\n";
#endif

    initRandom();
}

InputRays::InputRays(DomainType type, Num xLow, Num xHigh, Num yLow, Num yHigh, LLInt n,
    const std::string& prefix, bool printRays) : type_(type), xLow_(xLow), xHigh_(xHigh),
    yLow_(yLow), yHigh_(yHigh), n_(n), prefix_(prefix), printRays_(printRays)
{
    initRandom();
}

InputRays::InputRays(const InputRays& ir) : type_(ir.type_), xLow_(ir.xLow_),
    xHigh_(ir.xHigh_), yLow_(ir.yLow_), yHigh_(ir.yHigh_), n_(ir.n_),
    prefix_(ir.prefix_), printRays_(ir.printRays_)
{
    copyRandom(ir);
}

InputRays& InputRays::operator=(const InputRays& rhs)
{
    if (&rhs == this)
    {
        return *this;
    }

    type_ = rhs.type_;
    xLow_ = rhs.xLow_;
    xHigh_ = rhs.xHigh_;
    yLow_ = rhs.yLow_;
    yHigh_ = rhs.yHigh_;
    n_ = rhs.n_;
    prefix_ = rhs.prefix_;
    printRays_ = rhs.printRays_;

    copyRandom(rhs);

    return *this;
}

#ifdef SOBOL_RANDOM_SEQUENCE
void InputRays::setCore(Int core)
{
    core_ = core;
    gsl_qrng_init(q_);
    double point[SOBOL_DIMS];
    Int advance = (2 * core + 1) / SOBOL_DIMS;
    for (LLInt k = 0; k < advance * n_; ++k)
    {
        gsl_qrng_get(q_, point);
    }
}
#endif

PointNum InputRays::getRay(LLInt k) const
{
#if defined(SOBOL_RANDOM_SEQUENCE)
    double point[SOBOL_DIMS];
    gsl_qrng_get(q_, point);
    Int ind = (2 * getCore()) % SOBOL_DIMS;
    Num r1 = point[ind];
    Num r2 = point[ind+1];
//    Num x = min_.x() + ((max_.x() - min_.x()) * Num(point[getCore()*2]));
//    Num y = min_.y() + ((max_.y() - min_.y()) * Num(point[getCore()*2+1]));
#else
    Num r1 = gsl_rng_uniform_pos(r_);
    Num r2 = gsl_rng_uniform_pos(r_);
//    Num x = min_.x() + (max_.x() - min_.x()) * gsl_rng_uniform_pos(r_);
//    Num y = min_.y() + (max_.y() - min_.y()) * gsl_rng_uniform_pos(r_);
#endif

//    LLInt i = k / nx_;
//    LLInt j = k % nx_;
//    Num x = min_.x() + dx_ / 2 + dx_ * i;
//    Num y = min_.y() + dy_ / 2 + dy_ * j;

    Num x, y, r, phi;
    switch (type_)
    {
        case RECTANGLE:
            x = xLow_ + (xHigh_ - xLow_) * r1;
            y = yLow_ + (yHigh_ - yLow_) * r2;
            return PointNum(x, y);
            break;
        case ANNULUS:
            r = rLow_ + (rHigh_ - rLow_) * r1;
            phi = 2.0 * PI * r2;
            return PointNum(xC_ + r*cos(phi), yC_ + r*sin(phi));
            break;
        default:
            throw std::runtime_error("InputRays::getRay: bad type of domain.");
    }
}

Num InputRays::area() const
{
    Num S;

    switch (type_)
    {
        case RECTANGLE:
            S = (xHigh_ - xLow_) * (yHigh_ - yLow_);
            return S;
        case ANNULUS:
            S = PI * (rHigh_ * rHigh_ - rLow_ * rLow_);
            return S;
        default:
            S = 0.0;
            return S;
    }
}

std::ostream& operator<<(std::ostream& os, const InputRays& ir)
{
    Int w1 = 12;
    Int w2 = 20;

    os << ir.prefix_ << "InputRays:\n";
    os << ir.prefix_ << "    " << ir.getNumRays() << " rays are generated in the ";

    switch (ir.type_)
    {
    case RECTANGLE:
        os << "rectangle: ";
        os << PointNum(ir.xLow_, ir.xHigh_) << " x ";
        os << PointNum(ir.yLow_, ir.yHigh_) << ".\n";
        break;
    case ANNULUS:
        os << "annulus with center " << PointNum(ir.xC_, ir.yC_) << " and radius in ";
        os << PointNum(ir.rLow_, ir.rHigh_) << ".\n";
        break;
    default:
        throw std::runtime_error("InputRays::operator<<: bad type of domain.");
    }

    os << ir.prefix_ << "    Algorithm: ";

#if defined(SOBOL_RANDOM_SEQUENCE)
    os << "a Sobol sequence using the GSL, core " << ir.core_ << ".";
#else
    os << "uniform random " << gsl_rng_name(ir.r_) << " from GSL";
    os << ", seed = " << ir.seed_ << '.';
#endif
//    os << "uniform rectangular grid.";
    if (ir.printRays())
    {
        os << std::endl << ir.prefix_ <<"    Rays:\n";
        os << ir.prefix_;
        os << std::setw(w1 - ir.prefix_.size()) << "i";
        os << std::setw(w2) << "x";
        os << std::setw(w2) << "y";
        os << std::endl;
        for (LLInt i = 0; i < ir.getNumRays(); ++i)
        {
            PointNum ray = ir.getRay(i);
            os << std::setw(w1) << i+1;
            os << std::setw(w2) << ray.x();
            os << std::setw(w2) << ray.y();
            os << std::endl;
        }
    }
    return os;
}

std::istream& operator>>(std::istream& is, InputRays& ir)
{
    char c = ' ';
    Num S;

    while(c == ' ')
    {
        is >> c;
    }

    switch (c)
    {
        case 'a':
            ir.type_ = ANNULUS;
            is >> S >> ir.xC_ >> ir.yC_ >> ir.rLow_ >> ir.rHigh_;
            break;
        case 'r':
            ir.type_ = RECTANGLE;
            is >> S >> ir.xLow_ >> ir.xHigh_ >> ir.yLow_ >> ir.yHigh_;
            break;
        default:
            throw std::runtime_error("InputRays::operator>>: bad type.");
    }

    is >> ir.n_;

#ifdef SOBOL_RANDOM_SEQUENCE
    Int core;
    is >> core;
    ir.setCore(core);
#endif

    return is;
}

std::string InputRays::printInput() const
{
    std::stringstream ss;

    switch (type_)
    {
    case RECTANGLE:
        ss << 'r' << " ";
        ss << area() << " ";
        ss << xLow_ << " ";
        ss << xHigh_ << " ";
        ss << yLow_ << " ";
        ss << yHigh_;
        break;
    case ANNULUS:
        ss << 'a' << " ";
        ss << area() << " ";
        ss << xC_ << " ";
        ss << yC_ << " ";
        ss << rLow_ << " ";
        ss << rHigh_;
        break;
    default:
        throw std::runtime_error("InputRays::printInput: bad type of domain.");
    }

    ss << " " << n_;

#ifdef SOBOL_RANDOM_SEQUENCE
    ss << " " << core_;
#endif

    return ss.str();
}
