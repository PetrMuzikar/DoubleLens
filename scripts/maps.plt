#!/usr/bin/env gnuplot

reset

#show loadpath

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

zmax = 0
logz = 1
cont = 0
zmin = 1
cbmin = zmin
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
#file_exists(x) = system("[ -f '". x ."' ] && echo '1' || echo '0'") + 0

#argc = "$#"
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

wx = xmax - xmin
wy = ymax - ymin
p = 0.05
xx1 = xmin + wx*p
xx2 = xmax - wx*p
yy1 = ymin + wy*p
yy2 = ymax - wy*p
lab(x, y) = (x > xx1 && x < xx2 && y > yy1 && y < yy2) ? stringcolumn($3) : ""

#cm = (1 - m1) * d
cm = 0
#print "cm = " . sprintf(cm)

dataname = basename . ".dat"
if (sub == 1 && div == 0) {
    imgname = basename . "-mag-sub" . suff
    img2name = basename . "-mag-lap-sub" . suff
    img3name = basename . "-analytic-sub" . suff
    imgcname = basename . "-mag-sub-cont" . suff
    img2cname = basename . "-mag-lap-sub-cont" . suff
    img3cname = basename . "-analytic-sub-cont" . suff
}
if (sub == 0 && div == 1) {
    imgname = basename . "-mag-div" . suff
    img2name = basename . "-mag-lap-div" . suff
    img3name = basename . "-analytic-div" . suff
    imgcname = basename . "-mag-div-cont" . suff
    img2cname = basename . "-mag-lap-div-cont" . suff
    img3cname = basename . "-analytic-div-cont" . suff
}
if (sub == 0 && div == 0) {
    imgname = basename . "-mag" . suff
    img2name = basename . "-mag-lap" . suff
    img3name = basename . "-analytic" . suff
    imgcname = basename . "-mag-cont" . suff
    img2cname = basename . "-mag-lap-cont" . suff
    img3cname = basename . "-analytic-cont" . suff
}
imgrelname = basename . "-rel" . suff
img2relname = basename . "-rel-lap" . suff
tabname = basename . "-shooting-cont.dat"
tab2name = basename . "-shooting-lap-cont.dat"
tab3name = basename . "-analytic-cont.dat"
addtit = sprintf("m_1 = %g, d = %s, {/Symbol b} = %s", m1, d, beta)

set xrange [xmin:xmax]
set yrange [ymin:ymax]
unset surface
#set cntrparam levels discrete 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096 
#set cntrparam levels discrete 2, 4, 8, 16, 32, 64, 128, 256 
if (logz == 1) {
    set logscale zcb 2
    set cntrparam levels discrete 2, 4, 8, 16, 32
} else {
    #set cntrparam levels 5
}

# Reading variables
if (plotConf == 1) {
    load plotConfFile
}

if (cont >= 1) {
    set contour base
    #set zrange [4:]
    set format xy "%25.14g"
    set format z "%20.12g"
    if (sub == 1 && div != 1) {
        set table tabname
        sp dataname u ($3-cm):4:($6-$9)
        unset table
        set table tab2name
        sp dataname u ($3-cm):4:($7-$9)
        unset table
        set table tab3name
        sp dataname u 3:4:($8-$9)
    } 
    if (sub != 1 && div == 1) {
        set table tabname
        sp dataname u ($3-cm):4:($6/$9)
        unset table
        set table tab2name
        sp dataname u ($3-cm):4:($7/$9)
        unset table
        set table tab3name
        sp dataname u 3:4:($8/$9)
    } 
    if (sub != 1 && div != 1) {
        set table tabname
        sp dataname u ($3-cm):4:6
        unset table
        set table tab2name
        sp dataname u ($3-cm):4:7
        unset table
        set table tab3name
        sp dataname u 3:4:8
    }
    unset table
    unset contour
}

reset
#set palette rgbformulae 33,13,10
load "moreland.pal"

set autoscale
set xrange [xmin:xmax]
set yrange [ymin:ymax]
if (zmax == 0) {
    set zrange [1:]
    set cbrange [1:]
} else {
    set zrange [1:zmax]
    set cbrange [1:zmax]
}
if (sub == 1 || div == 1) {
    set autoscale z
    set autoscale cb
}

if (logz == 1) {
    set logscale zcb 2
}
set size ratio -1
#set lmargin 0.1
#set bmargin 0.1
#set size 1.2

set macro
set pm3d map
set ticslevel 0
unset key
set xlabel "{/Symbol b}_x"
set ylabel "{/Symbol b}_y"

if (plotConf == 1) {
    load plotConfFile
}

# default terminal defined in the file .gnuplot
if (cont == 0 || cont == 2) {
    set output imgname
    #"" u 1:2:3:(lab($1, $2)) every ::1::1 w labels font "Arial,8" notitle
    if (sub != 1 && div == 1) {
        set title "Double lens: magnification for " . addtit . " / mag. of a single lens"
        sp dataname u ($3-cm):4:($6/$9) with pm3d
    } 
    if (sub == 1 && div != 1) {
        set title "Double lens: magnification for " . addtit . " - mag. of a single lens"
        sp dataname u ($3-cm):4:($6-$9) with pm3d
    } 
    if (sub != 1 && div != 1) {
        set title "Double lens: magnification for " . addtit 
        sp dataname u ($3-cm):4:6 with pm3d
    }
}

contourTics = system("which contourTics.sh")

#@DEFAULT
if (cont == 1 || cont == 2) {
    set output imgcname
    set cbtics scale 2,0.5
    load "< " . contourTics . " " . tabname 
    if (sub != 1 && div == 1) {
        set title "Double lens: magnification for " . addtit . " / mag. of
a single lens"
        sp dataname u ($3-cm):4:($6/$9) with pm3d, \
        tabname w d nosurface lt 1 lc rgb "black" notitle
    }
    if (sub == 1 && div != 1) {
        set title "Double lens: magnification for " . addtit . " - mag. of
a single lens"
        sp dataname u ($3-cm):4:($6-$9) with pm3d, \
        tabname w d nosurface lt 1 lc rgb "black" notitle
    }
    if (sub != 1 && div != 1) {
        set title "Double lens: magnification for " . addtit 
        sp dataname u ($3-cm):4:6 with pm3d, \
        tabname w d nosurface lt 1 lc rgb "black" notitle
    }
    set cbtics scale default autofreq
}

#print GPVAL_Z_MAX
#print GPVAL_CB_MAX
zmin = GPVAL_Z_MIN
zmax = GPVAL_Z_MAX
cbmin = GPVAL_CB_MIN
cbmax = GPVAL_CB_MAX
set zrange [zmin:zmax]
set cbrange [cbmin:cbmax]

if (cont == 0 || cont == 2) {
    set output img2name
    #sp dataname u ($3-cm):4:7 with pm3d
    if (sub != 1 && div == 1) {
        set title "Double lens: corrected magnification for " . addtit . " / mag. of a single lens"
        sp dataname u ($3-cm):4:($7/$9) with pm3d
    }
    if (sub == 1 && div != 1) {
        set title "Double lens: corrected magnification for " . addtit . " - mag. of a single lens"
        sp dataname u ($3-cm):4:($7-$9) with pm3d
    }
    if (sub != 1 && div != 1) {
        set title "Double lens: corrected magnification for " . addtit 
        sp dataname u ($3-cm):4:7 with pm3d
    }
}

if (cont == 1 || cont == 2) {
    set output img2cname
    set cbtics scale 2,0.5
    load "< " . contourTics . " " . tab2name 
    #sp dataname u ($3-cm):4:7 with pm3d, \
    #    tab2name w d nosurface lt 1 lc rgb "black" notitle
    if (sub != 1 && div == 1) {
        set title "Double lens: corrected magnification / mag. of a single
lens for " . addtit 
        sp dataname u ($3-cm):4:($7/$9) with pm3d, \
        tab2name w d nosurface lt 1 lc rgb "black" notitle
    }
    if (sub == 1 && div != 1) {
        set title "Double lens: corrected magnification for " . addtit 
        sp dataname u ($3-cm):4:($7-$9) with pm3d, \
        tab2name w d nosurface lt 1 lc rgb "black" notitle
    }
    if (sub != 1 && div != 1) {
        set title "Double lens: corrected magnification for " . addtit 
        sp dataname u ($3-cm):4:7 with pm3d notitle, \
        tab2name w d nosurface lt 1 lc rgb "black"
    }
    set cbtics scale default autofreq
}

set zrange [zmin:zmax]
set cbrange [cbmin:cbmax]

if (cont == 0 || cont == 2) {
    set output img3name
    #sp dataname u ($3-cm):4:8 with pm3d
    if (sub != 1 && div == 1) {
        set title "Analytic map for " . addtit . " / mag. of a single lens"
        sp dataname u ($3-cm):4:($8/$9) with pm3d
    }
    if (sub == 1 && div != 1) {
        set title "Analytic map for " . addtit . " - mag. of a single lens"
        sp dataname u ($3-cm):4:($8-$9) with pm3d
    }
    if (sub != 1 && div != 1) {
        set title "Analytic map for " . addtit
        sp dataname u ($3-cm):4:8 with pm3d
    }
}

if (cont == 1 || cont == 2) {
    set output img3cname
    set cbtics scale 2,0.5
    load "< " . contourTics . " " . tab3name
    #sp dataname u ($3-cm):4:8 with pm3d, \
    #    tab2name w d nosurface lt 1 lc rgb "black" notitle
    if (sub != 1 && div == 1) {
        set title "Analytic map for " . addtit . " / mag. of a single lens"
        sp dataname u ($3-cm):4:($8/$9) with pm3d, \
        tab3name w d nosurface lt 1 lc rgb "black" notitle
    }
    if (sub == 1 && div != 1) {
        set title "Analytic map for " . addtit . " - mag. of a single lens"
        sp dataname u ($3-cm):4:($8-$9) with pm3d, \
        tab3name w d nosurface lt 1 lc rgb "black" notitle
    }
    if (sub != 1 && div != 1) {
        set title "Analytic map for " . addtit
        sp dataname u ($3-cm):4:8 with pm3d, \
        tab3name w d nosurface lt 1 lc rgb "black" notitle
    }
}

unset out

unset logscale
set xrange [:]
set zrange [:]
set cbrange [:]
set cbtics scale default autofreq

if ((rel == 1) && (sub == 0) && (div == 0)) {
    set macro
    call "compare.plt" "@basename"
}

unset output
unset pm3d
unset log z
set surface
set key

@DEFAULT

