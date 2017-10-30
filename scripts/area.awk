#!/usr/bin/awk -f

function areaRectangle(x1, x2, y1, y2) {
    return (x2 - x1) * (y2 - y1);
}

function areaAnnulus(r1, r2) {
    pi = 4 * atan2(1, 1);
    return pi * (r2**2 - r1**2);
}

/#domain:/ {
    if ($2 == "r") {
        S = areaRectangle($3, $4, $5, $6);
    }
    else if ($2 == "a") {
        S = areaAnnulus($5, $6);
    }
    else {
        print "Bad domain!";
        exit 1;
    }   

    for (k = 1; k <= 6; k++) {
        printf("%s ", $k);
    }
    printf("%.10e\n", S);

    next;
}

{
    print;
}
