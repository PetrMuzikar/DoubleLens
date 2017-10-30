#!/usr/bin/awk -f

#function areaRectangle(x1, x2, y1, y2) {
#    return (x2 - x1) * (y2 - y1);
#}
#
#function areaAnnulus(r1, r2) {
#    pi = atan2(1, 1);
#    return pi * (r2**2 - r1**2);
#}

BEGIN {
    Ssum = 0;
    line = 0;
    if (n == "")
    {
        n = 100000000;
    }
    if (nMax == "") {
        nMax = 50000000;
    }
}

/#domain:/ {
    if (Sread == "") {
        Ssum += $7; 
    }
    else {
        S = $7;
        nDomain = int(S / Sread * n);
        m = int(nDomain / nMax);
        rem = int(nDomain % nMax);
        #print S, Sread, nDomain, m, rem;

        #for (i = 0; i <= m; ++i) {
        #    if ((i == m) && (rem == 0)) {
        #        break;
        #    }
        #    for (k = 2; k <= NF; ++k) {
        #        printf("%s ", $k);
        #    }
        #    printf("%d\n", i < m ? nMax : rem);
        #} 

        for (i = 1; i <= m; ++i) {
            for (k = 2; k <= NF; ++k) {
                printf("%s ", $k);
            }
            printf("%d\n", nMax);
        }
        if (rem != 0) {
            for (k = 2; k <= NF; ++k) {
                printf("%s ", $k);
            }
            printf("%d\n", rem);
        }
    }
}

END {
    if (Sread == "") {
        printf("%.12e", Ssum);
    }
}
