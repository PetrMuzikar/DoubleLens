/// \file SourcePlaneGrid.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "ML/Point.hpp"
#include "Constants.hpp"
#include "SourcePlaneGrid.hpp"

SourcePlaneGrid::SourcePlaneGrid() : minGrid_(-1, -1), maxGrid_(1, 1), nGrid_(10, 10)
{
    init();
}

SourcePlaneGrid::SourcePlaneGrid(PointNum minGrid, PointNum maxGrid, PointInt nGrid)
    : minGrid_(minGrid), maxGrid_(maxGrid), nGrid_(nGrid)
{
    init();
}

SourcePlaneGrid::SourcePlaneGrid(Num xMinGrid, Num xMaxGrid, Int xNumGrid,
    Num yMinGrid, Num yMaxGrid, Int yNumGrid)
    : minGrid_(xMinGrid, yMinGrid), maxGrid_(xMaxGrid, yMaxGrid), nGrid_(xNumGrid, yNumGrid)
{
    init();
}

SourcePlaneGrid::SourcePlaneGrid(const SourcePlaneGrid& other)
    : minGrid_(other.minGrid_), maxGrid_(other.maxGrid_), nGrid_(other.nGrid_),
    diffGrid_(other.diffGrid_),
    insideCounter_(other.insideCounter_), outsideCounter_(other.outsideCounter_),
    errorCounter_(other.errorCounter_),
    betaX_(other.betaX_), betaY_(other.betaY_), pixels_(other.pixels_),
    magnification_(other.magnification_),
    magnificationLaplace_(other.magnificationLaplace_),
    magnificationSimple_(other.magnificationSimple_),
    magnificationExpr_(other.magnificationExpr_),
    prefix_(other.prefix_), printIndexes_(other.printIndexes_),
    ifComputeMagnification_(other.ifComputeMagnification_),
    shiftCorners_(other.shiftCorners_), printZeros_(other.printZeros_)
{
#ifdef RAYS_TO_PIXEL
    rtpc_ = other.rtpc_;
#endif
}

SourcePlaneGrid& SourcePlaneGrid::operator=(const SourcePlaneGrid& rhs)
{
    if (&rhs == this)
    {
        return *this;
    }

    minGrid_ = rhs.minGrid_;
    maxGrid_ = rhs.maxGrid_;
    nGrid_ = rhs.nGrid_;
    diffGrid_ = rhs.diffGrid_;
    insideCounter_ = rhs.insideCounter_;
    outsideCounter_ = rhs.outsideCounter_;
    errorCounter_ = rhs.errorCounter_;
    betaX_ = rhs.betaX_;
    betaY_ = rhs.betaY_;
    pixels_ = rhs.pixels_;
    magnification_ = rhs.magnification_;
    magnificationLaplace_ = rhs.magnificationLaplace_;
    magnificationSimple_ = rhs.magnificationSimple_;
    magnificationExpr_ = rhs.magnificationExpr_;
    prefix_ = rhs.prefix_;
    printIndexes_ = rhs.printIndexes_;
    ifComputeMagnification_ = rhs.ifComputeMagnification_;
    shiftCorners_ = rhs.shiftCorners_;
    printZeros_ = rhs.printZeros_;
#ifdef RAYS_TO_PIXEL
    rtpc_ = rhs.rtpc_;
#endif

    return *this;
}

void SourcePlaneGrid::init()
{
    diffGrid_ = (maxGrid_ - minGrid_) / nGrid_;

    insideCounter_ = 0;
    outsideCounter_ = 0;
    errorCounter_ = 0;

    Int m = nGrid_.x();
    Int n = nGrid_.y();

    betaX_.resize(m);
    betaY_.resize(n);
    pixels_.resize(m, n);
    magnification_.resize(m, n);
    magnificationLaplace_.resize(m, n);
    magnificationSimple_.resize(m, n);
    magnificationExpr_.resize(m, n);

    Num dx = diffGrid_.x();
    Num dy = diffGrid_.y();

    Num xMin = minGrid_.x() + dx / 2.0;
    Num yMin = minGrid_.y() + dy / 2.0;

    for (Int i = 0; i < m; ++i)
    {
        betaX_(i) = xMin + dx * i;
    }

    for (Int j = 0; j < n; ++j)
    {
        betaY_(j) = yMin + dy * j;
    }

    for (Int i = 0; i < m; ++i)
    {
        for (Int j = 0; j < n; ++j)
        {
            pixels_(i, j) = 0;
            magnification_(i, j) = 0;
            magnificationLaplace_(i, j) = 0;
            magnificationSimple_(i, j) = 0;
            magnificationExpr_(i, j) = 0;
        }
    }

    prefix_ = "# ";
    printIndexes_ = true;
    ifComputeMagnification_ = false;
    shiftCorners_ = false;
    printZeros_ = false;

#ifdef RAYS_TO_PIXEL
    for (UInt i = 0; i < rtpc_.size(); ++i)
    {
        PointInt pix = rtpc_[i].pixel_;
        rtpc_[i].coords_ = beta(pix.x(), pix.y());
        rtpc_[i].rays_.clear();
    }
#endif
}

bool SourcePlaneGrid::add(PointNum X)
{
    PointInt which = whichPixel(X);
    if ((which >= PointInt(0, 0)) && (which < nGrid_))
    {
        pixels_(which.x(), which.y())++;
        insideCounter_++;
        return true;
    }
    else
    {
        outsideCounter_++;
        return false;
    }
}

void SourcePlaneGrid::clear()
{
    insideCounter_ = 0;
    outsideCounter_ = 0;
    errorCounter_ = 0;

    for (Int i = 0; i < nGrid_.x(); ++i)
    {
        for (Int j = 0; j < nGrid_.y(); ++j)
        {
            pixels_(i, j) = 0;
            magnification_(i, j) = 0;
            magnificationLaplace_(i, j) = 0;
            magnificationSimple_(i, j) = 0;
            magnificationExpr_(i, j) = 0;
        }
    }

    ifComputeMagnification_ = false;

#ifdef RAYS_TO_PIXEL
    for (UInt i = 0; i < rtpc_.size(); ++i)
    {
        rtpc_[i].rays_.clear();
    }
#endif
}

void SourcePlaneGrid::reset(PointNum minGrid, PointNum maxGrid, PointInt nGrid)
{
    minGrid_ = minGrid;
    maxGrid_ = maxGrid;
    nGrid_ = nGrid;

    init();
}

void SourcePlaneGrid::add(const SourcePlaneGrid& spg)
{
    MatInt& mat = pixels_;
    const MatInt& other = spg.pixels();

    mat = mat + other;
}

//void SourcePlaneGrid::computeMagnification(const InputRays& ir)
//{
//
//}

void SourcePlaneGrid::computeLaplaceCorrection()
{
    Int nxgrid = nGrid_.x();
    Int nygrid = nGrid_.y();
    Num laplace;

    // inside
    for (Int ii = 1; ii < nxgrid - 1; ii++)
    {
        for (Int jj = 1; jj < nygrid - 1; jj++)
        {
            laplace = (magnification_(ii+1, jj) + magnification_(ii, jj+1) +
                       magnification_(ii-1,jj) + magnification_(ii, jj-1) - 4.0 * magnification_(ii, jj));
            magnificationLaplace_(ii, jj) = magnification_(ii, jj) - laplace / 24.0;
        }
    }

    // boundaries
    for (Int ii = 1; ii < nxgrid - 1; ii++)
    {
        laplace = (magnification_(ii-1, 0) + magnification_(ii+1, 0) -2.0 * magnification_(ii, 0)+
                   magnification_(ii, 0) + magnification_(ii, 2) - 2.0 * magnification_(ii, 1));
        magnificationLaplace_(ii, 0) = magnification_(ii, 0) - laplace / 24.0;

        laplace = (magnification_(ii-1, nygrid-1) + magnification_(ii+1, nygrid-1) -
                   2.0 * magnification_(ii, nygrid-1) +
                   magnification_(ii, nygrid-1) + magnification_(ii, nygrid-3) - 2.0 * magnification_(ii, nygrid-2));
        magnificationLaplace_(ii, nygrid-1) = magnification_(ii, nygrid-1)-laplace / 24.0;
    }
    for (Int jj = 1; jj < nygrid - 1; jj++)
    {
        laplace = (magnification_(0, jj-1) + magnification_(0, jj+1) - 2.0 * magnification_(0, jj)+
                   magnification_(0, jj) + magnification_(2, jj) - 2.0 * magnification_(1, jj));
        magnificationLaplace_(0, jj) = magnification_(0, jj) - laplace / 24.0;

        laplace = (magnification_(nxgrid-1, jj-1) + magnification_(nxgrid-1, jj+1) -
                   2.0 * magnification_(nxgrid-1, jj) +
                   magnification_(nxgrid-1, jj)+magnification_(nxgrid-3, jj) - 2.0 * magnification_(nxgrid-2, jj));
        magnificationLaplace_(nxgrid-1, jj) = magnification_(nxgrid-1, jj)-laplace / 24.0;
    }

    // corners
    laplace = (magnification_(0, 0) + magnification_(2, 0) - 2.0 * magnification_(1, 0) +
               magnification_(0, 0) + magnification_(0, 2) - 2.0 * magnification_(0, 1));
    magnificationLaplace_(0, 0) = magnification_(0, 0) - laplace / 24.0;

    laplace = (magnification_(nxgrid-3, 0) + magnification_(nxgrid-1, 0) -
               2.0 * magnification_(nxgrid-2, 0) +
               magnification_(nxgrid-1, 0) + magnification_(nxgrid-1, 2) - 2.0 * magnification_(nxgrid-1, 1));
    magnificationLaplace_(nxgrid-1, 0) = magnification_(nxgrid-1, 0) - laplace / 24.0;

    laplace = (magnification_(0, nygrid-3) + magnification_(0, nygrid-1) -
               2.0 * magnification_(0, nygrid-2) +
               magnification_(0, nygrid-1) + magnification_(2, nygrid-1) - 2.0 * magnification_(1, nygrid-1));
    magnificationLaplace_(0, nygrid-1) = magnification_(0, nygrid-1) - laplace / 24.0;

    laplace = (magnification_(nxgrid-3, nygrid-1)+magnification_(nxgrid-1, nygrid-1) -
               2.0 * magnification_(nxgrid-2, nygrid-1) +
               magnification_(nxgrid-1, nygrid-1) + magnification_(nxgrid-1, nygrid-3) -
               2.0 * magnification_(nxgrid-1, nygrid-2));
    magnificationLaplace_(nxgrid-1, nygrid-1) = magnification_(nxgrid-1, nygrid-1) -
            laplace / 24.0;
}

void SourcePlaneGrid::printHeader(std::ostream& os) const
{
    Int w = 30;

    os << prefix_ << "SourcePlaneGrid:\n";

    os << prefix_ << "    The incomming rays are detected in the rectangle ";
    os << PointNum(minGrid_.x(), maxGrid_.x()) << " x ";
    os << PointNum(minGrid_.y(), maxGrid_.y()) << ".\n";
    os << prefix_ << "    There are " << nGrid_.x() << " x " << nGrid_.y() << " rectangular pixels.\n";

    if (shiftCorners_)
    {
        os << prefix_ << "    Printing coords of lower left corners of pixels for gnuplot.\n";
        os << prefix_ << "        shift = " << diffGrid_ / 2.0 << "\n";
    }
    else
    {
        os << prefix_ << "    Printing coords of centers of pixels.\n";
    }

    if (!rtpc_.empty())
    {
        std::stringstream ss(prefix_);
        std::string p;
        ss >> p; // Eliminating whitespace at the end.
        os << p << "p    Watching rays coming into pixels:\n";
        os << p << "p" << std::setw(w-1) << "indexes";
        os << std::setw(w) << "coordinates" << std::endl;

        for (RaysToPixelCollection::const_iterator it = rtpc_.begin(); it != rtpc_.end(); it++)
        {
            os << p << "p" << std::setw(w-1) << std::string(it->pixel_);
            os << std::setw(w) << std::string(it->coords_) << std::endl;
        }
    }
}

void SourcePlaneGrid::printData(std::ostream& os) const
{
    Int m = nGrid_.x();
    Int n = nGrid_.y();

    Int width = 20;
    Int wWidth = 22;
    Int w = 10;
    Int w1 = 15;
    Int intWidth = 5;

    Int precOld = os.precision();
    Int prec = 11;
    Int precHigh = 13;

    Int raysGnuplot = 1000000;
    Num magGnuplot = 1e10;
    Num relGnuplot = 1e-10; 
    const Num xAdd = betaX_(n-1) + diffGrid_.x();
    const Num yAdd = betaY_(n-1) + diffGrid_.y();

    Num dx, dy;

    if (printIndexes())
    {
        os << prefix_ << std::setw(intWidth-prefix_.length()) << "i";
        os << std::setw(intWidth) << "j";
        os << std::setw(width) << "x";
        os << std::setw(width) << "y";
    }
    else
    {
        os << prefix_ << std::setw(width-prefix_.length()) << "x";
        os << std::setw(width) << "y";
    }

    os << std::setw(w) << "rays";


    if (ifComputeMagnification())
    {
        os << std::setw(w1) << "mag.";
        os << std::setw(w1) << "mag. Lap.";
        os << std::setw(wWidth) << "mag. simple";
        os << std::setw(wWidth) << "mag. expr.";
        os << std::setw(w1) << "rel.";
        os << std::setw(w1) << "rel. Lap.";
    }

    os << std::endl;

    if (shiftCorners_)
    {
        dx = diffGrid_.x() / 2;
        dy = diffGrid_.y() / 2;
    }
    else
    {
        dx = 0;
        dy = 0;
    }

    Num rel, relLap;
    for (Int i = 0; i < m; i++)
    {
        for (Int j = 0; j < n; j++)
        {
            if (printIndexes())
            {
                os << std::setw(intWidth) << i;
                os << std::setw(intWidth) << j;
            }
            os.precision(prec);
            os << std::scientific;
            os << std::setw(width) << (betaX_(i) - dx);
            os << std::setw(width) << (betaY_(j) - dy);
            os.precision(precOld);

            if (ifComputeMagnification())
            {
                rel = (magnification_(i, j) - magnificationSimple_(i, j)) / magnificationSimple_(i, j);
                relLap = (magnificationLaplace_(i, j) - magnificationSimple_(i, j)) / magnificationSimple_(i, j);
                os << std::setw(w) << pixels_(i, j);
                os << std::setw(w1) << magnification_(i, j);
                os << std::setw(w1) << magnificationLaplace_(i, j);
                os.precision(precHigh);
                os << std::setw(wWidth) << magnificationSimple_(i, j);
                os << std::setw(wWidth) << magnificationExpr_(i, j);
                os.precision(precOld);
                os << std::setw(w1) << rel;
                os << std::setw(w1) << relLap;
                os << std::endl;
            }
            else
            {
                os << std::setw(intWidth) << pixels_(i, j) << std::endl;
            }
        }

        if (printZeros())
        {
            if (printIndexes())
            {
                os << std::setw(intWidth) << i;
                os << std::setw(intWidth) << n;
            }
            os.precision(prec);
            os << std::scientific;
            os << std::setw(width) << (betaX_(i) - dx);
            os << std::setw(width) << yAdd - dy;
            os.precision(precOld);

            if (ifComputeMagnification())
            {
                os << std::setw(w) << raysGnuplot;
                os << std::setw(w1) << magGnuplot;
                os << std::setw(w1) << magGnuplot;
                os.precision(precHigh);
                os << std::setw(wWidth) << magGnuplot;
                os << std::setw(wWidth) << magGnuplot;
                os.precision(precOld);
                os << std::setw(w1) << relGnuplot;
                os << std::setw(w1) << relGnuplot;
                os << std::setw(w) << "#zero";
                os << std::endl;
            }
            else
            {
                os << std::setw(intWidth) << raysGnuplot;
                os << std::setw(w) << "#zero";
                os << std::endl;
            }
        }

        if (i < m-1)
        {
            os << std::endl;
        }
    }

    if (printZeros())
    {

        os << std::endl;

        for (Int j = 0; j <= n; ++j)
        {
            Num yy = ((j < n) ? betaY_(j) : yAdd);
            if (printIndexes())
            {
                os << std::setw(intWidth) << m;
                os << std::setw(intWidth) << j;
            }
            os.precision(prec);
            os << std::scientific;
            os << std::setw(width) << xAdd - dx;
            os << std::setw(width) << yy - dy;
            os.precision(precOld);

            if (ifComputeMagnification())
            {
                os << std::setw(w) << raysGnuplot;
                os << std::setw(w1) << magGnuplot;
                os << std::setw(w1) << magGnuplot;
                os.precision(precHigh);
                os << std::setw(wWidth) << magGnuplot;
                os << std::setw(wWidth) << magGnuplot;
                os.precision(precOld);
                os << std::setw(w1) << relGnuplot;
                os << std::setw(w1) << relGnuplot;
                os << std::setw(w) << "#zero";
                os << std::endl;
            }
            else
            {
                os << std::setw(intWidth) << raysGnuplot;
                os << std::setw(w) << "#zero";
                os << std::endl;
            }
        }
    }
}

void SourcePlaneGrid::printFooter(std::ostream& os) const
{
    LLInt total = inside() + outside() + error();

    os << prefix_ << "rays= " << total << std::endl;
    os << prefix_ << "inside= " << inside() << std::endl;
    os << prefix_ << "outside= " << outside() << std::endl;
    os << prefix_ << "error= " << error() << std::endl;

#ifdef RAYS_TO_PIXEL
    //if (ifComputeMagnification())
    //{
        std::string p = "#rtp";
        Int wi = 5;
        Int w = 24;
        os << std::scientific;
        os << std::endl;
        os.precision(12);
        for (UInt i = 0; i < rtpc_.size(); ++i)
        {
            PointInt pix = rtpc_[i].pixel_;
            PointNum coords = rtpc_[i].coords_;
            Int n = rtpc_[i].rays_.size();
            //os << p << prefix_ << "pixel " << pix << ", coords " << pixels_(pix.x(), pix.y()) << std::endl;
            for (Int j = 0; j < n; ++j)
            {
                PointNum r = rtpc_[i].rays_[j];
                os << p << std::setw(wi) << pix.x() << std::setw(wi) << pix.y();
                os << std::setw(w) << coords.x() << std::setw(w) << coords.y();
                os << std::setw(w) << r.x() << std::setw(w) << r.y() << std::endl;
            }
        }
    //}
#endif
}

std::ostream& operator<<(std::ostream& os, const SourcePlaneGrid& spg)
{
    spg.printHeader(os);
    spg.printData(os);
    spg.printFooter(os);

    return os;
}

std::istream& operator>>(std::istream& is, SourcePlaneGrid& spg)
{
    is >> spg.minGrid_.x() >> spg.maxGrid_.x() >> spg.nGrid_.x();
    is >> spg.minGrid_.y() >> spg.maxGrid_.y() >> spg.nGrid_.y();

#ifdef RAYS_TO_PIXEL
    Int i, j;
    while (is >> i >> j)
    {
        RaysToPixel rtp;
        rtp.pixel_ = PointInt(i, j);
        spg.rtpc_.push_back(rtp);
    }
#endif

    spg.init();

    return is;
}

std::string SourcePlaneGrid::printInput() const
{
    std::stringstream ss;

    ss.precision(10);
    ss << minGrid_.x() << " ";
    ss << maxGrid_.x() << " ";
    ss << nGrid_.x() << " ";
    ss << minGrid_.y() << " ";
    ss << maxGrid_.y() << " ";
    ss << nGrid_.y() << " ";

    for (RaysToPixelCollection::const_iterator it = rtpc_.begin(); it != rtpc_.end(); it++)
    {
        PointInt pix = it->pixel_;
        ss << pix.x() << " " << pix.y() << " ";
    }

    return ss.str();
}

void SourcePlaneGrid::readData(const std::string& fileName)
{
    using std::abs;
    std::ifstream inf(fileName.c_str());
    std::string line, rec;
    std::stringstream ss;
    std::string msg;
    Int i, j;
    Num x, y;
    Int rays;
    UInt np;

    if (!inf.is_open())
    {
        msg = this->prefix_ + "SourcePlaneGrid::readData: error while opening the file " + fileName + "!";
        throw std::runtime_error(msg.c_str());
    }

    try
    {
        while (getline(inf, line))
        {
            if ((line[0] == '#'))
            {
                np = line.find("# inside=");
                if (np != std::string::npos)
                {
                    ss.str(line);
                    ss >> rec >> rec >> rays;
                    insideCounter_ += rays;
                    ss.clear();
                    continue;
                }

                np = line.find("# outside=");
                if (np != std::string::npos)
                {
                    ss.str(line);
                    ss >> rec >> rec >> rays;
                    outsideCounter_ += rays;
                    ss.clear();
                    continue;
                }

                np = line.find("# error=");
                if (np != std::string::npos)
                {
                    ss.str(line);
                    ss >> rec >> rec >> rays;
                    errorCounter_ += rays;
                    ss.clear();
                    continue;
                }

                continue;
            }

            if (line == "")
            {
                continue;
            }

            ss.str(line);
            ss >> i >> j >> x >> y >> rays;

            pixels_(i, j) = pixels_(i, j) + rays;

//            if ((abs(betaX_(i) - x) < eps) && abs(betaY_(j) - y) < eps)
//            {
//
//            }
//            else
//            {
//
//            }
        }
    }
    catch (std::exception& ex)
    {
        msg = this->prefix_ + "SourcePlaneGrid::readData: error while processing the file " + fileName + "!";
        std::cerr << msg;
        throw;
    }
}
