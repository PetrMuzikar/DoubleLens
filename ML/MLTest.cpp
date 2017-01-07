#include <iostream>
#include <sstream>
#include "Qrng.hpp"

int main(int argc, char* argv[])
{
    using namespace ML;

    LLInt n = 1000000;
    const Int dim = 2;
    QrngHalton q(dim);
    Num x[dim];
    VecNum xx(dim);

    if (argc > 1)
    {
        std::stringstream ss;
        ss << argv[1];
        ss >> n;
    }

    for (Int i = 0; i < n; ++i)
    {
        //q.get(x);
        q.get(xx);
    }

    return 0;
}
