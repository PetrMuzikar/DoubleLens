#ifndef INVERSERAYSHOOTING_HPP_INCLUDED
#define INVERSERAYSHOOTING_HPP_INCLUDED

#include <iostream>
#include <iomanip>
#include <sstream>

//#define IRS_MESSAGES

//#define PRINT_THETAS

////#define SHIFT_CORNERS

#define DEBUG_INVERSE_RAY_SHOOTING

//#define USE_OPENMP_WHILE_SHOOTING

#ifdef USE_OPENMP_WHILE_SHOOTING
#include <omp.h>
#endif

#include "ML/ML.hpp"
#include "InputRays.hpp"
#include "IntegrationException.hpp"
#include "SourcePlaneGrid.hpp"

using namespace ML;

template <typename Lens>
class InverseRayShooting;

template <typename Lens>
std::ostream& operator<<(std::ostream& os, const InverseRayShooting<Lens>& irs);

template <typename Lens>
std::istream& operator<<(std::istream& is, InverseRayShooting<Lens>& irs);

template <typename Lens>
class InverseRayShooting
{
public:
    InverseRayShooting() : prefix_("# ") {}

    InverseRayShooting(const InputRays& ir, const Lens& lens, const SourcePlaneGrid& spg,
        const std::string pref = "# ")
        : ir_(ir), lens_(lens), spg_(spg), prefix_(pref)
    {
        ir_.setOutputPrefix(pref);
        lens_.setOutputPrefix(pref);
        spg_.setOutputPrefix(pref);
    }

    InverseRayShooting(const InverseRayShooting& irs)
        : ir_(irs.ir), lens_(irs.lens_), spg_(irs.spg_), prefix_(irs.prefix_)
    {

    }

    InverseRayShooting& operator=(const InverseRayShooting& irs)
    {
        if (this == &irs)
        {
            return *this;
        }

        ir_ = irs.ir_;
        lens_ = irs.lens_;
        spg_ = irs.spg_;
        prefix_ = irs.prefix_;

        return *this;
    }

    ~InverseRayShooting() {}

//    void shooting();
    void shootingInteg();
    void shootingSimple();
//    void shootingBoth();

    void computeMagnification();

    InputRays& getIR()
    {
        return ir_;
    }

    Lens& getLens()
    {
        return lens_;
    }

    SourcePlaneGrid& getSPG()
    {
        return spg_;
    }

    void setOutputPrefix(std::string pref)
    {
        prefix_ = pref;
        ir_.setOutputPrefix(pref);
        lens_.setOutputPrefix(pref);
        spg_.setOutputPrefix(pref);
    }

    std::string getOutputPrefix() const
    {
        return prefix_;
    }

    void printIndexes(bool dec) { spg_.printIndexes(dec); }
    bool printIndexes() const { return spg_.printIndexes(); }

#ifdef SOBOL_RANDOM_SEQUENCE
    Int getCore() const
    {
        return ir_.getCore();
    }

    void setCore(Int core)
    {
        ir_.setCore(core);
    }
#endif

    friend std::ostream& operator<<(std::ostream& os, const InverseRayShooting& irs)
    {
        os << irs.ir_ << std::endl;
        os << irs.lens_ << std::endl;
        os << irs.spg_;

        return os;
    }

    friend std::istream& operator>>(std::istream& is, InverseRayShooting& irs)
    {
        is >> irs.ir_;
        is >> irs.lens_;
        irs.spg_.printIndexes(true);
        is >> irs.spg_;

        return is;
    }

    std::string printInput() const
    {
        std::stringstream ss;

        ss << "## ";
        ss << ir_.printInput() << " ";
        ss << lens_.printInput() << " ";
        ss << spg_.printInput();

        return ss.str();
    }

    Num norm() const;

private:
    InputRays ir_;
    Lens lens_;
    SourcePlaneGrid spg_;

    std::string prefix_;
};

template <typename Lens>
Num InverseRayShooting<Lens>::norm() const
{
    LLInt total = ir_.getNumRays() - spg_.error();
    Num inputArea = ir_.area();
    Num sourceArea = (spg_.maxGrid() - spg_.minGrid()).area();
    LLInt pixels = spg_.nGrid().area();
    Num normF = total / inputArea * sourceArea / pixels;
//    std::cerr << total << std::endl;
//    std::cerr << inputArea << std::endl;
//    std::cerr << sourceArea << std::endl;
//    std::cerr << pixels << std::endl;
//    std::cerr << normF << std::endl;
    return(normF);
}

template <typename Lens>
void InverseRayShooting<Lens>::computeMagnification()
{
    LLInt total = ir_.getNumRays() - spg_.error();
    spg_.ifComputeMagnification(true);
    Num normF = this->norm();
    Num absPrec = 1e-8;
    Num relPrec = absPrec;
    Num remPrec = 1e-5;

    if (total == 0)
    {
        for (Int i = 0; i < spg_.nGrid().x(); ++i)
        {
            for (Int j = 0; j < spg_.nGrid().y(); ++j)
            {
                //spg_.magnificationSimple(i, j) = lens_.magSimple(spg_.beta(i, j));
                spg_.magnificationSimple(i, j) = lens_.magSimple(spg_.beta(i, j), absPrec, relPrec, remPrec);
                spg_.magnificationExpr(i, j) = lens_.magExpr(spg_.beta(i, j));
            }
        }
    }
    else
    {
        for (Int i = 0; i < spg_.nGrid().x(); ++i)
        {
            for (Int j = 0; j < spg_.nGrid().y(); ++j)
            {
                spg_.magnification(i, j) = spg_.pixels(i, j) / normF;
                spg_.magnificationSimple(i, j) = lens_.magSimple(spg_.beta(i, j));
                spg_.magnificationExpr(i, j) = lens_.magExpr(spg_.beta(i, j));
            }
        }
    }

    spg_.computeLaplaceCorrection();
}

template <typename Lens>
void InverseRayShooting<Lens>::shootingInteg()
{
//    std::string prefix = lens_.getOutputPrefix();

    LLInt n = ir_.getNumRays();
#ifdef IRS_MESSAGES
    LLInt step = 10;
    LLInt dn = (n > step ? n / step : n);
#endif
    PointNum init, finish;
    LLInt i;
//    #pragma omp for private(i, init, finish)
    for (i = 0; i < n; ++i)
    {
        while (lens_.rejectRay(init = ir_.getRay(i), 1e-8))
        {
            spg_.rayFailed();
#ifdef DEBUG_INVERSE_RAY_SHOOTING
            std::cerr << prefix_ << "InverseRayShooting: A bad ray too close to the lens: " << init << std::endl;
#endif
        }

        try
        {
            finish = lens_.rayInteg(init); // throws exceptions
            spg_.add(finish);
#ifdef RAYS_TO_PIXEL
        if (spg_.isInside(finish))
        {
            PointInt pix = spg_.whichPixel(finish);
            for (RaysToPixelCollection::iterator it = spg_.rtpc().begin(); it != spg_.rtpc().end(); it++)
            {
                if (it->pixel_ == pix)
                {
                    it->rays_.push_back(init);
                    break;
                }
            }
        }
#endif

#ifdef IRS_MESSAGES
            if (!(i % dn))
            {
                std::cerr << prefix_ << "Shooting: ";
                std::cerr << (i / dn) * 100 / step << "% done.\n";
                std::cerr << *this << std::endl << std::flush;
            }
#endif
        }
        catch (const IntegrationException& iex)
        {
            spg_.rayFailed();
            std::cerr << prefix_ << iex.what() << std::endl;
            continue;
        }
        catch (const std::exception& ex)
        {
            throw;
        }

//#ifdef PRINT_THETAS
//        std::cout << std::setw(width) << std::scientific << init.x();
//        std::cout << std::setw(width) << std::scientific << init.y();
//        std::cout << space;
//
//        std::cout << std::setw(width) << std::scientific << finish.x();
//        std::cout << std::setw(width) << std::scientific << finish.y();
//        std::cout << std::endl;
//#else
//        std::cout << std::setw(width) << std::scientific << finish.x();
//        std::cout << std::setw(width) << std::scientific << finish.y();
//        std::cout << std::endl;
//#endif
    }
}

template <typename Lens>
void InverseRayShooting<Lens>::shootingSimple()
{
//    std::string prefix = lens_.getOutputPrefix();

    LLInt n = ir_.getNumRays();
#ifdef IRS_MESSAGES
    LLInt step = 10;
    LLInt dn = (n > step ? n / step : n);
#endif
    PointNum init, finish;

#ifdef USE_OPENMP_WHILE_SHOOTING
#pragma omp parallel for
#endif
    for (LLInt i = 0; i < n; i++)
    {
        while (lens_.rejectRay(init = ir_.getRay(i), 1e-8))
        {
            spg_.rayFailed();
#ifdef DEBUG_INVERSE_RAY_SHOOTING
            std::cerr << prefix_ << "InverseRayShooting: A bad ray too close to the lens: " << init << std::endl;
#endif
        }

        finish = lens_.raySimple(init);
        spg_.add(finish);
#ifdef RAYS_TO_PIXEL
        if (spg_.isInside(finish))
        {
            PointInt pix = spg_.whichPixel(finish);
            for (RaysToPixelCollection::iterator it = spg_.rtpc().begin(); it != spg_.rtpc().end(); it++)
            {
                if (it->pixel_ == pix)
                {
                    it->rays_.push_back(init);
                    break;
                }
            }
        }
#endif

#ifdef IRS_MESSAGES
        if (!(i % dn))
        {
            std::cerr << prefix_ << "Shooting: ";
            std::cerr << (i / dn) * 100 / step << "% done.\n";
            std::cerr << *this << std::endl << std::flush;
        }
#endif

//#ifdef PRINT_THETAS
//        std::cout << std::setw(width) << std::scientific << init.x();
//        std::cout << std::setw(width) << std::scientific << init.y();
//        std::cout << space << std::flush;
//#endif
//        std::cout << std::setw(width) << std::scientific << finish.x();
//        std::cout << std::setw(width) << std::scientific << finish.y();
//        std::cout << std::endl;

    }
}

#endif // INVERSERAYSHOOTING_HPP_INCLUDED
