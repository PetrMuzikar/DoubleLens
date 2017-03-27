#ifndef SOURCEPLANEGRID_HPP_INCLUDED
#define SOURCEPLANEGRID_HPP_INCLUDED

#include <iostream>
#include <iomanip>

#include <vector>

#include "ML/ML.hpp"
#include "ML/Point.hpp"
#include "ML/Vec.hpp"
#include "ML/Mat.hpp"
#include "InputRays.hpp"

#define RAYS_TO_PIXEL

using namespace ML;

#ifdef RAYS_TO_PIXEL
struct RaysToPixel
{
    PointInt pixel_;
    PointNum coords_;
    std::vector<PointNum> rays_;
};

typedef std::vector<RaysToPixel> RaysToPixelCollection;
#endif

class SourcePlaneGrid
{
public:
    SourcePlaneGrid();

    SourcePlaneGrid(PointNum minGrid, PointNum maxGrid, PointInt nGrid);

    SourcePlaneGrid(Num xMinGrid, Num xMaxGrid, Int xNumGrid,
                    Num yMinGrid, Num yMaxGrid, Int yNumGrid);

    SourcePlaneGrid(const SourcePlaneGrid& other);

    SourcePlaneGrid& operator=(const SourcePlaneGrid& rhs);

    PointInt whichPixel(PointNum X) const 
    {
        PointNum temp = (X - minGrid_) / diffGrid_;
        return floor(temp);
    }

    bool add(PointNum X);
    void add(const SourcePlaneGrid& spg);

    void rayFailed() { errorCounter_++; }

    void clear();

    void reset(PointNum minGrid, PointNum maxGrid, PointInt nGrid);

    void reset(Num xMinGrid, Num xMaxGrid, Int xNumGrid, Num yMinGrid, Num yMaxGrid, Int yNumGrid)
    {
        reset(PointNum(xMinGrid, yMinGrid), PointNum(xMaxGrid, yMaxGrid), PointInt(xNumGrid, yNumGrid));
    }

    LLInt count(LLInt i, LLInt j) const { return pixels_(i, j); }

    LLInt inside() const { return insideCounter_; }
    LLInt outside() const { return outsideCounter_; }
    LLInt error() const { return errorCounter_; }

    bool isEmpty() const
    {
        return (insideCounter_ == 0 && outsideCounter_ == 0);
    }

    bool isInside(const PointNum& beta)
    {
        return ((beta >= minGrid_) && (beta <= maxGrid_));
    }

    PointNum minGrid() const { return minGrid_; }
    PointNum maxGrid() const { return maxGrid_; }
    PointNum diffGrid() const { return diffGrid_; }
    PointInt nGrid() const { return nGrid_; }

    void setOutputPrefix(const std::string& pref) { prefix_ = pref; }
    std::string getOutputPrefix() const { return prefix_; }

    // Needs InputRays for normalization.
//    void computeMagnification(const InputRays& ir);

    bool ifComputeMagnification() const { return ifComputeMagnification_; }
    void ifComputeMagnification(bool doIt) { ifComputeMagnification_ = doIt; }

    void computeLaplaceCorrection();

    PointNum beta(Int i, Int j) const { return PointNum(betaX_(i), betaY_(j)); }

    Int pixels(Int i, Int j) const { return pixels_(i, j); }
    const MatInt& pixels() const { return pixels_; }

    Num magnification(Int i, Int j) const { return magnification_(i, j); }
    Num& magnification(Int i, Int j) { return magnification_(i, j); }

    Num magnificationLaplace(Int i, Int j) const { return magnificationLaplace_(i, j); }
    Num& magnificationLaplace(Int i, Int j) { return magnificationLaplace_(i, j); }

    Num magnificationSimple(Int i, Int j) const { return magnificationSimple_(i, j); }
    Num& magnificationSimple(Int i, Int j) { return magnificationSimple_(i, j); }

    Num magnificationExpr(Int i, Int j) const { return magnificationExpr_(i, j); }
    Num& magnificationExpr(Int i, Int j) { return magnificationExpr_(i, j); }

#ifdef RAYS_TO_PIXEL
    RaysToPixelCollection& rtpc() { return rtpc_; }
#endif

    void printIndexes(bool dec) { printIndexes_ = dec; }
    bool printIndexes() const { return printIndexes_; }

    void shiftCorners(bool dec) { shiftCorners_ = dec; }
    bool shiftCorners() const { return shiftCorners_; }

    void printZeros(bool dec) { printZeros_ = dec; }
    bool printZeros() const { return printZeros_; }

    void printHeader(std::ostream& os) const;
    void printData(std::ostream& os) const;
    void printFooter(std::ostream& os) const;

    std::string printInput() const;

    void readData(const std::string& fileName);

    friend std::istream& operator>>(std::istream& is, SourcePlaneGrid& spg);
    friend std::ostream& operator<<(std::ostream& os, const SourcePlaneGrid& spg);

private:
    PointNum minGrid_;
    PointNum maxGrid_;
    PointInt nGrid_;

    PointNum diffGrid_;

    LLInt insideCounter_;
    LLInt outsideCounter_;
    LLInt errorCounter_;

    VecNum betaX_;
    VecNum betaY_;

    MatInt pixels_;
    MatNum magnification_;
    MatNum magnificationLaplace_;
    MatNum magnificationSimple_;
    MatNum magnificationExpr_;

    std::string prefix_;
    bool printIndexes_;
    bool ifComputeMagnification_;
    bool shiftCorners_;
    bool printZeros_;

#ifdef RAYS_TO_PIXEL
    RaysToPixelCollection rtpc_;
#endif

    void init();
};

#endif // SOURCEPLANEGRID_HPP_INCLUDED
