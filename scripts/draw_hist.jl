#!/usr/bin/env julia

using PyPlot;

function draw_hist(files...; maxerr=0.1, nbins=100, suff="hist.eps", xrange=[], yrange=[])
    bins = linspace(-maxerr, maxerr, nbins);

    desc = "";
    if length(xrange) == 2
        s = "xrange = $(xrange)\n";
        print(s);
        desc *= s;
    end
    
    if length(yrange) == 2
        s = "yrange = $(yrange)\n";
        print(s);
        desc *= s;
    end

    fig = 0
    for f in files
        println("Reading the file $(f).")
        fig += 1;
        p = readdlm(f);
        r = p[:, [3, 4, 10]];
        sel = r[:, 3] .<= maxerr
        r = r[sel, :];
        if length(xrange) == 2
            sel = xrange[1] .<= r[:, 1] .<= xrange[2];
            r = r[sel, :];
        end
        if length(yrange) == 2
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
        xl = gca()[:get_xlim]();
        yl = gca()[:get_ylim]();
        xx = xl[1] + (xl[2] - xl[1]) * 0.02;
        yy = yl[1] + (yl[2] - yl[1]) * 0.98;
        ss = @sprintf("mu = %.3e\nsigma = %.3e\n%s", mu, sigma, desc);
        text(xx, yy, ss, verticalalignment="top");
        epsf = replace(f, r"out.dat$", suff);
        savefig(epsf);
    end

end

if length(ARGS) > 0
    draw_hist(ARGS...);
end
