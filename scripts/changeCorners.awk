#!/usr/bin/awk -f

BEGIN {
    width = 20;
    wWidth = 22;
    w = 10;
    w1 = 15;
    intWidth = 5;
    prec = 11;
    precHigh = 13;
    raysGnuplot = 1000000;
    magGnuplot = 1e10;
    relGnuplot = 1; 
}

/#zero$/ {
    printf("%5d%5d", $1, $2);
    printf("%20.11e%20.11e", $3, $4);
    printf("%10d", raysGnuplot);     
    printf("%15g%15g", magGnuplot, magGnuplot);
    printf("%22g%22g", magGnuplot, magGnuplot);
    printf("%15g%15g", relGnuplot, relGnuplot);
    printf("%10s\n", "#zero");
    next;
}

{
    print;
}
