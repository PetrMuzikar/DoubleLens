#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "Domain.hpp"

class Rectangle : public Domain
{
public:
    Rectangle(Num xLow, Num xHigh, Num yLow, Num yHigh);
private:
    Num xLow_;
    Num xHigh_;
    Num yLow_;
    Num yHigh_;
};

#endif
