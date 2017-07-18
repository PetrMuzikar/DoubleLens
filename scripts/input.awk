#!/usr/bin/awk -f

function m1mu1(mu1, beta) {
    return mu1 / (mu1 + (1 - beta) * (1 - mu1));
}

BEGIN {
    #todo = 0;

    type = 0;
    S = 0;
    xLow = 0;
    xHigh = 0;
    yLow = 0;
    yHigh = 0;
    n = 0;
    start = 0;
    seed = 0;

    M_MSun = 0;
    mu1 = 0;
    Dskpc = 0;
    d = 0;
    d1 = 0;
    beta = 0;
    nrE = 0;
    prec = 0;

    absPrec = 0;
    relPrec = 0;
    remPrec = 0;

    xLowGrid = 0;
    xHighGrid = 0;
    nxGrid = 0;
    yLowGrid = 0;
    yHighGrid = 0;
    nyGrid = 0;

    m1 = mu1;
}

{
    if (random != 1) {
        # Sobol or Halton
        type = $2;
        S = $3;
        xLow = $4;
        xHigh = $5;
        yLow = $6;
        yHigh = $7;
        n = $8;
        start = $9; #!!!

        M_MSun = $10;
        mu1 = $11;
        Dskpc = $12;
        d = $13;
        d1 = $14;
        beta = $15;
        nrE = $16;
        prec = $17;

        absPrec = $18;
        relPrec = $19;
        remPrec = $20;

        xLowGrid = $21;
        xHighGrid = $22;
        nxGrid = $23;
        yLowGrid = $24;
        yHighGrid = $25;
        nyGrid = $26;
    }
    else {
        # random
        type = $2;
        S = $3;
        xLow = $4;
        xHigh = $5;
        yLow = $6;
        yHigh = $7;
        n = $8;
        seed = $9;

        M_MSun = $10;
        mu1 = $11;
        Dskpc = $12;
        d = $13;
        d1 = $14;
        beta = $15;
        nrE = $16;
        prec = $17;

        absPrec = $18;
        relPrec = $19;
        remPrec = $20;

        xLowGrid = $21;
        xHighGrid = $22;
        nxGrid = $23;
        yLowGrid = $24;
        yHighGrid = $25;
        nyGrid = $26;
    }

    m1 = m1mu1(mu1, beta);
}

END {
    if (todo == 0) {
        # for gnuplot
        printf("\"%g\" ", mu1);     
        printf("\"%g\" ", d);     
        printf("\"%g\" ", beta);     
        printf("\"%.12g\" ", xLowGrid);     
        printf("\"%.12g\" ", xHighGrid);     
        printf("\"%.12g\" ", yLowGrid);     
        printf("\"%.12g\"\n", yHighGrid);     
    }
    else if (todo == 1) {
        # for computation of normalization
#        norm = (xHighGrid - xLowGrid) * (yHighGrid - yLowGrid);
#        norm /= (xHigh - xLow) * (yHigh - yLow);
#        printf("%.14g", norm);
        temp = (xHighGrid - xLowGrid) * (yHighGrid - yLowGrid);
        temp /= nxGrid * nyGrid;
        printf("%.14g", temp);
    }
    if (todo == 2) {
        # for computation of magnification
        printf("%g %g %g %.12g %.12g %d %.12g %.12g %d ", m1, d, beta, xLowGrid, xHighGrid, nxGrid, yLowGrid, yHighGrid, nyGrid);
    }
}
