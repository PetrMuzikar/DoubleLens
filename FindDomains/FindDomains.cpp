#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <list>
#include <string>
#include <complex>
#include <cmath>
#include <exception>
#include <stdexcept>

#include "Domain.hpp"

using std::abs;
using std::sqrt;

Int findNeighbors(const CNum elem, const Num r, CNumList& from, CNumList& res)
{
    Int k = 0;

//    Int w = 20;
//    std::cout << std::setw(w) << elem << std::endl;

    for (CNumList::iterator it = from.begin(); it != from.end(); it++)
    {
//        std::cout << std::setw(w) << *it;
//        std::cout << std::setw(w) << abs(*it - elem);
//        std::cout << std::endl;
        if (abs(*it - elem) < r)
        {
            k++;
            res.push_back(*it);
            it = from.erase(it);
        }
    }

    return k;
}

Int groupOfNeighbors(const CNum elem, const Num r, CNumList& from,
    CNumList& res)
{
    Int k = 1;

    res.clear();
    res.push_back(elem);

    for (CNumList::iterator itRes = res.begin(); itRes != res.end(); itRes++)
    {
        if (!from.empty())
        {
            k += findNeighbors(*itRes, r, from, res);
        }
        else
        {
            break;
        }
    }

    return k;
}

Int groups(const Num r, CNumList& from, CNumLists& gr)
{
    Int k = 0;
    CNumList res;
    CNum elem;

    gr.clear();

    while (!from.empty())
    {
        k++;
        elem = from.front();
        from.pop_front();
        groupOfNeighbors(elem, r, from, res);
        gr.push_back(res);
    }

    return k;
}

void domain(const CNumList& data, Domain& dom, const Num pad = 0.05)
{
    if (data.empty())
    {
        throw std::runtime_error("domain: empty data.");
    }

    Num x, y;
    Num xC, yC, dx2, dy2;
    Num xLow = data.front().real();
    Num xHigh = xLow;
    Num yLow = data.front().imag();
    Num yHigh = yLow;

    for (CNumList::const_iterator it = data.begin(); it != data.end(); it++)
    {
        x = it->real();
        y = it->imag();

        if (x < xLow)
        {
            xLow = x;
        }
        else if (x > xHigh)
        {
            xHigh = x;
        }

        if (y < yLow)
        {
            yLow = y;
        }
        else if (y > yHigh)
        {
            yHigh = y;
        }
    }

    xC = (xLow + xHigh) / 2.0;
    dx2 = (xHigh - xLow) / 2.0 * (1.0 + pad);
    xLow = xC - dx2;
    xHigh = xC + dx2;

    yC = (yLow + yHigh) / 2.0;
    dy2 = (yHigh - yLow) / 2.0 * (1.0 + pad);
    yLow = yC - dy2;
    yHigh = yC + dy2;

    CNum zC(xC, yC);
    CNum z = data.front();
    Num r;
    Num rLow = abs(z - zC);
    Num rHigh = rLow;
    Num dr2;

    for (CNumList::const_iterator it = data.begin(); it != data.end(); it++)
    {
        z = *it;
        r = abs(z - zC);
        if (r > rHigh)
        {
            rHigh = r;
        }
        else if (r < rLow)
        {
            rLow = r;
        }
    }

    r = (rLow + rHigh) / 2.0;
    dr2 = (rHigh - rLow) / 2.0;
    rLow = r - dr2 * (1.0 + pad);
    rHigh = r + dr2 * (1.0 + pad);

    if ((rLow > 0.0) && (dr2 / r < 0.25))
    {
        // annulus
        dom.type = ANNULUS;
        dom.xC = xC;
        dom.yC = yC;
        dom.rLow = rLow;
        dom.rHigh = rHigh;
    }
    else
    {
        // rectangle
        dom.type = RECTANGLE;
        dom.xLow = xLow;
        dom.xHigh = xHigh;
        dom.yLow = yLow;
        dom.yHigh = yHigh;
    }
}

int main(int argc, char* argv[])
{
    CNumList from;
    CNumLists gr;
    //NumLists data;
    DomainList data;
    Num x, y;
//    Num xLow, xHigh, yLow, yHigh;
//    Int k = 0;
    Num r = 0.3;
    Num pad = 0.05;
//    Num S;
//    Num Ssum = 0.0;
//    LLInt n = 100000000;
//    LLInt nMax = 25000000;
    std::string prefix = "# ";
//    std::string baseName = "ir";
    std::string line;
    std::stringstream ss;
    Int wid = 20;
    //Int pl = prefix.length();
    Int prec = 12;
    Int precOld;

    try
    {
        if (argc > 1)
        {
            ss.str(argv[1]);
            ss >> r;
            ss.clear();
        }
        //    std::cout << prefix << "r = " << r << std::endl;

        if (argc > 2)
        {
            ss.str(argv[2]);
            ss >> pad;
            ss.clear();
        }
        //    std::cout << prefix << "padding = " << pad << std::endl;

//        if (argc > 3)
//        {
//            ss.str(argv[3]);
//            ss >> n;
//            ss.clear();
//        }
//        //    std::cout << prefix << "rays = " << n << std::endl;
//
//        if (argc > 4)
//        {
//            ss.str(argv[4]);
//            ss >> nMax;
//            ss.clear();
//        }

//        if (argc > 5)
//        {
//            ss.str(argv[5]);
//            ss >> baseName;
//            ss.clear();
//        }

//        Int n = 0;
        while (getline(std::cin, line))
        {
            if ((line[0] == '#') || (line == ""))
            {
                continue;
            }

            ss.str(line);
            ss >> x >> y;

//            std::cout << n++ << std::endl;

            from.push_back(CNum(x, y));

            ss.clear();
        }

        //    for (CNumList::iterator it = from.begin(); it != from.end(); it++)
        //    {
        //        std::cout << std::setw(w) << it->real();
        //        std::cout << std::setw(w) << it->imag();
        //        std::cout << std::endl;
        //    }

        groups(r, from, gr);

        std::cout.setf(std::ios::scientific);

        for (CNumLists::iterator grIt = gr.begin(); grIt != gr.end(); grIt++)
        {
            Domain dom;
            domain(*grIt, dom, pad);
//            S = dom.area();
//            Ssum += S;

            data.push_back(dom);

            precOld = std::cout.precision();
            std::cout.precision(prec);
            for (CNumList::iterator it = grIt->begin(); it != grIt->end(); it++)
            {
                std::cout << std::setw(wid) << it->real();
                std::cout << std::setw(wid) << it->imag();
                std::cout << std::endl;
            }
            std::cout.precision(precOld);
            std::cout << std::endl;
            std::cout << std::endl;
        }

        for (DomainList::iterator it = data.begin(); it != data.end(); it++)
        {
//            S = it->area();

//            LLInt nGroup = LLInt(round(S / Ssum * n));
//            LLInt m = nGroup / nMax;
//            LLInt rem = nGroup - m * nMax;
//            std::stringstream ss;

//            if (rem == 0)
//            {
//                m--;
//            }
//            for (LLInt i = 0; i <= m; ++i)
//            {
//                if ((i == m) && (rem == 0))
//                {
//                    break;
//                }
//                ss.str("");
//                ss << baseName << "-" << k << ".dat";
//                std::ofstream of;
//                of.open(ss.str().c_str());
//
//                of.precision(prec);
//
//                of << *it;
//                of << "  " << (i < m ? nMax : rem);
//                of << std::endl;
//
//                of.close();
//
//                std::cout.precision(prec);
//                std::cout << "# domain: " << *it;
//                std::cout << "  " << S;
//                std::cout << "  " << (i < m ? nMax : rem);
//                std::cout << std::endl;
//
//                k++;
//            }
            std::cout.precision(prec);
            std::cout << "#domain: " << *it;
            std::cout << std::endl;
        }
//        std::cout << "# rays= " << n << std::endl;
//        std::cout << "# Ssum= " << std::setprecision(12) << Ssum << std::endl;
//        std::cout << "# rays/Ssum= " << std::setprecision(12) << n/Ssum << std::endl;
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
