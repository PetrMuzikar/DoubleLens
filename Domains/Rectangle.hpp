#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "Domain.hpp"

class Rectangle : public Domain
{
public:
    Rectangle() : Domain(), xLow_(0.0), xHigh_(0.0), yLow_(0.0), yHigh_(0.0) {}
    Rectangle(Num xLow, Num xHigh, Num yLow, Num yHigh) : Domain(2, 0.0), xLow_(xLow), xHigh_(xHigh), yLow_(yLow), yHigh_(yHigh)
    {
        S_ = (xHigh_ - xLow_) * (yHigh_ - yLow_);
    }

private:
    Num xLow_;
    Num xHigh_;
    Num yLow_;
    Num yHigh_;
};

#endif
