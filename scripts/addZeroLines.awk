#!/usr/bin/awk -f

BEGIN {
    nPix = 400
    lastPix = nPix - 1
    low = 1;
    xLow = 0;
    yLow = 0;
    ifdx = 1;
    dx = 0;
    ifdy = 1;
    dy = 0;
}

/^$|^#/ {
    print;
    next;
}

($2 == lastPix) {
    print;
    x = $3;
    y = $4 + dy;
    printf("%5d%5d%20e%20e", $1, nPix, x, y);
    printf("%10d", 0); 
    printf("%15e%15e", 0, 0); 
    printf("%22e%22e", 0, 0); 
    printf("%15e%15e", 0, 0); 
    printf(" #zero\n");

    if ($1 == lastPix) {
        print "";
        x = $3 + dx;
        for (i = 0; i <= nPix; i++) {
            y = yLow + i * dy;
            printf("%5d%5d%20e%20e", nPix, i, x, y);
            printf("%10d", 0); 
            printf("%15e%15e", 0, 0); 
            printf("%22e%22e", 0, 0); 
            printf("%15e%15e", 0, 0); 
            printf(" #zero\n");
        }
    }
    lastX = $3;
    laxtY = $4;
    next;
}

{ 
    if (low) {
        xLow = $3;
        yLow = $4;
        low = 0;
    }

    if (ifdx) {
        if ($3 != lastX) {
            dx = $3 - lastX;
            ifdx = 0;
        }
    }

    if (ifdy) {
        if ($4 != lastY) {
            dy = $4 - lastY;
            ifdy = 0;
        }
    }

    lastX = $3;
    laxtY = $4;
    print;
}
