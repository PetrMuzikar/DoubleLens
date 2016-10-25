#include <iomanip>
#include <sstream>
#include <string>
#include "PolyCNumSolver.hpp"

namespace ML
{
void PolyCNumSolver::transf(const CNum p[], const Int deg, CNum pTr[], Int& degTr) const
{
    // This procedure performs a Lehmer-Schur transformation
    // T(p)(z) = conj(a[0])*p(z) - a[n-1]*g(z),
    // where g(z) = conj(a[n-1]) + conj(a[n-2])*z + ... + conj(a[0])*z^{n-1}.
    // The degree of the polynomial p is less than n.

    using std::abs;
    PolyCNum w(deg+1);
    Int i;
    Num maxi;

    for (i = 0; i <= deg; i++)
    {
        w[i] = conj(p[deg-i]);
    }
    for (i = 0; i <= deg; i++)
    {
        pTr[i] = w[deg] * p[i] - p[deg] * w[i];
    }
    // pTr has a maximal degree deg-1

    // normalization
//    const Num pTr0 = abs(real(pTr[0]));
//    if (pTr0 > EPS)
//    {
//        // pTr[0] is real
//        for (i = 0; i <= deg - 1; i++)
//        {
//            pTr[i] /= pTr0;
//        }
//    }
    maxi = abs(pTr[0]);
    for (i = 1; i <= deg; i++)
    {
        if (abs(pTr[i]) > maxi)
        {
            maxi = abs(pTr[i]);
        }
    }
    for (i = 0; i <= deg; i++)
    {
        pTr[i] /= maxi;
    }

    for (i = deg - 1; i >= 0; i--)
    {
        if (abs(pTr[i]) >= EPS)
        {
            break;
        }
    }
    degTr = i;
}

void PolyCNumSolver::linTransf(const PolyCNum& p, const CNum rho, const CNum c, PolyCNum& pTr) const
{
    // Given a polynomial p(z), this function computes coefficients of a polynomial p(rho*z+c).

    Int deg = p.degree();
    PolyCNum rhoPow(deg+1);
    PolyCNum cPow(deg+1);
    CNum val;
    Int i, j;

    rhoPow[0] = ONE;
    cPow[0] = ONE;
    for (i = 1; i <= deg; i++)
    {
        rhoPow[i] = rho * rhoPow[i-1];
        cPow[i] = c * cPow[i-1];
    }

    for (i = 0; i <= deg; i++)
    {
        val = ZERO;
        for (j = i; j <= deg; j++)
        {
            val += combNum(j, i) * p[j] * cPow[j-i];
        }
        pTr[i] = val * rhoPow[i];
    }
}

Int PolyCNumSolver::decide(const PolyCNum& p, const Num rho, const CNum c) const
{
    // This function decides whether there is a root of the polynomial p
    // in the circle with center in c and radius rho.
    //
    // Returns:
    // 2 if the root is c
    // 1 if the root is inside
    // 0 if no root is inside
    // -1 if the root may be just on the circle

    using std::abs;
    PolyCNum pTr(p.size());
    PolyCNum work(p.size());
    MatCNum mat(p.size()+2, p.size());
    Int deg = p.degree();
    CNum val;
    Int i, k, deg1, deg2, degWork;
    bool notEx;

    if (abs(p(c)) < EPS)
    {
        return 2;
    }

    linTransf(p, rho, c, pTr);
    for (i = 0; i <= deg; i++)
    {
        mat[0][i] = pTr[i];
    }

    deg1 = deg;
    for (k = 1; k <= deg + 1; k++)
    {
        // najde nejmensi k tak, ze (Tk f)(c) = 0
        transf(mat[k-1], deg1, mat[k], deg2);
        if (abs(mat[k][0]) < EPS)
        {
            break;
        }
        deg1 = deg2;
    }

    for (i = 0; i <= deg1; i++)
    {
        work[i] = mat[k-1][i];  // koeficienty polynomu (T(k-1) f)(z)
    }
    degWork = deg1;

//    deg1 = deg;
    for (i = 1; i <= k-1; i++)
    {
        val = mat[i][0];
        if ((real(val) < 0.0) && (abs(imag(val)) < EPS))
        {
            return 1;
        }
    }

    notEx = true;
    if (degWork == 0)
    {
        for (i = 1; i <= k-1; i++)
        {
            val = mat[i][0];
            notEx = notEx && (real(val) > 0.0) && (abs(imag(val)) < EPS);
        }
    }
    if (notEx)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

CNum PolyCNumSolver::rootLehSch(const PolyCNum& p, const Num absPrec, const Num relPrec) const
{
    using std::abs;
    CNum c, c2, rem;
    Num radius;
    Int i, j, found, step;
    Int deg = p.degree();
    SVec<CNum,8> factors;

    static const Num A = 1.5 / cos(PI / 8.0);
    static const Num SC = 1.01;
    static const Num REDUCE = 0.8;
    static const Num PI4 = PI / 4.0;
    static const Int STEPS = 1000;

    for (i = 0; i <= 7; i++)
    {
        factors[i] = A * exp(i * PI4 * I);
    }

//    c = 1.0066;
//    radius = 0.016;
//    std::cout << decide(p, radius, c) << std::endl;
//    radius /= 2.0;
//    std::cout << decide(p, radius, c) << std::endl;
//    radius /= 2.0;
//    std::cout << decide(p, radius, c) << std::endl;
//    std::cout << decide(p, radius * REDUCE, c + radius * A) << std::endl;

    radius = 1.0;
    c = 0.0;
    step = 0;

    while ((radius > absPrec + relPrec * abs(c)) && (++step < STEPS))
    {
        std::cerr << "step " << step << std::endl;
        found = decide(p.data(), deg, radius, c);

        do
        {
            switch (found)
            {
            case 2:
//                return c;
//                radius /= 2.0;
                break;
            case 1:
                while (found == 1)
                {
                    radius /= 2.0;
                    found = decide(p.data(), deg, radius, c);
                    while (found == -1)
                    {
                        radius *= SC;
                        found = decide(p.data(), deg, radius, c);
                    }
                }
                break;
            case 0:
                while (found == 0)
                {
                    radius *= 2.0;
                    found = decide(p.data(), deg, radius, c);
                    while (found == -1)
                    {
                        radius /= SC;
                        found = decide(p.data(), deg, radius, c);
                    }
                }
                radius /= 2.0;
                break;
            case -1:
                // repeat the switch
                radius /= SC;
                found = decide(p.data(), deg, radius, c);
                break;
            }
        }
        while (found == -1);

        std::cerr << "c = " << c << std::endl;
        std::cerr << "r = " << radius/2 << " => found = " << decide(p.data(), deg, radius/2, c) << std::endl;
        std::cerr << "r = " << radius << " => found = " << decide(p.data(), deg, radius, c) << std::endl;
        std::cerr << "r = " << 2*radius << " => found = " << decide(p.data(), deg, 2*radius, c) << std::endl;

        if (found == 2)
        {
//            c += 0.4 * radius * exp(Num(rand()) * 2.0 * PI * I / Num(RAND_MAX));
            radius /= 2.0;
            continue;
        }

        // Now in the circle with radius=radius there is no root
        // and in the circle with radius=2*radius there is a root

        std::cerr << " r = " << REDUCE * radius << std::endl;
        for (j = 0; j <= 7; j++)
        {
            c2 = radius * factors[j];
            found = decide(p.data(), deg, REDUCE * radius, c + c2);
            std::cerr << " c = " << c + c2 << " => found = " << found << std::endl;
            while (found == -1)
            {
                radius *= SC;
                found = decide(p.data(), deg, REDUCE * radius, c + c2);
            }
            if (found >= 1)
            {
                radius *= REDUCE;
                c += c2;
                break;
            }
        }

        if ((j > 7) && (!found))
        {
            std::stringstream ss;
            ss << "PolyCNumSolver::rootLehSch: no root in the annulus:\n";
            ss << "# c = " << c << "; r = " << radius;
            throw std::runtime_error(ss.str());
        }

//        std::cerr << step << " ";
//        std::cerr << std::setprecision(15) << std::scientific << c.real() << " ";
//        std::cerr << std::setprecision(15) << std::scientific << c.imag() << " ";
//        std::cerr << radius << std::endl;
    }

    if (step >= STEPS)
    {
        throw std::runtime_error("PolyCNumSolver::rootLehSch: maximal number of steps reached!");
    }

    return c;
}

void PolyCNumSolver::rootsLehSch(const Num absPrec, const Num relPrec, VecCNum& roots) const
{
    // Lehmer-Schur method for computing roots of cpolynomials with complex coefficients.
    using std::abs;

    Int deg = p_.degree();
    PolyCNum p1(p_), p2;
    CNum c, val;

    // for all roots
    for (Int i = 0; i < deg; i++)
    {
        std::cerr << "# poly = " << p1("x") << std::endl;

        c = rootLehSch(p1, absPrec, relPrec);

        roots[i] = c;
//        horner(p1.data(), deg-i, c, p2.data());
        p1.horner(c, val, p2);
//        copyData(deg - i, p2.data(), p1.data());
        p1 = PolyCNum(p2);
//        normalize(deg-i-1, p1.data());
    }
}

void PolyCNumSolver::rootsDurKer(const Num absPrec, const Num relPrec, const VecCNum& rootsInit, VecCNum& roots)
{
    //  Durandova-Kernerova iteracni metoda pro hledani komlexnich
    //  korenu polynomu s komplexnimi koeficienty,
    //  viz http://en.wikipedia.org/wiki/Durand-Kerner_method.

    using std::abs;

    Int deg = p_.degree();
    PolyCNum work(deg+1);
    VecCNum rootsNew(deg);
    VecCNum rootsOld(deg);
    bool finished;
    Int i, j, n;
    CNum num;

    const Int NITER = 80;

    for (i = 0; i < deg; i++)
    {
        rootsOld[i] = rootsInit[i];
        rootsNew[i] = rootsInit[i];
    }

    for (i = 0; i <= deg; i++)
    {
        work[i] = p_[i] / p_[deg];
    }

    // for all iterations (max NITER)
    for (n = 1; n <= NITER; n++)
    {
        // for all roots
        for (i = 0; i < deg; i++)
        {
            num = ONE;
            for (j = 0; j < deg; j++)
            {
                if (j != i) num /= (rootsNew[i] - rootsNew[j]);
            }
            rootsNew[i] -= num * horner(&work[0], deg, rootsNew[i]);
        }

        finished = true;
        for (i = 0; (i < deg) && finished; ++i)
        {
            finished &= (abs(rootsNew[i] - rootsOld[i]) < absPrec + abs(rootsNew[i]) * relPrec);
        }
        if (finished)
        {
            for (j = 0; j < deg; j++)
            {
                roots[j] = rootsNew[j];
            }
            return;
        }
        for (i = 0; i < deg; i++)
        {
            rootsOld[i] = rootsNew[i];
        }
    }

//    std::cerr << "PolyCNumSolver::rootsDurKer: warning - Reached a maximal number of iterations!\n";

    for (i = 0; i < deg; i++)
    {
        roots[i] = rootsNew[i];
    }

    throw std::runtime_error("PolyCNumSolver::rootsDurKer: warning - Reached a maximal number of iterations!");
}

}
