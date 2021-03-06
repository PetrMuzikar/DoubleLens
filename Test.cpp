#include "ML/Vec.hpp"
#include "ML/SVec.hpp"
#include "ML/Mat.hpp"
#include "ML/SMat.hpp"
#include "ML/Pow.hpp"
#include "ML/Poly.hpp"
#include "ML/Point.hpp"
#include "ML/PolyCNumSolver.hpp"

#include "GravLens.hpp"
#include "InputRays.hpp"
#include "SourcePlaneGrid.hpp"
#include "InverseRayShooting.hpp"
#include "Test.hpp"

void debugRaysToPixel(const std::string& prefix)
{
    InverseRayShooting<GravLens> irs;
    std::cin >> irs;
    irs.setOutputPrefix(prefix);

    GravLens& lens = irs.getLens();
    SourcePlaneGrid& spg = irs.getSPG();

    std::cout << lens << std::endl;
    spg.printHeader(std::cout);

    for (RaysToPixelCollection::const_iterator it = spg.rtpc().begin(); it != spg.rtpc().end();  it++)
    {
        PointNum coords = it->coords_;
        VecPointNum images;
        lens.images(coords, images);
        PointNum image, source;
        Int w = 15;
        Num jac;
        std::cout << "# nRoots: " << images.size() << std::endl;
        std::cout << "#" << std::setw(w-1) << "image_x";
        std::cout << std::setw(w) << "image_y";
        std::cout << std::setw(w) << "source_x";
        std::cout << std::setw(w) << "source_y";
        std::cout << std::setw(w) << "jac";
        std::cout << std::setw(w) << "1 / jac" << std::endl;
        for (UInt k = 0; k < images.size(); ++k)
        {
            image = images[k];
            source = lens.raySimple(image);
            jac = lens.jac(image);
            std::cout << std::scientific << std::setprecision(5);
            std::cout << std::setw(w) << image.x();
            std::cout << std::setw(w) << image.y();
            std::cout << std::setw(w) << source.x();
            std::cout << std::setw(w) << source.y();
            std::cout << std::setw(w) << jac;
            std::cout << std::setw(w) << 1.0 / abs(jac) << std::endl;
        }

        std::cout << "# mag: " << lens.magSimple(coords) << std::endl;
        std::cout << std::endl;
    }
}

void debug(const std::string& prefix)
{
    using std::abs;

    GravLens lens(1, 0.5, 10, 4.0/1000.0, 0.5, 0, 500, 1e-05);
//    InverseRayShooting<GravLens> irs;
//    std::cin >> irs;
//    irs.setOutputPrefix(prefix);

//    GravLens& lens = irs.getLens();

    // From Mathematica:
    PointNum img[] =
    {
        PointNum(0.0019247898537471289,-0.002000047280965616),
        PointNum(0.0019999535568391807,-0.001924794650398635),
        PointNum(0.0020000000000000412,250.00199990400358),
        PointNum(0.0020000523216666425,-0.002075209422016799),
        PointNum(0.0020752042677470066,-0.0019999486517388427)

    };
    const Int Nimg = sizeof(img) / sizeof(img[0]);

    PointNum defl, deflSimple;
    PointNum bads[] =
    {
//        PointNum(0.09559996126, 5.695974436)
        PointNum(0.0, 249.998)
//        PointNum(1, 1)
//        PointNum(-1.365234375000000e+00, 7.753906250000000e-01),
//        PointNum(1, 1.2104113),
//        PointNum(6.649780e-02, 1.800750e+00),
//        PointNum(1.800750e+00, 6.649780e-02)
    };
    const Int N = sizeof(bads) / sizeof(bads[0]);
    std::cout << lens << std::endl;
//    std::cout << std::setprecision(15) << std::scientific;
    for (Int i = 0; i < N; ++i)
    {
        try
        {
            defl = lens.rayInteg(bads[i]);
            deflSimple = lens.raySimple(bads[i]);
        }
        catch (const IntegrationException& iex)
        {
            std::cerr << prefix << iex.what() << std::endl;
            continue;
        }
        catch (const std::exception& ex)
        {
            throw;
        }
//
        std::cout << "ray:    " << std::setw(10) << bads[i]  << std::endl;
        std::cout << "integ:  " << std::setw(10) << defl << std::endl;
        std::cout << "simple: " << std::setw(10) << deflSimple << std::endl;

        std::cout << std::endl;

        std::cout << "# source:    " << std::setw(10) << bads[i] << std::endl;
//        CNum y = bads[i].toC();
        VecPointNum images;
        PointNum y = bads[i];
        lens.images(y, images);
        PointNum image, source;
        Num jac;
        std::cout << "# nRoots: " << images.size() << std::endl;
        for (UInt k = 0; k < images.size(); ++k)
        {
            image = images[k];
            source = lens.raySimple(image);
            jac = lens.jac(image);
            std::cout << std::scientific << std::setprecision(5);
            std::cout << std::setw(30) << image.toC();
            std::cout << std::setw(30) << source.toC();
            std::cout << std::setw(15) << jac;
            std::cout << std::setw(15) << 1.0 / abs(jac) << std::endl;
        }

        std::cout << "mag: " << lens.magSimple(y) << std::endl;
        std::cout << std::endl;

        std::cout << "Images from Mathematica:\n";
        std::cout << std::scientific << std::setprecision(5);
        for (Int k = 0; k < Nimg; ++k)
        {
            image = img[k];
            source = lens.raySimple(image);
            jac = lens.jac(image);
            std::cout << std::setw(30) << image.toC();
            std::cout << std::setw(30) << source.toC();
            std::cout << std::setw(15) << jac;
            std::cout << std::setw(15) << 1.0 / abs(jac) << std::endl;
        }
    }

#ifdef INTEGRATE_PHOTON_PATH
    std::cout << prefix << "integrating\n";
#else
    std::cout << prefix << "simple\n";
#endif

}

void debugRays(const std::string& prefix)
{
    InverseRayShooting<GravLens> irs;
    std::cin >> irs;

    InputRays ir = irs.getIR();
    GravLens lens = irs.getLens();
    ir.setOutputPrefix(prefix);
    lens.setOutputPrefix(prefix);

    Int w = 15;

    std::cout << ir << std::endl;
    std::cout << lens << std::endl;

    std::cout << std::setw(2*w) << "ray";
    std::cout << std::setw(2*w) << "simple";
    std::cout << std::setw(2*w) << "integ" << std::endl;

    for (Int k = 0; k < ir.getNumRays(); ++k)
    {
        PointNum inRay = ir.getRay(k);
        PointNum simpleRay = lens.raySimple(inRay);
        PointNum integRay = lens.rayInteg(inRay);

        std::cout << std::setw(w) << inRay.x();
        std::cout << std::setw(w) << inRay.y();
        std::cout << std::setw(w) << simpleRay.x();
        std::cout << std::setw(w) << simpleRay.y();
        std::cout << std::setw(w) << integRay.x();
        std::cout << std::setw(w) << integRay.y() << std::endl;
    }
}

void run(const std::string& prefix)
{
    InverseRayShooting<GravLens> irs;
    std::cin >> irs;
    irs.setOutputPrefix(prefix);

    std::cout << irs.printInput() << std::endl;

#ifdef INTEGRATE_PHOTON_PATH
    std::cout << prefix << "Integrating...\n";
    irs.shootingInteg();
#else
    std::cout << prefix << "Computing using the expression for deflection...\n";
    irs.shootingSimple();
#endif

    irs.computeMagnification();
    std::cout << irs << std::flush;
}

void imageOfSourcePlane(int argc, char* argv[], const std::string& prefix)
{
//    InverseRayShooting<GravLens> irs;
//    GravLens& lens = irs.getLens();
//    SourcePlaneGrid& spg = irs.getSPG();

    GravLens lens;
    SourcePlaneGrid spg;
    Int nPoints;
    Int nBoundary;
    PointNum step;
    Int w = 20;
    Int wi = 6;
    //Num absPrec = 1e-8;
    //Num absPrec = 1e-9;
    //Num relPrec = absPrec;
    ////Num remPrec = 5e-6;
    //Num remPrec = 1e-5;

    // argv >= 4
    if (argc >= 5)
    {
        throw std::runtime_error((prefix + "imageOfSourcePlane: accepts only one input file!").c_str());
    }

    std::string inp = argv[3];
    std::streambuf* cinbuf = std::cin.rdbuf();
    std::ifstream inf;
    inf.open(inp.c_str());
    if (!inf.is_open())
    {
        throw std::runtime_error((prefix + "imageOfSourcePlane: Error while opening the file " + inp + ".").c_str());
    }
    std::cin.rdbuf(inf.rdbuf());

    std::cin >> nPoints;
    std::cin >> lens;
    std::cin >> spg;

    gsl_qrng* q = gsl_qrng_alloc(gsl_qrng_sobol, 2);
    Num point[2], x, y;
    PointNum minGrid = spg.minGrid();
    PointNum maxGrid = spg.maxGrid();
    PointNum diffGrid = maxGrid - minGrid;
    PointNum source;
    VecPointNum images;

    std::cout.precision(12);
    std::cout.setf(std::ios::scientific);

    for (Int i = 0; i < nPoints; ++i)
    {
        gsl_qrng_get(q, point);
        x = minGrid.x() + diffGrid.x() * point[0];
        y = minGrid.y() + diffGrid.y() * point[1];
        source = PointNum(x, y);
        lens.images(source, images);

        for (UInt j = 0; j < images.size(); ++j)
        {
            std::cout << std::setw(w) << source.x();
            std::cout << std::setw(w) << source.y();
            std::cout << std::setw(w) << images[j].x();
            std::cout << std::setw(w) << images[j].y();
            std::cout << std::setw(wi) << images.size();
            std::cout << std::endl;
        }
    }

    std::cout << prefix << "Points on the rectangle:\n";

    nBoundary = Int(sqrt(nPoints));
    step = diffGrid / nBoundary;

    y = minGrid.y();
    for (Int i = 0; i <= nBoundary; ++i)
    {
        x = minGrid.x() + step.x() * i;        
        source = PointNum(x, y);
        lens.images(source, images);
        for (UInt j = 0; j < images.size(); ++j)
        {
            std::cout << std::setw(w) << source.x();
            std::cout << std::setw(w) << source.y();
            std::cout << std::setw(w) << images[j].x();
            std::cout << std::setw(w) << images[j].y();
            std::cout << std::setw(wi) << images.size();
            std::cout << std::endl;
        }
    }

    y = maxGrid.y();
    for (Int i = 0; i <= nBoundary; ++i)
    {
        x = minGrid.x() + step.x() * i;        
        source = PointNum(x, y);
        lens.images(source, images);
        for (UInt j = 0; j < images.size(); ++j)
        {
            std::cout << std::setw(w) << source.x();
            std::cout << std::setw(w) << source.y();
            std::cout << std::setw(w) << images[j].x();
            std::cout << std::setw(w) << images[j].y();
            std::cout << std::setw(wi) << images.size();
            std::cout << std::endl;
        }
    }

    x = minGrid.x();
    for (Int i = 0; i <= nBoundary; ++i)
    {
        y = minGrid.y() + step.y() * i;        
        source = PointNum(x, y);
        lens.images(source, images);
        for (UInt j = 0; j < images.size(); ++j)
        {
            std::cout << std::setw(w) << source.x();
            std::cout << std::setw(w) << source.y();
            std::cout << std::setw(w) << images[j].x();
            std::cout << std::setw(w) << images[j].y();
            std::cout << std::setw(wi) << images.size();
            std::cout << std::endl;
        }
    }

    x = maxGrid.x();
    for (Int i = 0; i <= nBoundary; ++i)
    {
        y = minGrid.y() + step.y() * i;        
        source = PointNum(x, y);
        lens.images(source, images);
        for (UInt j = 0; j < images.size(); ++j)
        {
            std::cout << std::setw(w) << source.x();
            std::cout << std::setw(w) << source.y();
            std::cout << std::setw(w) << images[j].x();
            std::cout << std::setw(w) << images[j].y();
            std::cout << std::setw(wi) << images.size();
            std::cout << std::endl;
        }
    }

    gsl_qrng_free(q);
    inf.close();
    std::cin.rdbuf(cinbuf);
}

void shooting(int argc, char* argv[], const std::string& prefix)
{
    InverseRayShooting<GravLens> irs;

    // argv >= 4
    if (argc >= 5)
    {
        throw std::runtime_error((prefix + "shooting: accepts only one input file!").c_str());
    }

    irs.setOutputPrefix(prefix);

    std::string inp = argv[3];
    std::streambuf* cinbuf = std::cin.rdbuf();
    std::ifstream inf;
    inf.open(inp.c_str());
    if (!inf.is_open())
    {
        throw std::runtime_error((prefix + "shooting: Error while opening the file " + inp + ".").c_str());
    }
    std::cin.rdbuf(inf.rdbuf());

    std::cin >> irs;
    irs.setOutputPrefix(prefix);

    std::cout << irs.printInput() << std::endl;

#ifdef INTEGRATE_PHOTON_PATH
    std::cout << prefix << "Integrating...\n";
    irs.shootingInteg();
#else
    std::cout << prefix << "Computing using the expression for deflection...\n";
    irs.shootingSimple();
#endif

    irs.computeMagnification();
    std::cout << irs << std::flush;

    inf.close();
    std::cin.rdbuf(cinbuf);
}

void combineData(int argc, char* argv[], const std::string& prefix)
{
    std::string fileName;
    InverseRayShooting<GravLens> irs;
    SourcePlaneGrid& spg = irs.getSPG();

    // argc > 4
    if (argc <= 4)
    {
        throw std::runtime_error((prefix + "combineData: not enough input files!").c_str());
    }
    irs.setOutputPrefix(prefix);

    std::string inp = argv[3];
    std::streambuf* cinbuf = std::cin.rdbuf();
    std::ifstream inf;
    inf.open(inp.c_str());
    if (!inf.is_open())
    {
        throw std::runtime_error((prefix + "combineData: Error while opening the file " + inp + ".").c_str());
    }
    std::cin.rdbuf(inf.rdbuf());

    std::cin >> irs;
    irs.setOutputPrefix(prefix);

    std::cout << irs.printInput() << std::endl;

    spg.clear();
    for (Int k = 4; k < argc; ++k)
    {
        fileName = argv[k];
        spg.readData(fileName);
//        std::cerr << fileName << "read.\n";
    }

    irs.computeMagnification();
    spg.shiftCorners(true);
    spg.printZeros(true);
    std::cout << irs;
    std::cout << prefix << "norm= " << irs.norm() << std::endl;

    inf.close();
    std::cin.rdbuf(cinbuf);
}

void rays(int argc, char* argv[], const std::string& prefix)
{
    GravLens g;
    std::string inp = argv[3];
    std::streambuf* cinbuf = std::cin.rdbuf();
    std::ifstream inf;
    inf.open(inp.c_str());
    if (!inf.is_open())
    {
        throw std::runtime_error((prefix + "rays: Error while opening the file " + inp + ".").c_str());
    }
    std::cin.rdbuf(inf.rdbuf());

    std::cin >> g;
    g.setOutputPrefix(prefix);
    std::cout << g << std::endl;

    if (argc > 4)
    {
        inf.close();
        inf.clear();
        inp = argv[4];
        inf.open(inp.c_str());
        if (!inf.is_open())
        {
            throw std::runtime_error((prefix + "rays: Error while opening the file " + inp + ".").c_str());
        }
    }

    PointNum init, simple, integ; 
    Int w = 20;
    std::cout << prefix << std::setw(w-prefix.length()) << "xRay";
    std::cout << std::setw(w) << "yRay";
    std::cout << std::setw(w) << "xSimple";
    std::cout << std::setw(w) << "ySimple";
    std::cout << std::setw(w) << "xInteg";
    std::cout << std::setw(w) << "yInteg";
    std::cout << std::endl << std::scientific;
    std::cout.precision(12);

    while (std::cin >> init)
    {
        try
        {
            integ = g.rayInteg(init);
        }
        catch (IntegrationException& ex)
        {
            std::cerr << ex.what() << std::endl;
            continue;
        }
        simple = g.raySimple(init);
        std::cout << std::setw(w) << init.x();
        std::cout << std::setw(w) << init.y();
        std::cout << std::setw(w) << simple.x();
        std::cout << std::setw(w) << simple.y();
        std::cout << std::setw(w) << integ.x();
        std::cout << std::setw(w) << integ.y();
        std::cout << std::endl;
    }

    inf.close();
    std::cin.rdbuf(cinbuf);
}

void magPoint(int argc, char* argv[], const std::string& prefix)
{
    GravLens g;
    std::string inp = argv[3];
    std::streambuf* cinbuf = std::cin.rdbuf();
    std::ifstream inf;
    inf.open(inp.c_str());
    if (!inf.is_open())
    {
        throw std::runtime_error((prefix + "rays: Error while opening the file " + inp + ".").c_str());
    }
    std::cin.rdbuf(inf.rdbuf());

    std::cin >> g;

    g.setOutputPrefix(prefix);

    std::cout << g << std::endl;

    if (argc > 4)
    {
        inf.close();
        inf.clear();
        inp = argv[4];
        inf.open(inp.c_str());
        if (!inf.is_open())
        {
            throw std::runtime_error((prefix + "magPoint: Error while opening the file " + inp + ".").c_str());
        }
    }

    PointNum source;
    Int w = 20;
    Int wi = 8;
    std::cout << prefix;
    std::cout << std::setw(w-prefix.length()) << "xS";
    std::cout << std::setw(w) << "yS";
    std::cout << std::setw(w) << "xImage";
    std::cout << std::setw(w) << "yImage";
    std::cout << std::setw(w) << "rem";
    std::cout << std::setw(w) << "jac";
    std::cout << std::setw(w) << "1 / jac";
    std::cout << std::setw(wi) << "nImages";
    std::cout << std::endl;
    std::cout << std::scientific;
    std::cout.precision(12);
    while (inf >> source)
    {
        VecPointNum images;
        g.images(source, images);
        Int nImages = images.size();
        Num mag = g.magSimple(source);
        for (Int i = 0; i < nImages; ++i)
        {
            Num jac = g.jac(images[i]);
            Num rem = abs(source - g.raySimple(images[i]));
            std::cout << std::setw(w) << source.x();
            std::cout << std::setw(w) << source.y();
            std::cout << std::setw(w) << images[i].x();
            std::cout << std::setw(w) << images[i].y();
            std::cout << std::setw(w) << rem;
            std::cout << std::setw(w) << jac;
            std::cout << std::setw(w) << 1.0 / jac;
            std::cout << std::setw(wi) << nImages;
            std::cout << std::endl;
        }
        std::cout << prefix << "mag= " << std::setw(7*w-5-prefix.length()) << mag;
        std::cout << std::endl;
    }

    inf.close();
    std::cin.rdbuf(cinbuf);
}

