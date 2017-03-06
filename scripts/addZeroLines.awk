#!/usr/bin/awk -f

BEGIN {
    nPix = 400
    lastPix = nPix - 1
}

/^$|^#/ {
    print;
    next;
}

($2 == lastPix) {
    print;
    printf("%5d%5d%20e%20e", $1, nPix, 0, 0);
    printf("%10d", 0); 
    printf("%15e%15e", 0, 0); 
    printf("%22e%22e", 0, 0); 
    printf("%15e%15e", 0, 0); 
    printf(" #zero\n");

    if ($1 == lastPix) {
        print "";
        for (i = 0; i <= nPix; i++) {
            printf("%5d%5d%20e%20e", nPix, i, 0, 0);
            printf("%10d", 0); 
            printf("%15e%15e", 0, 0); 
            printf("%22e%22e", 0, 0); 
            printf("%15e%15e", 0, 0); 
            printf(" #zero\n");
        }
    }
    next;
}

{ print }
