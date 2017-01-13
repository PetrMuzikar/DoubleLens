#include <iostream>
#include <sstream>
#include "Qrng.hpp"

int main(int argc, char* argv[])
{
    using namespace ML;

    LLInt n = 1000000;
    LLInt n2 = 0;
    const Int dim = 4;
    QrngSobol q(dim);
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
        q.get(x);
    }

    std::cout << std::scientific << std::setprecision(12);
    for (LLInt i = 0; i < n2; ++i)
    {
        q.get(x);
        for (Int k = 0; k < dim; ++k)
        {
            std::cout << std::setw(20) << x[k];
        }
        std::cout << std::endl;
    }

    return 0;
}
