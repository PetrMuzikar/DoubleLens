#!/usr/bin/awk -f

function dist(x1, y1, x2, y2) {
    return sqrt((x1-x2)^2 + (y1-y2)^2);
}

function inInterval(a, x, b) {
    return ((b - x) * (x - a) > 0);
}

function rectRect(dom1, dom2) {
    #print "Checking rectangle + rectangle...";

    x1L = dom1[3];
    x1H = dom1[4];
    y1L = dom1[5];
    y1H = dom1[6];

    x2L = dom2[3];
    x2H = dom2[4];
    y2L = dom2[5];
    y2H = dom2[6];

    if ((x2L > x1H) || (x2H < x1L) || (y2L > y1H) || (y2H < y1L)) {
        return 0;
    }
    else {
        return 1;
    }
}

function rectAnn(dom1, dom2) {
    #print "Checking rectangle + annulus...";

    x1L = dom1[3];
    x1H = dom1[4];
    y1L = dom1[5];
    y1H = dom1[6];

    x2C = dom2[3];
    y2C = dom2[4];
    r2L = dom2[5];
    r2H = dom2[6];

    vert[0,0] = x1L;
    vert[0,1] = y1L;
    vert[1,0] = x1H;
    vert[1,1] = y1L;
    vert[2,0] = x1H;
    vert[2,1] = y1H;
    vert[3,0] = x1L;
    vert[3,1] = y1H;

    wh = 0;
    for (i=0; i<4; i++) {
        x = vert[i,0];
        y = vert[i,1];
        r = dist(x, x2C, y, y2C);
        if (r < r2L) {
            if (wh == 0) {
                wh = 1;
            } 
            else if (wh != 1) {
                return 1;
            }
        } 
        else if (r <= r2H) {
            return 1;
        }
        else {
            if (wh == 0) {
                wh = 2;
            } 
            else if (wh != 2) {
                return 1;
            }
            wh = 2;
        }
    }

    if (wh == 2) {
        if ((!inInterval(x1L, x2C, x1H)) && (!inInterval(y1L, y2C, y1H))) {
            return 0;
        }
        else if ((inInterval(x1L, x2C, x1H)) && (inInterval(y1L, y2C, y1H))) {
            return 1;
        }
        else if (inInterval(x2C, x1L, x2C+r2H) || 
                 inInterval(x2C-r2H, x1H, x2C) ||
                 inInterval(y2C, y1L, y2C+r2H) ||
                 inInterval(y2C-r2H, y1H, y2C)) {
            return 1;
        }
    }

    return 0;
}

function annAnn(dom1, dom2) {
    #print "Checking annulus + annulus...";

    x1C = dom1[3];
    y1C = dom1[4];
    r1L = dom1[5];
    r1H = dom1[6];

    x2C = dom2[3];
    y2C = dom2[4];
    r2L = dom2[5];
    r2H = dom2[6];

    r = dist(x1C, y1C, x2C, y2C);

    if (r > r1H + r2H) {
        return 0;
    }
    else if ((r2L > r1H) && (r < r2L - r1H)) {
        return 0;
    }
    else if ((r1L > r2H) && (r < r1L - r2H)) {
        return 0;
    }
    else {
        return 1;
    }
}

BEGIN {
    n = 0;
    inter = 0;
}

/#domain:/ {
    d[n++] = $0;
    if (n > 30) {
        exit 1;
    }
    next;
}

{next}

END {
    for (i=0; i<n; i++) {
        split(d[i], dom1);
        if (dom1[2] == "r") {
            for (j=i+1; j<n; j++) {
                split(d[j], dom2);
                if (dom2[2] == "r") {
                    inter = rectRect(dom1, dom2);
                }
                else if (dom2[2] == "a") {
                    inter = rectAnn(dom1, dom2);
                }
                else {
                    print "Bad domain!";
                    exit 1;
                }
                if (inter) {
                    print "Intersection:";
                    for (s = 1; s < length(dom1); s++) {
                        printf("%s ", dom1[s]);
                    }
                    print "";
                    for (s = 1; s < length(dom2); s++) {
                        printf("%s ", dom2[s]);
                    }
                    print "\n";
                }
            }
        }
        else if (dom1[2] == "a") {
            for (j=i+1; j<n; j++) {
                split(d[j], dom2);
                if (dom2[2] == "r") {
                    inter = rectAnn(dom2, dom1);
                }
                else if (dom2[2] == "a") {
                    inter = annAnn(dom1, dom2);
                }
                else {
                    print "Bad domain!";
                    exit 1;
                }
                if (inter) {
                    print "Intersection:";
                    for (s = 1; s < length(dom1); s++) {
                        printf("%s ", dom1[s]);
                    }
                    print "";
                    for (s = 1; s < length(dom2); s++) {
                        printf("%s ", dom2[s]);
                    }
                    print "\n";
                }
            }
        }
        else {
            print "Error!";
            exit 1;
        }
    }
}


