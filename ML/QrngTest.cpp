#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <vector>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_qrng.h>
#include "Qrng.hpp"

using namespace ML;

struct Pixel
{
    Pixel(Int ii=0, Int jj=0) : i(ii), j(jj), counter(0) {}
    Int i;
    Int j;
    Int counter;
};

class Pixels
{
public:
    Pixels(Int nDiv=100) : nDiv_(nDiv)
    {
        nPixels_ = nDiv_ * nDiv_;
        nTested_ = 0LL;
        nWatched_ = 0;
        d_ = 1.0 / nDiv_;
    }

    Int len() const { return nWatched_; }

    void addPixel(Int i, Int j)
    {
        p_.push_back(Pixel(i, j));
        nWatched_++;
    }

    void add(Num x[])
    {
        Int i = Int(floor(x[0] / d_));
        Int j = Int(floor(x[1] / d_));
        for (Int k = 0; k < nWatched_; ++k)
        {
            if ((p_[k].i == i) && (p_[k].j == j))
            {
                p_[k].counter++;
            }
        }
        nTested_++;
    }

    friend std::ostream& operator<<(std::ostream& os, const Pixels& pix);
    friend std::istream& operator>>(std::istream& is, Pixels& pix);

private:
    Int nDiv_;
    Int nPixels_;
    LLInt nTested_;
    Int nWatched_;
    Num d_;
    std::vector<Pixel> p_;
};

std::istream& operator>>(std::istream& is, Pixels& pix)
{
    Int nDiv, i, j;
    is >> nDiv;
    pix = Pixels(nDiv);
    while (is >> i >> j)
    {
        pix.addPixel(i, j);
    }

    return is;
}

std::ostream& operator<<(std::ostream& os, const Pixels& pix)
{
    Num norm = Num(pix.nTested_) / pix.nPixels_;
    Int w = 8;
    Int w1 = 12;
    Int w2 = 16;
    os << std::scientific;
    os.precision(6);
    os << "# nDiv = " << pix.nDiv_ << " => " << pix.nPixels_ << " pixels\n";
    os << "# nTested = " << pix.nTested_ << " => " << norm << " per pixel\n";
    os << "#" << std::setw(w-1) << "pixel";
    os << std::setw(w) << "pixel_i" << std::setw(w) << "pixel_j";
    os << std::setw(w1) << "counts";
    os << std::setw(w2) << "absDiff";
    os << std::setw(w2) << "relDiff";
    os << std::endl;
    for (Int k = 0; k < pix.nWatched_; ++k)
    {
        const Pixel& p = pix.p_[k];
        os << std::setw(w) << k;
        os << std::setw(w) << p.i;
        os << std::setw(w) << p.j;
        os << std::setw(w1) << p.counter;
        Num absDiff = p.counter - norm;
        Num relDiff = absDiff / norm;
        os << std::setw(w2) << absDiff;
        os << std::setw(w2) << relDiff;
        os << std::endl;
    }

    return os;
}

int main(int argc, char* argv[])
{
    /*
    using namespace ML;

    LLInt nStep = 1000000;
    LLInt nWrite = 0;
    LLInt nRepeat = 1;
    const Int dim = 2;
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
    LLInt counter = 0;
    for (LLInt r = 0; r < nRepeat; ++r)
    {
        std::cout << std::setprecision(12);
        for (LLInt s = 0; s < nStep; ++s)
        {
            q.get(x);
            counter++;
        }

        for (LLInt w = 0; w < nWrite; ++w)
        {
            q.get(x);
            std::cout << std::setw(12) << counter;
            for (Int d = 0; d < dim; ++d)
            {
                std::cout << std::setw(20) << x[d];
            }
            std::cout << std::endl;
            counter++;
        }
        t = clock();
        std::cout << std::setprecision(3);
        std::cout << "#clock: " << Num(t - t1) / CLOCKS_PER_SEC << " s\n\n\n";
        t1 = t;
    }

    std::cout << "#total_clock: " << Num(t - t0) / CLOCKS_PER_SEC << " s\n";

    return 0;
    */

    gsl_qrng* q = gsl_qrng_alloc(gsl_qrng_sobol, 2);
    gsl_rng_env_setup();
    gsl_rng* r = gsl_rng_alloc(gsl_rng_default);
    QrngSobol myQ(2);

    Int ch = 0;
    if (argc > 1)
    {
        std::stringstream ss(argv[1]);
        ss >> ch;
        if (ss.bad())
        {
            std::cerr << "Error while reading the choice.\n";
            ch = 0;
        }
    }

    std::string outFile("out.dat");
    std::ofstream ofs;
    if (argc > 2)
    {
        std::stringstream ss(argv[2]);
        ss >> outFile;
        if (ss.bad())
        {
            std::cerr << "Error while reading the name of output file.\n";
        }
    }
    ofs.open(outFile.c_str());
    if (!ofs.is_open())
    {
        std::cerr << "Error while opening the file " << outFile << ".\n";
        exit(1);
    }

    std::string inFile("in.dat");
    std::ifstream ifs;
    if (argc > 3)
    {
        std::stringstream ss(argv[3]);
        ss >> inFile;
        if (ss.bad())
        {
            std::cerr << "Error while reading the name of input file.\n";
        }
    }
    ifs.open(inFile.c_str());
    if (!ifs.is_open())
    {
        std::cerr << "Error while opening the file " << inFile << ".\n";
        exit(1);
    }

    switch (ch)
    {
    case 0:
        ofs << "# Generator: my sobol.\n";
        break;
    case 1:
        ofs << "# Generator: " << gsl_qrng_name(q) << std::endl;
        break;
    case 2:
        ofs << "# Generator: " << gsl_rng_name(r) << std::endl;
        break;
    default:
        std::cerr << "Bad choice, using ch = 0.\n";
        ch = 0;
        ofs << "# Generator: my sobol.\n";
    }

    LLInt nRays;
    Pixels pix;

    ifs >> nRays;
    ifs >> pix;

    ifs.close();

    Int l = floor(log(nRays) / log(2.0));
    LLInt nMax = LLInt(1) << l;
    LLInt when = LLInt(1) << (l - 10);

    const Int choice = ch;
    for (LLInt n = 1; n <= nMax; ++n)
    {
        Num x[2];

        switch (choice)
        {
        case 0:
            myQ.get(x);
            break;
        case 1:
            gsl_qrng_get(q, x);
            break;
        case 2:
            x[0] = gsl_rng_uniform(r);
            x[1] = gsl_rng_uniform(r);
            break;
        default:
            exit(1);
        }

        pix.add(x);

        if (n == when)
        {
            ofs << pix << std::endl;
            when <<= 1;
        }
    }

    ofs.close();
    gsl_qrng_free(q);
    gsl_rng_free(r);

    return 0;
}
