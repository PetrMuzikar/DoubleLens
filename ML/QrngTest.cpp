#include <iostream>
#include <ctime>
#include <sstream>
#include "Qrng.hpp"

int main(int argc, char* argv[])
{
    using namespace ML;

    LLInt nStep = 1000000;
    LLInt nWrite = 0;
    LLInt nRepeat = 1;
    const Int dim = 4;
    QrngSobol q(dim);
    Num x[dim];

    if (argc > 1)
    {
        std::stringstream ss;
        ss << argv[1];
        ss >> nStep;
    }

    if (argc > 2)
    {
        std::stringstream ss;
        ss << argv[2];
        ss >> nWrite;
    }

    if (argc > 3)
    {
        std::stringstream ss;
        ss << argv[3];
        ss >> nRepeat;
    }

    std::cout << std::scientific;
    clock_t t0 = clock();
    clock_t t1 = t0;
    clock_t t = t0;
    for (LLInt r = 0; r < nRepeat; ++r)
    {
        std::cout << std::setprecision(12);
        for (LLInt s = 0; s < nStep; ++s)
        {
            q.get(x);
        }

        for (LLInt w = 0; w < nWrite; ++w)
        {
            q.get(x);
            for (Int d = 0; d < dim; ++d)
            {
                std::cout << std::setw(20) << x[d];
            }
            std::cout << std::endl;
        }
        t = clock();
        std::cout << std::setprecision(3);
        std::cout << "#clock: " << Num(t - t1) / CLOCKS_PER_SEC << " s\n\n\n";
        t1 = t;
    }

    std::cout << "#total_clock: " << Num(t - t0) / CLOCKS_PER_SEC << " s\n";

    return 0;
}
