#!/usr/bin/awk -f

BEGIN {
    minRays = 1e9;
    maxRays = -1;
}

/^$|#zero/ {
    next;
}

/##/ {
    d = $13;
    mu1 = $14;
    beta = $15;
    nrE = $16;
    prec = $17;
    xMin = $18;
    xMax = $19;
    nx = $20;
    yMin = $21;
    yMax = $22;
    ny = $23;

    m1 = mu1 / (mu1 + (1 - mu1) * (1 - beta));

    next;
}

/# norm=/ {
    norm = $3;
    next;
}

/# total time/ {
    time = $7;
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
    printf("mu1 = %g\n", mu1);
    printf("m1 = %g\n", m1);
    printf("d = %g\n", d);
    printf("beta = %g\n", beta);
    printf("norm = %g\n", norm);
    printf("minRays = %g\n", minRays);
    printf("maxRays = %g\n", maxRays);
    printf("time = %g\n", time);
}
