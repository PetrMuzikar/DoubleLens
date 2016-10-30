#!/usr/bin/awk -f

BEGIN{
    obj = 0;
}

/a/{
    obj++;
    xC = $3;
    yC = $4;
    rLow = $5;
    rHigh = $6;
    printf("set object %d circle center %e,%e radius %e\n", obj, xC, yC, rLow);
    obj++;
    printf("set object %d circle center %e,%e radius %e\n", obj, xC, yC, rHigh);
}

/r/{
    obj++;
    xLow = $3;
    xHigh = $4;
    yLow = $5;
    yHigh = $6;
    printf("set object %d rectangle from %e,%e to %e,%e\n", obj, xLow, yLow, xHigh, yHigh);
}

