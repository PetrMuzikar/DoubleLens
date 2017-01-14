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

    xLowGrid = 0;
    xHighGrid = 0;
    nxGrid = 0;
    yLowGrid = 0;
    yHighGrid = 0;
    nyGrid = 0;

    m1 = mu1;
}

{
    if (random == 0) {
        # Sobol
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

        xLowGrid = $18;
        xHighGrid = $19;
        nxGrid = $20;
        yLowGrid = $21;
        yHighGrid = $22;
        nyGrid = $23;
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

        xLowGrid = $18;
        xHighGrid = $19;
        nxGrid = $21;
        yLowGrid = $21;
        yHighGrid = $22;
        nyGrid = $23;
    }

    m1 = m1mu1(mu1, beta);
}

END {
    if (todo == 0) {
        # for gnuplot
        printf("\"%g\" ", mu1);     
        printf("\"%g\" ", d);     
        printf("\"%g\" ", beta);     
        printf("\"%.10g\" ", xLowGrid);     
        printf("\"%.10g\" ", xHighGrid);     
        printf("\"%.10g\" ", yLowGrid);     
        printf("\"%.10g\"\n", yHighGrid);     
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
        printf("%g %g %g %.10g %.10g %d %.10g %.10g %d ", m1, d, beta, xLowGrid, xHighGrid, nxGrid, yLowGrid, yHighGrid, nyGrid);
    }
}
