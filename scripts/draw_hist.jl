#!/usr/bin/env julia

using PyPlot;
using Statistics;
using DelimitedFiles;
using Printf;

function hist_bins(data, bins)
    under = 0;
    over = 0;
    counts = zeros(length(bins)-1);
    for x in data
        if x <= bins[1]
            under += 1;
            continue;
        elseif x > bins[end]
            over += 1;
            continue;
        end
        for k=2:length(bins)
            if x <= bins[k]
                counts[k-1] += 1;
                break
            end
        end
    end
    [counts, [under, over]]
end

function draw_hist(files...; maxi=0.1, nbins=100, n=4, suff="-hist.pdf", suff2="-hist2.pdf", xrange=[], yrange=[], fRow=nothing, fSel=nothing, xlab="relative differences", ylab="counts")

    dx = 2.0 * maxi / nbins;
    bins = range(-maxi, stop=maxi, step=dx)

    b = 10 .^ range(log10(maxi), stop=log10(maxi)-n+1, length=n);
    bins2 = vcat(-b, 0, reverse(b));
    bins2_loc = -(n+1):(n+1);

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
        p = readdlm(f, comments=true);
        sel = abs.(p[:, 10]) .< 1;
        if fRow == nothing
            r = p[sel, [3, 4, 10]];
        else
            r = mapslices(fRow, p[sel, :], 2);
        end
        if fSel != nothing
            sel = vec(mapslices(fSel, r, 2));
            r = r[sel, :];
        end 
        if length(xrange) == 2
            sel = xrange[1] .<= r[:, 1] .<= xrange[2];
            r = r[sel, :];
        end
        if length(yrange) == 2
            sel = yrange[1] .<= r[:, 2] .<= yrange[2];
            r = r[sel, :];
        end

        h = hist_bins(r[:,end], bins2);
        counts = vcat(h[2][1], h[1], h[2][2]);

        sel = abs.(r[:, end]) .<= maxi;
        r = r[sel, :];
        mu = mean(r[:,end]);
        sigma = std(r[:,end]);
    
        fig += 1;
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
        imgName = replace(f, r"-(out|diff).dat$" => suff);
        savefig(imgName);

        fig += 1;
        figure(fig);
        clf();
        plt[:bar](bins2_loc[1:end-1], counts, width=1);
        bins2_labels = vcat("< "*string(-maxi), bins2, "> "*string(maxi));
        gca()[:set_xlim](-(n+1), n+1);
        gca()[:set_xticks](bins2_loc);
        gca()[:set_xticklabels](bins2_labels);
        xlabel(xlab);
        ylabel(ylab);
        imgName2 = replace(f, r"-(out|diff).dat$" => suff2);
        savefig(imgName2);
    end
end

if length(ARGS) > 0
    draw_hist(ARGS...);
end
