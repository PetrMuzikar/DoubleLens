#!/usr/bin/env gnuplot

reset

maps = 1
nolap = 2
lap = 3

script = maps

print "Processing " . ARG0 . "..."

ft = "PDF"

if (ft eq "EPS") {
    @EPS
    suff = ".eps"
}
if (ft eq "PDF") {
    @PDF
    suff = ".pdf"
}

#epsi = 1e-8
magmin = 1
magmax = 8
submin = -0.1
submax = 0.1
divmin = 0.9
divmax = 1.1

logz = 1

#cont = 0

mag = 1
sub = 0
div = 0
rel = 1

# Reading variables
if (plotConf == 1) {
    load plotConfFile
}

mu1 = 0.5
m1 = 0.5
d = 1.9
beta = 0.01

f(x,y) = x / (x + (1 - y) * (1 - x))

if (ARGC == 0) { 
    basename = "vyst"
}

if (ARGC >= 1) { 
    input = ARG1 . ".dat"
    basename = ARG1
}

if (ARGC >= 2) {
    mu1 = ARG2
}

if (ARGC >= 3) {
    d = ARG3
}

if (ARGC >= 4) {
    beta = ARG4
}

if (ARGC >= 5) { 
    xmin = ARG5
}

if (ARGC >= 6) { 
    xmax = ARG6
}

if (ARGC >= 7) { 
    ymin = ARG7
}

if (ARGC >= 8) { 
    ymax = ARG8
}

m1 = f(mu1, beta)

print "mu1 = " . mu1
print "m1 = " . sprintf("%f",m1)
print "d = " . d
print "beta = " . beta

#wx = xmax - xmin
#wy = ymax - ymin
#p = 0.05
#xx1 = xmin + wx*p
#xx2 = xmax - wx*p
#yy1 = ymin + wy*p
#yy2 = ymax - wy*p
#lab(x, y) = (x > xx1 && x < xx2 && y > yy1 && y < yy2) ? stringcolumn($3) : ""

dataname = basename . ".dat"
if (mag == 1) {
    imgname = basename . "-mag" . suff
    img2name = basename . "-mag-lap" . suff
    img3name = basename . "-analytic" . suff
    imgcname = basename . "-mag-cont" . suff
    #img2cname = basename . "-mag-lap-cont" . suff
    #img3cname = basename . "-analytic-cont" . suff
}
if (sub == 1) {
    subimgname = basename . "-mag-sub" . suff
    subimg2name = basename . "-mag-lap-sub" . suff
    subimg3name = basename . "-analytic-sub" . suff
    subimgcname = basename . "-mag-sub-cont" . suff
    #subimg2cname = basename . "-mag-lap-sub-cont" . suff
    #subimg3cname = basename . "-analytic-sub-cont" . suff
}
if (div == 1) {
    divimgname = basename . "-mag-div" . suff
    divimg2name = basename . "-mag-lap-div" . suff
    divimg3name = basename . "-analytic-div" . suff
    divimgcname = basename . "-mag-div-cont" . suff
    #divimg2cname = basename . "-mag-lap-div-cont" . suff
    #divimg3cname = basename . "-analytic-div-cont" . suff
}
imgrelname = basename . "-rel" . suff
img2relname = basename . "-rel-lap" . suff
tabname = basename . "-shooting-cont.dat"
#tab2name = basename . "-shooting-lap-cont.dat"
#tab3name = basename . "-analytic-cont.dat"
addtit = sprintf("m_1 = %g, d = %s, {/Symbol b} = %s", m1, d, beta)

if (system("awk '/##/{print $8; exit} {next}' " . dataname) <= 1e8) {
    magmin = 0.95
} else {
    magmin = 0.99
}

stats "<awk '/#/{next} {print}' " . dataname using 8
magmax = STATS_max

set xrange [xmin:xmax]
set yrange [ymin:ymax]
unset surface

#if (logz == 1) {
#    set cntrparam levels discrete 2, 4, 8, 16, 32
#} else {
#    set cntrparam levels 5
#}

# Reading variables
if (plotConf == 1) {
    load plotConfFile
}

#if (cont >= 1) {
#    set contour base
#    set format xy "%25.14g"
#    set format z "%20.12g"
#    if (sub == 0 && div == 0) {
#        set table tabname
#        sp dataname u ($3-cm):4:6
#        unset table
#        set table tab2name
#        sp dataname u ($3-cm):4:7
#        unset table
#        set table tab3name
#        sp dataname u 3:4:8
#    }
#    if (sub == 1 && div == 0) {
#        set table tabname
#        sp dataname u ($3-cm):4:($6-$9)
#        unset table
#        set table tab2name
#        sp dataname u ($3-cm):4:($7-$9)
#        unset table
#        set table tab3name
#        sp dataname u 3:4:($8-$9)
#    } 
#    if (sub == 0 && div == 1) {
#        set table tabname
#        sp dataname u ($3-cm):4:($6/$9)
#        unset table
#        set table tab2name
#        sp dataname u ($3-cm):4:($7/$9)
#        unset table
#        set table tab3name
#        sp dataname u 3:4:($8/$9)
#    } 
#    unset table
#    unset contour
#}

reset

if (logz == 1) {
#    magstep = 2
    set log zcb 2
} else {
#    magstep = 1
}

set autoscale xy
set xrange [xmin:xmax]
set yrange [ymin:ymax]
if (sub == 1 || div == 1) {
    set autoscale z
    set autoscale cb
}

set size ratio -1

#set macro
#set pm3d map
#set pm3d corners2color c1
#set ticslevel 0
#unset key
#set xlabel "{/Symbol b}_x"
#set ylabel "{/Symbol b}_y"

if (plotConf == 1) {
    load plotConfFile
}

#cbmin = magmin * (1 - epsi)
#cbmax = magmax * (1 + epsi)
#set cbrange [cbmin:cbmax]
#
#wid = cbmax - cbmin
#z1 = cbmin + wid/4
#z2 = cbmin + wid/2
#z3 = cbmin + 3*wid/4
#
#set palette defined ( \
#    cbmin "yellow", \
#    magmin "yellow", \
#    magmin 0.226 0.3 0.75, \
#    z1 0.55 0.7 1, \
#    z2 0.87 0.87 0.87, \
#    z3 0.95 0.6 0.47, \
#    magmax 0.7 0 0.15, \
#    magmax "green", \
#    cbmax "green" \
#) 
#
#set cbtics autofreq

apo(x) = "\"" . x . "\""

if (mag == 1) {
    set output imgname
    tit = "Double lens: magnification for " . addtit 
    draw = "sp " . apo(dataname) . " u 3:4:6"
    call "drawMag.plt" magmin magmax draw tit

    set output img2name
    tit = "Double lens: corrected magnification for " . addtit 
    draw = "sp " . apo(dataname) . " u 3:4:7"
    call "drawMag.plt" magmin magmax draw tit

    set output img3name
    tit = "Analytic map for " . addtit
    draw = "sp " . apo(dataname) . " u 3:4:8"
    call "drawMag.plt" magmin magmax draw tit
}

if (sub == 1) {
    set output subimgname
    tit = "Double lens: magnification for " . addtit . " - mag. of a single lens"
    draw = "sp " . apo(dataname) . " u 3:4:($6-$9)"
    call "drawMag.plt" submin submax draw tit

    set output subimg2name
    tit = "Double lens: corrected magnification for " . addtit . " - mag. of a single lens"
    draw = "sp " . apo(dataname) . " u 3:4:($7-$9)"
    call "drawMag.plt" submin submax draw tit

    set output subimg3name
    tit = "Analytic map for " . addtit . " - mag. of a single lens"
    draw = "sp " . apo(dataname) . " u 3:4:($8-$9)"
    call "drawMag.plt" submin submax draw tit
} 

if (div == 1) {
    set output divimgname
    tit = "Double lens: magnification for " . addtit . " / mag. of a single lens"
    draw = "sp " . apo(dataname) . "u 3:4:($6/$9)"
    call "drawMag.plt" divmin divmax draw tit

    set output divimg2name
    tit = "Double lens: corrected magnification for " . addtit . " / mag. of a single lens"
    draw = "sp " . apo(dataname) . " u 3:4:($7/$9)"
    call "drawMag.plt" divmin divmax draw tit

    set output divimg3name
    tit = "Analytic map for " . addtit . " / mag. of a single lens"
    draw = "sp " . apo(dataname) . " u 3:4:($8/$9)"
    call "drawMag.plt" divmin divmax draw tit
}

#if (cont == 0 || cont == 2) {
#    set output imgname
#    if (sub != 1 && div == 1) {
#        set title "Double lens: magnification for " . addtit . " / mag. of a single lens"
#        sp dataname u ($3-cm):4:($6/$9) with pm3d
#    } 
#    if (sub == 1 && div != 1) {
#        set title "Double lens: magnification for " . addtit . " - mag. of a single lens"
#        sp dataname u ($3-cm):4:($6-$9) with pm3d
#    } 
#    if (sub != 1 && div != 1) {
#        set title "Double lens: magnification for " . addtit 
#        sp dataname u ($3-cm):4:6 with pm3d
#    }
#}

#contourTics = system("which contourTics.sh")
#
##@DEFAULT
#if (cont == 1 || cont == 2) {
#    set output imgcname
#    load "< " . contourTics . " " . tabname 
#    if (sub != 1 && div == 1) {
#        set title "Double lens: magnification for " . addtit . " / mag. of
#a single lens"
#        sp dataname u ($3-cm):4:($6/$9) with pm3d, \
#        tabname w d nosurface lt 1 lc rgb "black" notitle
#    }
#    if (sub == 1 && div != 1) {
#        set title "Double lens: magnification for " . addtit . " - mag. of
#a single lens"
#        sp dataname u ($3-cm):4:($6-$9) with pm3d, \
#        tabname w d nosurface lt 1 lc rgb "black" notitle
#    }
#    if (sub != 1 && div != 1) {
#        set title "Double lens: magnification for " . addtit 
#        sp dataname u ($3-cm):4:6 with pm3d, \
#        tabname w d nosurface lt 1 lc rgb "black" notitle
#    }
#    #set cbtics scale default autofreq
#}

#print GPVAL_CB_MIN
#print GPVAL_CB_MAX

#if (cont == 0 || cont == 2) {
#    set output img2name
#    #sp dataname u ($3-cm):4:7 with pm3d
#    if (sub != 1 && div == 1) {
#        set title "Double lens: corrected magnification for " . addtit . " / mag. of a single lens"
#        sp dataname u ($3-cm):4:($7/$9) with pm3d
#    }
#    if (sub == 1 && div != 1) {
#        set title "Double lens: corrected magnification for " . addtit . " - mag. of a single lens"
#        sp dataname u ($3-cm):4:($7-$9) with pm3d
#    }
#    if (sub != 1 && div != 1) {
#        set title "Double lens: corrected magnification for " . addtit 
#        sp dataname u ($3-cm):4:7 with pm3d
#    }
#}
#
#if (cont == 1 || cont == 2) {
#    set output img2cname
#    load "< " . contourTics . " " . tab2name 
#    #sp dataname u ($3-cm):4:7 with pm3d, \
#    #    tab2name w d nosurface lt 1 lc rgb "black" notitle
#    if (sub != 1 && div == 1) {
#        set title "Double lens: corrected magnification / mag. of a single
#lens for " . addtit 
#        sp dataname u ($3-cm):4:($7/$9) with pm3d, \
#        tab2name w d nosurface lt 1 lc rgb "black" notitle
#    }
#    if (sub == 1 && div != 1) {
#        set title "Double lens: corrected magnification for " . addtit 
#        sp dataname u ($3-cm):4:($7-$9) with pm3d, \
#        tab2name w d nosurface lt 1 lc rgb "black" notitle
#    }
#    if (sub != 1 && div != 1) {
#        set title "Double lens: corrected magnification for " . addtit 
#        sp dataname u ($3-cm):4:7 with pm3d notitle, \
#        tab2name w d nosurface lt 1 lc rgb "black"
#    }
#    #set cbtics scale default autofreq
#}

#set zrange [zmin:zmax]
#set cbrange [cbmin:cbmax]

#if (cont == 0 || cont == 2) {
#    set output img3name
#    #sp dataname u ($3-cm):4:8 with pm3d
#    if (sub != 1 && div == 1) {
#        set title "Analytic map for " . addtit . " / mag. of a single lens"
#        sp dataname u ($3-cm):4:($8/$9) with pm3d
#    }
#    if (sub == 1 && div != 1) {
#        set title "Analytic map for " . addtit . " - mag. of a single lens"
#        sp dataname u ($3-cm):4:($8-$9) with pm3d
#    }
#    if (sub != 1 && div != 1) {
#        set title "Analytic map for " . addtit
#        sp dataname u ($3-cm):4:8 with pm3d
#    }
#}

#if (cont == 1 || cont == 2) {
#    set output img3cname
#    #set cbtics scale 2,0.5
#    load "< " . contourTics . " " . tab3name
#    #sp dataname u ($3-cm):4:8 with pm3d, \
#    #    tab2name w d nosurface lt 1 lc rgb "black" notitle
#    if (sub != 1 && div == 1) {
#        set title "Analytic map for " . addtit . " / mag. of a single lens"
#        sp dataname u ($3-cm):4:($8/$9) with pm3d, \
#        tab3name w d nosurface lt 1 lc rgb "black" notitle
#    }
#    if (sub == 1 && div != 1) {
#        set title "Analytic map for " . addtit . " - mag. of a single lens"
#        sp dataname u ($3-cm):4:($8-$9) with pm3d, \
#        tab3name w d nosurface lt 1 lc rgb "black" notitle
#    }
#    if (sub != 1 && div != 1) {
#        set title "Analytic map for " . addtit
#        sp dataname u ($3-cm):4:8 with pm3d, \
#        tab3name w d nosurface lt 1 lc rgb "black" notitle
#    }
#}

unset out

unset logscale
set xrange [:]
set zrange [:]
set cbrange [:]
set cbtics scale default autofreq

if (rel == 1) {
    set macro
    call "compare.plt" "@basename"
}

unset output
unset pm3d
unset log z
set surface
set key

@DEFAULT

