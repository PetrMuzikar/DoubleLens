#!/usr/bin/awk -f

# For processing the file *-domains.dat.

BEGIN{
    obj = 0;
}

/a/{
    obj++;
    xC = $2;
    yC = $3;
    rLow = $4;
    rHigh = $5;
    printf("set object %d circle center %e,%e radius %e\n", obj, xC, yC, rLow);
    obj++;
    printf("set object %d circle center %e,%e radius %e\n", obj, xC, yC, rHigh);
}

/r/{
    obj++;
    xLow = $2;
    xHigh = $3;
    yLow = $4;
    yHigh = $5;
    printf("set object %d rectangle from %e,%e to %e,%e\n", obj, xLow, yLow, xHigh, yHigh);
}

