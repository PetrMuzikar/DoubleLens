#include <iostream>
#include <sstream>
#include "Qrng.hpp"

int main(int argc, char* argv[])
{
    using namespace ML;

    LLInt n = 1000000;
    LLInt n2 = 0;
    const Int dim = 4;
    QrngHalton q(dim);
    Num x[dim];
    VecNum xx(dim);

    if (argc > 1)
    {
        std::stringstream ss;
        ss << argv[1];
        ss >> n;
    }

    if (argc > 2)
    {
        std::stringstream ss;
        ss << argv[2];
        ss >> n2;
    }

    for (LLInt i = 0; i < n; ++i)
    {
        q.get(xx);
    }

    for (LLInt i = 0; i < n2; ++i)
    {
        q.get(xx);
        std::cout << xx << std::endl;
    }

    return 0;
}
