#ifndef POINT_HPP
#define POINT_HPP

#include <iomanip>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <string>
#include <sstream>
#include <cmath>

#include "ML.hpp"
#include "Pow.hpp"
#include "Util.hpp"

//#define POINT_SIMPLE_OUTPUT

namespace ML
{

template <typename T> class Point;

typedef Point<Int> PointInt;
typedef Point<Num> PointNum;

template <typename T>
std::ostream& operator<<(std::ostream& os, const Point<T>& p);

template <typename T>
std::istream& operator>>(std::istream& is, Point<T>& p);

template <typename T>
Point<T> operator+(Num number, const Point<T>& point);

template <typename T>
Point<T> operator-(Num number, const Point<T>& point);

template <typename T>
Point<T> operator*(Num number, const Point<T>& point);

template <typename T>
Point<T> operator/(Num number, const Point<T>& point);

template <typename T>
T SQR(const Point<T>& p);

template <typename T>
class Point
{
public:
    Point();
    Point(T x, T y);
    Point(CNum z);
    Point(const Point& other);
    Point& operator=(const Point& other);

    Point& operator+=(const Point& other);
    Point& operator-=(const Point& other);

    T x() const;
    void x(T val);
    T& x();

    T y() const;
    void y(T val);
    T& y();

    bool operator==(const Point& rhs) const;
    bool operator!=(const Point& rhs) const;
    bool operator>(const Point& rhs) const;
    bool operator>=(const Point& rhs) const;
    bool operator<(const Point& rhs) const;
    bool operator<=(const Point& rhs) const;

    // Friend operator functions should be declared before
    // member operator functions.
    friend Point operator+ <>(Num number, const Point& point);
    friend Point operator- <>(Num number, const Point& point);
    friend Point operator* <>(Num number, const Point& point);
    friend Point operator/ <>(Num number, const Point& point);

    Point operator+(const Point& next) const;
    Point operator-(const Point& next) const;
    Point operator*(const Point& next) const;
    Point operator/(const Point& next) const;

    Point operator+(Num next) const;
    Point operator-(Num next) const;
    Point operator*(Num next) const;
    Point operator/(Num next) const;

    Point operator-() const;
    operator PointNum() const;
    operator std::string() const;

    T dot(const Point& next) const;

    CNum toC() const
    {
        return CNum(x_, y_);
    }

    T area() const
    {
        using std::abs;
        return abs(x_ * y_);
    }

    T SQR(const Point<T>& p);

    friend std::ostream& operator<< <>(std::ostream& os, const Point& p);
    friend std::istream& operator>> <>(std::istream& is, Point& p);

private:
    T x_;
    T y_;
};

template <typename T>
Point<T>::Point() : x_(0), y_(0) {}

template <typename T>
Point<T>::Point(T x, T y) : x_(x), y_(y) {}

template <typename T>
Point<T>::Point(CNum z) : x_(z.real()), y_(z.imag()) {}

template <typename T>
Point<T>::Point(const Point<T>& other) : x_(other.x_), y_(other.y_) {}

template <typename T>
Point<T>& Point<T>::operator=(const Point<T>& rhs)
{
    if (this == &rhs)
        return *this;

    x_ = rhs.x_;
    y_ = rhs.y_;

    return *this;
}

template <typename T>
Point<T>& Point<T>::operator+=(const Point<T>& rhs)
{
    x_ += rhs.x_;
    y_ += rhs.y_;

    return *this;
}

template <typename T>
Point<T>& Point<T>::operator-=(const Point<T>& rhs)
{
    x_ -= rhs.x_;
    y_ -= rhs.y_;

    return *this;
}

template <typename T>
inline T Point<T>::x() const
{
    return x_;
}

template <typename T>
inline void Point<T>::x(T val)
{
    x_ = val;
}

template <typename T>
inline T& Point<T>::x()
{
    return x_;
}

template <typename T>
inline T Point<T>::y() const
{
    return y_;
}

template <typename T>
inline void Point<T>::y(T val)
{
    y_ = val;
}

template <typename T>
inline T& Point<T>::y()
{
    return y_;
}

template <typename T>
bool Point<T>::operator==(const Point<T>& rhs) const
{
    using std::abs;
    return abs(x_ - rhs.x_) < EPS && abs(y_ - rhs.y_) < EPS;
}

template <typename T>
inline bool Point<T>::operator!=(const Point<T>& rhs) const
{
    using std::abs;
    return abs(x_ - rhs.x_) > EPS && abs(y_ - rhs.y_) > EPS;
}

template <typename T>
inline bool Point<T>::operator>(const Point& rhs) const
{
    return x_ > rhs.x_ && y_ > rhs.y_;
}

template <typename T>
inline bool Point<T>::operator>=(const Point& rhs) const
{
    return x_ >= rhs.x_ && y_ >= rhs.y_;
}

template <typename T>
inline bool Point<T>::operator<(const Point& rhs) const
{
    return x_ < rhs.x_ && y_ < rhs.y_;
}

template <typename T>
inline bool Point<T>::operator<=(const Point& rhs) const
{
    return x_ <= rhs.x_ && y_ <= rhs.y_;
}

template <typename T>
inline Point<T> Point<T>::operator+(const Point& next) const
{
    return Point(x_ + next.x_, y_ + next.y_);
}

template <typename T>
inline Point<T> Point<T>::operator-(const Point& next) const
{
    return Point(x_ - next.x_, y_ - next.y_);
}

template <typename T>
inline Point<T> Point<T>::operator*(const Point& next) const
{
    return Point(x_ * next.x_, y_ * next.y_);
}

template <typename T>
inline Point<T> Point<T>::operator/(const Point& next) const
{
    return Point(x_ / next.x_, y_ / next.y_);
}

template <typename T>
inline Point<T> Point<T>::operator+(Num next) const
{
    return Point(x_ + next, y_ + next);
}

template <typename T>
Point<T> Point<T>::operator-(Num next) const
{
    return Point(x_ - next, y_ - next);
}

template <typename T>
Point<T> Point<T>::operator*(Num next) const
{
    return Point(x_ * next, y_ * next);
}

template <typename T>
Point<T> Point<T>::operator/(Num next) const
{
    return Point(x_ / next, y_ / next);
}

template <typename T>
inline Point<T> operator+(Num number, const Point<T>& point)
{
    return point + number;
}

template <typename T>
inline Point<T> operator-(Num number, const Point<T>& point)
{
    return -point + number;
}

template <typename T>
inline Point<T> operator*(Num number, const Point<T>& point)
{
    return point * number;
}

template <typename T>
inline Point<T> operator/(Num number, const Point<T>& point)
{
    return Point<T>(number / point.x(), number / point.y());
}

template <typename T>
inline Point<T> Point<T>::operator-() const
{
    return Point<T>(-x_, -y_);
}

template <typename T>
inline Point<T>::operator PointNum() const
{
    return PointNum(x_, y_);
}

template <typename T>
inline Point<T>::operator std::string() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

template <typename T>
inline T Point<T>::dot(const Point& next) const
{
    return x_ * next.x_ + y_ * next.y_;
}

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const Point<T>& p)
{
//    std::ios_base::fmtflags format = os.flags();
//    os.precision(6);
//    os.setf(std::ios_base::scientific);
//    os << '(' << std::setw(13) << p.x_ << ',' << std::setw(13) << p.y_ << ')';
//    os.unsetf(std::ios_base::scientific);

#ifdef POINT_SIMPLE_OUTPUT
    os << p.x_ << ' ' << p.y_;
#else
    os << '(' << printNumber(p.x_) << ',' << printNumber(p.y_) << ')';
#endif
    return os;
}

template <typename T>
std::istream& operator>>(std::istream& is, Point<T>& p)
{
    is >> p.x_ >> p.y_;
    return is;
}

template <typename T>
inline PointInt floor(const Point<T>& p)
{
    using std::floor;
    return PointInt(floor(p.x()), floor(p.y()));
}

template <typename T>
inline T abs(const Point<T>& p)
{
    return sqrt(SQR(p.x()) + SQR(p.y()));
}

template <typename T>
inline T norm(const Point<T>& p)
{
    return sqrt(SQR(p.x()) + SQR(p.y()));
    return sqrt(SQR(p.x()) + SQR(p.y()));
}

template <typename T>
inline T SQR(const Point<T>& p)
{
    return p.dot(p);
}

}

#endif // POINT_HPP
