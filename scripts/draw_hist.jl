#!/usr/bin/env julia

using PyPlot;

maxerr = 0.2;
nbins = 100;

args = ARGS

num = tryparse(Float64, args[1])
if !isnull(num)
    maxerr = get(num)
    args = args[2:end]
end
num = tryparse(Int64, args[1])
if !isnull(num)
    nbins = get(num)
    args = args[2:end]
end

println("maxerr = ", maxerr)
println("nbins = ", nbins)

bins = linspace(-maxerr, maxerr, nbins);

fig = 0
for f in args
    println("Reading the file $(f).")
    fig += 1;
    p = readdlm(f);
    r = p[:, end-1];
    filter!(x -> (abs(x) <= maxerr), r);
    mu = mean(r);
    sigma = std(r);

    figure(fig);
    clf();
    plt[:hist](r, bins);
    xlabel("relative differences");
    ylabel("counts");
    xx = gca()[:get_xlim]()[2]*0.5;
    yy = gca()[:get_ylim]()[2]*0.9;
    ss = @sprintf("μ = %.4e\nσ = %.4e", mu, sigma)
    text(xx, yy, ss);
    epsf = replace(f, r"out.dat$", "hist.eps");
    savefig(epsf);
end

