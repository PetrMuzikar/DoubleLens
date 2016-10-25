#include <iostream>
#include <fstream>
#include <streambuf>
#include <iomanip>
#include <stdexcept>
#include <cstdlib>
#include <ctime>

#include "ML/Point.hpp"
#include "ML/Vec.hpp"
#include "Constants.hpp"
#include "InputRays.hpp"
#include "GravLens.hpp"
#include "InverseRayShooting.hpp"
#include "Test.hpp"

//#define DEBUG_MAIN
//#define INTEGRATE_PHOTON_PATH

int main(int argc, char* argv[])
{
    clock_t before = clock();
    std::string prefix = "# ";

#ifdef DEBUG_MAIN
//    testInputRays();
    //debug(prefix);
    //debugRays(prefix);
    debugRaysToPixel(prefix);
//    testML();

    return 0;
#endif

    /*
        i ... images of source plane
        s ... shooting
        c ... combine results
        r ... individual rays
        m ... magnification using analytic formula
    */
    char choice = 's';

//    std::string inp("in.dat");
    std::string out("out.dat");
    std::string err("err.dat");

//    std::streambuf* cinbuf = std::cin.rdbuf();
    std::streambuf* coutbuf = std::cout.rdbuf();
    std::streambuf* cerrbuf = std::cerr.rdbuf();

//    std::ifstream inf;
    std::ofstream outf;
    std::ofstream errf;

    try
    {
        errf.open(err.c_str());
        std::cerr.rdbuf(errf.rdbuf());

        // format: ./ShootingSobol choice out in1 in2 ...
        if (argc >= 4)
        {
            choice = *argv[1];
            out = argv[2];
//        inp = argv[3];

            outf.open(out.c_str());
            std::cout.rdbuf(outf.rdbuf());

            switch (choice)
            {
            case 'i':
                imageOfSourcePlane(argc, argv, prefix);
                break;
            case 's':
                shooting(argc, argv, prefix);
                break;
            case 'c':
                combineData(argc, argv, prefix);
                break;
            case 'r':
                rays(argc, argv, prefix);
                break;
            case 'm':
                magPoint(argc, argv, prefix);
                break;
            default:
                throw std::runtime_error("main: bad choice!");
                break;
            }
        }
        else
        {
            throw std::runtime_error("main: bad arguments!");
        }

        clock_t after = clock();
        clock_t diff = after - before;

        std::cout << prefix << "Time: " << double(diff) / CLOCKS_PER_SEC << " s." << std::endl;

//        inf.close();
        outf.close();
        errf.close();

//        std::cin.rdbuf(cinbuf);
        std::cout.rdbuf(coutbuf);
        std::cerr.rdbuf(cerrbuf);

    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;

//        inf.close();
        outf.close();
        errf.close();

//        std::cin.rdbuf(cinbuf);
        std::cout.rdbuf(coutbuf);
        std::cerr.rdbuf(cerrbuf);

        return 1;
    }

    return 0;
}
