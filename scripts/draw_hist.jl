#!/usr/bin/env julia

using PyPlot;

isdefined(:maxerr) || (maxerr = 0.1);
isdefined(:nbins) || (nbins = 100);
isdefined(:suff) || (suff = "hist.eps");

println("maxerr = ", maxerr);
println("nbins = ", nbins);
println("suff = ", suff);

desc = "";
if isdefined(:xrange)
    s = "xrange = $(xrange)\n";
    print(s);
    desc *= s;
end

if isdefined(:yrange)
    s = "yrange = $(yrange)\n";
    print(s);
    desc *= s;
end

bins = linspace(-maxerr, maxerr, nbins);

fig = 0
for f in ARGS
    println("Reading the file $(f).")
    fig += 1;
    p = readdlm(f);
    r = p[:, [3, 4, 10]];
    sel = r[:, 3] .<= maxerr
    r = r[sel, :];
    if isdefined(:xrange)
        sel = xrange[1] .<= r[:, 1] .<= xrange[2];
        r = r[sel, :];
    end
    if isdefined(:yrange)
        sel = yrange[1] .<= r[:, 2] .<= yrange[2];
        r = r[sel, :];
    end
    mu = mean(r[:,3]);
    sigma = std(r[:,3]);

    figure(fig);
    clf();
    plt[:hist](r[:,3], bins);
    gca()[:set_xlim](-maxerr, maxerr);
    xlabel("relative differences");
    ylabel("counts");
    xx = gca()[:get_xlim]()[2]*0.5;
    yy = gca()[:get_ylim]()[2]*0.9;
    ss = @sprintf("μ = %.4e\nσ = %.4e\n%s", mu, sigma, desc);
    text(xx, yy, ss);
    epsf = replace(f, r"out.dat$", suff);
    savefig(epsf);
end

