#!/usr/bin/env gnuplot

reset

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

mu1 = "0.5"
m1 = "0.5"
d = "1.9"
beta = "0.01"

f(x,y) = x / (x + (1 - y) * (1 - x))
#file_exists(x) = system("[ -f '". x ."' ] && echo '1' || echo '0'") + 0

argc = "$#"
if (argc == 0) { 
    basename = "vyst"
}

if (argc >= 1) { 
    input = "$0.dat"
    basename = "$0"
}

if (argc >= 2) {
    mu1 = "$1"
}

if (argc >= 3) {
    d = "$2"
}

if (argc >= 4) {
    beta = "$3"
}

if (argc >= 5) { 
    xmin = "$4"
}

if (argc >= 6) { 
    xmax = "$5"
}

if (argc >= 7) { 
    ymin = "$6"
}

if (argc >= 8) { 
    ymax = "$7"
}

m1 = f(mu1, beta)

print "mu1 = " . mu1
print "m1 = " . sprintf("%f",m1)
print "d = " . d
print "beta = " . beta
#print "settings = " . settings
#print "zmax = " . zmax
#print "cont = " . cont
#print "logz = " . logz

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
    epsname = basename . "-mag-sub.eps"
    eps2name = basename . "-mag-lap-sub.eps"
    eps3name = basename . "-analytic-sub.eps"
    epscname = basename . "-mag-sub-cont.eps"
    eps2cname = basename . "-mag-lap-sub-cont.eps"
    eps3cname = basename . "-analytic-sub-cont.eps"
}
if (sub == 0 && div == 1) {
    epsname = basename . "-mag-div.eps"
    eps2name = basename . "-mag-lap-div.eps"
    eps3name = basename . "-analytic-div.eps"
    epscname = basename . "-mag-div-cont.eps"
    eps2cname = basename . "-mag-lap-div-cont.eps"
    eps3cname = basename . "-analytic-div-cont.eps"
}
if (sub == 0 && div == 0) {
    epsname = basename . "-mag.eps"
    eps2name = basename . "-mag-lap.eps"
    eps3name = basename . "-analytic.eps"
    epscname = basename . "-mag-cont.eps"
    eps2cname = basename . "-mag-lap-cont.eps"
    eps3cname = basename . "-analytic-cont.eps"
}
#eps3name = basename . "-rel.eps"
#eps4name = basename . "-rel-lap.eps"
outname = basename . "-rel.eps"
outname2 = basename . "-rel-lap.eps"
tabname = basename . "-shooting-cont.dat"
tab2name = basename . "-shooting-lap-cont.dat"
tab3name = basename . "-analytic-cont.dat"
#addtit = sprintf("m_1 = %g, d = %g, {/Symbol b} = %g", m1, d, beta)
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
        sp dataname u ($$3-cm):4:($$6-$$9)
        unset table
        set table tab2name
        sp dataname u ($$3-cm):4:($$7-$$9)
        unset table
        set table tab3name
        sp dataname u 3:4:($$8-$$9)
    } 
    if (sub != 1 && div == 1) {
        set table tabname
        sp dataname u ($$3-cm):4:($$6/$$9)
        unset table
        set table tab2name
        sp dataname u ($$3-cm):4:($$7/$$9)
        unset table
        set table tab3name
        sp dataname u 3:4:($$8/$$9)
    } 
    if (sub != 1 && div != 1) {
        set table tabname
        sp dataname u ($$3-cm):4:6
        unset table
        set table tab2name
        sp dataname u ($$3-cm):4:7
        unset table
        set table tab3name
        sp dataname u 3:4:8
    }
    unset table
    unset contour
}

reset
set palette rgbformulae 33,13,10
set autoscale
set xrange [xmin:xmax]
set yrange [ymin:ymax]
#if (!(xt eq "auto")) {
#    set xtics xt
#}
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
#set zrange [1:1.0000001]
#set cbrange [1:1.0000001]

#set logscale zcb 1.000001
#load "pl.plt"

if (logz == 1) {
    set logscale zcb 2
}
#set format y "%.7f"
#if (logz == 0) {
#    #set format cb "%.9f"
#    if (dec == 0) {
#        set format "%g"
#    } else {
#        form = sprintf("%%.%df", dec)
#        print form
#        set format form
#    }
#}
set size ratio -1
set lmargin 0.1
set bmargin 0.1
set size 1.2, 1.2

set macro
set pm3d map
#set logscale zcb 2
set ticslevel 0
#unset surface
unset key
#set view map
set xlabel "{/Symbol b}_x"
set ylabel "{/Symbol b}_y"

if (plotConf == 1) {
    load plotConfFile
}

set term wxt
if (cont == 0 || cont == 2) {
    #"" u 1:2:3:(lab($$1, $$2)) every ::1::1 w labels font "Arial,8" notitle
    if (sub != 1 && div == 1) {
        set title "Double lens: magnification for " . addtit . " / mag. of a single lens"
        sp dataname u ($$3-cm):4:($$6/$$9) with pm3d
    } 
    if (sub == 1 && div != 1) {
        set title "Double lens: magnification for " . addtit . " - mag. of a single lens"
        sp dataname u ($$3-cm):4:($$6-$$9) with pm3d
    } 
    if (sub != 1 && div != 1) {
        set title "Double lens: magnification for " . addtit 
        sp dataname u ($$3-cm):4:6 with pm3d
    }
    set term postscript eps color enhanced
    set output epsname
    replot
}

contourTics = system("which contourTics.sh")

set term wxt
if (cont == 1 || cont == 2) {
    set cbtics scale 2,0.5
    load "< " . contourTics . " " . tabname 
    if (sub != 1 && div == 1) {
        set title "Double lens: magnification for " . addtit . " / mag. of
a single lens"
        sp dataname u ($$3-cm):4:($$6/$$9) with pm3d, \
        tabname w d nosurface lt 1 lc rgb "black" notitle
    }
    if (sub == 1 && div != 1) {
        set title "Double lens: magnification for " . addtit . " - mag. of
a single lens"
        sp dataname u ($$3-cm):4:($$6-$$9) with pm3d, \
        tabname w d nosurface lt 1 lc rgb "black" notitle
    }
    if (sub != 1 && div != 1) {
        set title "Double lens: magnification for " . addtit 
        sp dataname u ($$3-cm):4:6 with pm3d, \
        tabname w d nosurface lt 1 lc rgb "black" notitle
    }
    set term postscript eps color enhanced
    set output epscname
    replot
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

# set term wxt
# set title "Double lens: corrected mangnification for " . addtit
# if (cont == 1) {
# sp dataname u ($$3-cm):4:7 with pm3d, \
#    tab2name w d nosurface lt 1 lc rgb "black" notitle
# } else {
# sp dataname u ($$3-cm):4:7 with pm3d
# }
# set term postscript eps color enhanced
# set output eps2name
# replot

set term wxt
    if (cont == 0 || cont == 2) {
    #sp dataname u ($$3-cm):4:7 with pm3d
    if (sub != 1 && div == 1) {
        set title "Double lens: corrected magnification for " . addtit . " / mag. of a single lens"
        sp dataname u ($$3-cm):4:($$7/$$9) with pm3d
    }
    if (sub == 1 && div != 1) {
        set title "Double lens: corrected magnification for " . addtit . " - mag. of a single lens"
        sp dataname u ($$3-cm):4:($$7-$$9) with pm3d
    }
    if (sub != 1 && div != 1) {
        set title "Double lens: corrected magnification for " . addtit 
        sp dataname u ($$3-cm):4:7 with pm3d
    }
    set term postscript eps color enhanced
    set output eps2name
    replot
}

set term wxt
if (cont == 1 || cont == 2) {
    set cbtics scale 2,0.5
    load "< " . contourTics . " " . tab2name 
    #sp dataname u ($$3-cm):4:7 with pm3d, \
    #    tab2name w d nosurface lt 1 lc rgb "black" notitle
    if (sub != 1 && div == 1) {
        set title "Double lens: corrected magnification / mag. of a single
lens for " . addtit 
        sp dataname u ($$3-cm):4:($$7/$$9) with pm3d, \
        tab2name w d nosurface lt 1 lc rgb "black" notitle
    }
    if (sub == 1 && div != 1) {
        set title "Double lens: corrected magnification for " . addtit 
        sp dataname u ($$3-cm):4:($$7-$$9) with pm3d, \
        tab2name w d nosurface lt 1 lc rgb "black" notitle
    }
    if (sub != 1 && div != 1) {
        set title "Double lens: corrected magnification for " . addtit 
        sp dataname u ($$3-cm):4:7 with pm3d notitle, \
        tab2name w d nosurface lt 1 lc rgb "black"
    }
    set term postscript eps color enhanced
    set output eps2cname
    replot
    set cbtics scale default autofreq
}

set zrange [zmin:zmax]
set cbrange [cbmin:cbmax]

# set term wxt
# #set zrange [1:1.002]
# #set cbrange [1:1.002]
# set title "Analytic map, " . addtit 
# if (cont == 1) {
# sp dataname u 3:4:8 with pm3d, \
#    tab3name w d nosurface lt 1 lc rgb "black" notitle
# } else {
# sp dataname u 3:4:8 with pm3d
# }

set term wxt
if (cont == 0 || cont == 2) {
    #sp dataname u ($$3-cm):4:8 with pm3d
    if (sub != 1 && div == 1) {
        set title "Analytic map for " . addtit . " / mag. of a single lens"
        sp dataname u ($$3-cm):4:($$8/$$9) with pm3d
    }
    if (sub == 1 && div != 1) {
        set title "Analytic map for " . addtit . " - mag. of a single lens"
        sp dataname u ($$3-cm):4:($$8-$$9) with pm3d
    }
    if (sub != 1 && div != 1) {
        set title "Analytic map for " . addtit
        sp dataname u ($$3-cm):4:8 with pm3d
    }
    set term postscript eps color enhanced
    set output eps3name
    replot
}

set term wxt
if (cont == 1 || cont == 2) {
    set cbtics scale 2,0.5
    load "< " . contourTics . " " . tab3name
    #sp dataname u ($$3-cm):4:8 with pm3d, \
    #    tab2name w d nosurface lt 1 lc rgb "black" notitle
    if (sub != 1 && div == 1) {
        set title "Analytic map for " . addtit . " / mag. of a single lens"
        sp dataname u ($$3-cm):4:($$8/$$9) with pm3d, \
        tab3name w d nosurface lt 1 lc rgb "black" notitle
    }
    if (sub == 1 && div != 1) {
        set title "Analytic map for " . addtit . " - mag. of a single lens"
        sp dataname u ($$3-cm):4:($$8-$$9) with pm3d, \
        tab3name w d nosurface lt 1 lc rgb "black" notitle
    }
    if (sub != 1 && div != 1) {
        set title "Analytic map for " . addtit
        sp dataname u ($$3-cm):4:8 with pm3d, \
        tab3name w d nosurface lt 1 lc rgb "black" notitle
    }
    set term postscript eps color enhanced
    set output eps3cname
    replot
    #unset key
}

set term wxt
unset out
replot

unset logscale
set xrange [:]
set zrange [:]
set cbrange [:]
set cbtics scale default autofreq

if ((rel == 1) && (sub == 0) && (div == 0)) {
    set macro
    comp = system("which compare.plt")
    call comp "@basename"
}

unset output
unset pm3d
unset log z
set surface
set key
set term wxt

