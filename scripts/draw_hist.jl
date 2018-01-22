#!/usr/bin/env julia

using PyPlot;

function draw_hist(files...; maxi=0.1, nbins=100, suff="-hist.eps", xrange=[], yrange=[], fRow=nothing, fSel=nothing, xlab="relative differences", ylab="counts")

    bins = linspace(-maxi, maxi, nbins);

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
        sel = p[:, 5] .>= 0;
        if fRow == nothing
            r = p[sel, [3, 4, 10]];
        else
            r = mapslices(fRow, p[sel, :], 2);
        end
        if fSel != nothing
            sel = vec(mapslices(fSel, r, 2));
            r = r[sel, :];
        end 
        sel = abs.(r[:, end]) .<= maxi;
        r = r[sel, :];
        if length(xrange) == 2
            sel = xrange[1] .<= r[:, 1] .<= xrange[2];
            r = r[sel, :];
        end
        if length(yrange) == 2
            sel = yrange[1] .<= r[:, 2] .<= yrange[2];
            r = r[sel, :];
        end
        mu = mean(r[:,end]);
        sigma = std(r[:,end]);
    
        figure(fig);
        clf();
        plt[:hist](r[:,end], bins);
        gca()[:set_xlim](-maxi, maxi);
        xlabel(xlab);
        ylabel(ylab);
        xl = gca()[:get_xlim]();
        yl = gca()[:get_ylim]();
        xx = xl[1] + (xl[2] - xl[1]) * 0.02;
        yy = yl[1] + (yl[2] - yl[1]) * 0.98;
        ss = @sprintf("mu = %.3e\nsigma = %.3e\n%s", mu, sigma, desc);
        text(xx, yy, ss, verticalalignment="top");
        epsf = replace(f, r"-(out|diff).dat$", suff);
        savefig(epsf);
    end

end

if length(ARGS) > 0
    draw_hist(ARGS...);
end
