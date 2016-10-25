#include <iostream>
#include "Rectangle.hpp"

int main()
{
    Domain d;
    Rectangle r(0, 1, 0, 1);

    std::cout << r.area() << std::endl;

    return 0;
}
