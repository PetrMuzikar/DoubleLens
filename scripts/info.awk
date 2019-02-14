#!/usr/bin/awk -f

BEGIN {
    minRays = 1e9;
    maxRays = -1;
}

/^$|#zero/ {
    next;
}

/##/ {
    MMS = $10;
    d1 = $11;
    Ds = $12;
    d = $13;
    mu1 = $14;
    beta = $15;
    nrE = $16;
    prec = $17;
    absPrec = $18;
    relPrec = $19;
    remPrec = $20;
    xMin = $21;
    xMax = $22;
    nx = $23;
    yMin = $24;
    yMax = $25;
    ny = $26;

    m1 = mu1 / (mu1 + (1 - mu1) * (1 - beta));

    next;
}

/rectangular pixels/ {
    px = $4;
    py = $6; 
    next;
}

/# rays=/ {
    rays = $3;
    next;
}

/# inside=/ {
    inside = $3;
    next;
}

/# outside=/ {
    outside = $3;
    next;
}

/# error=/ {
    error = $3;
    next;
}

/# norm=/ {
    norm = $3;
    next;
}

/# total time/ {
    time = $7;
    next;
}

/^#/ {
    next;
}

{ 
    if ($5 < minRays) {
        minRays = $5;
    }

    if ($5 > maxRays) {
        maxRays = $5;
    }
}

END {
    if (csv == 1) {
        printf("%s;", ARGV[1]);
        printf("%g;", d);
        printf("%g;", beta);
        printf("%g;%g;", px, py);
        printf("%g;", mu1);
        printf("%g;", m1);
        printf("%g;", nrE);
        printf("%.12g;%.12g;%.12g;%.12g;", xMin, xMax, yMin, yMax);
        printf("%g;", prec);
        printf("%g;", absPrec);
        printf("%g;", relPrec);
        printf("%g;", remPrec);
        printf("%d;", rays);
        printf("%d;", inside);
        printf("%d;", outside);
        printf("%d;", error);
        printf("%g;", norm);
        printf("%d;", minRays);
        printf("%d;", maxRays);
        printf("%g", time);
    }
    else {
        printf("file = %s\n", ARGV[1]);
        printf("d = %g\n", d);
        printf("beta = %g\n", beta);
        printf("pixels = %g %g\n", px, py);
        printf("mu1 = %g\n", mu1);
        printf("m1 = %g\n", m1);
        printf("nrE = %g\n", nrE);
        printf("sourcePlaneRect = %.12g %.12g %.12g %.12g\n", xMin, xMax, yMin, yMax);
        printf("prec(integ) = %g\n", prec);
        printf("absPrec(roots) = %g\n", absPrec);
        printf("relPrec(roots) = %g\n", relPrec);
        printf("remPrec(roots) = %g\n", remPrec);
        printf("rays = %d\n", rays);
        printf("inside = %d\n", inside);
        printf("outside = %d\n", outside);
        printf("error = %d\n", error);
        printf("norm = %g\n", norm);
        printf("minRays = %d\n", minRays);
        printf("maxRays = %d\n", maxRays);
        printf("time = %g h\n", time);
    }
}
