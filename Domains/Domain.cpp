#include <iostream>
#include <exception>
#include <stdexcept>

#include "Domain.hpp"

//Num Domain::area() const
//{
//    Num s;
//    switch (type)
//    {
//        case Domain::Type::RECTANGLE:
//            s = (xHigh - xLow) * (yHigh - yLow);
//            return s;
//        case Domain::Type::ANNULUS:
//            s = PI * (rHigh * rHigh - rLow * rLow);
//            return s;
//        default:
//            s = 0;
//            return s;
//    }
//}
//
//CNum Domain::randomPoint(const Num r1, const Num r2) const
//{
//    Num r, phi, x, y;
//
//    switch (type)
//    {
//        case Domain::Type::ANNULUS:
//            r = rLow + (rHigh - rLow) * r1;
//            phi = 2.0 * PI * r2;
//            return CNum(r*cos(phi), r*sin(phi));
//        case Domain::Type::RECTANGLE:
//            x = xLow + (xHigh - xLow) * r1;
//            y = yLow + (yHigh - yLow) * r2;
//            return CNum(x, y);
//        default:
//            throw std::runtime_error("Domain::randomPoint: bad type.");
//    }
//}
//
//std::ostream& operator<<(std::ostream& os, const Domain& dom)
//{
////    os.precision(10);
//    os.setf(std::ios::scientific);
//    switch(dom.type)
//    {
//        case Domain::Type::RECTANGLE:
//            os << 'r';
//            os << "  " << dom.area();
//            os << "  " << dom.xLow;
//            os << "  " << dom.xHigh;
//            os << "  " << dom.yLow;
//            os << "  " << dom.yHigh;
//            break;
//        case Domain::Type::ANNULUS:
//            os << 'a';
//            os << "  " << dom.area();
//            os << "  " << dom.xC;
//            os << "  " << dom.yC;
//            os << "  " << dom.rLow;
//            os << "  " << dom.rHigh;
//            break;
//        default:
//            throw std::runtime_error("Domain::operator<<: bad type.");
//    }
//
//    return os;
//}
//
//std::istream& operator>>(std::istream& is, Domain& dom)
//{
//    char c;
//
//    is >> c;
//
//    switch (c)
//    {
//        case 'a':
//            dom.type = Domain::Type::ANNULUS;
//            is >> dom.S >> dom.xC >> dom.yC >> dom.rLow >> dom.rHigh;
//            break;
//        case 'r':
//            dom.type = Domain::Type::RECTANGLE;
//            is >> dom.S >> dom.xLow >> dom.xHigh >> dom.yLow >> dom.yHigh;
//            break;
//        default:
//            throw std::runtime_error("Domain::operator>>: bad type.");
//    }
//
//    return is;
//}
