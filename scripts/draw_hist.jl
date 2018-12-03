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

function filter_data(f; xrange=[], yrange=[], fRow=nothing, fSel=nothing)
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

    println("Reading the file $(f).")
    p = readdlm(f, comments=true);
    sel = 1e-10 .< abs.(p[:, 10]) .< 1;
    p = p[sel, :];
    #if fRow == nothing
    #    r = p[sel, [3, 4, 10]];
    #else
    #    r = mapslices(fRow, p[sel, :], 2);
    #end
    #if fSel != nothing
    #    sel = vec(mapslices(fSel, r, 2));
    #    r = r[sel, :];
    #end 
    if length(xrange) == 2
        sel = xrange[1] .<= p[:, 3] .<= xrange[2];
        p = p[sel, :];
    end
    if length(yrange) == 2
        sel = yrange[1] .<= p[:, 4] .<= yrange[2];
        p = p[sel, :];
    end

    [p, desc]
end

function draw_h1(p, imgName; maxi=0.1, nbins=100, xlab="relative differences", ylab="counts", desc="")

    dx = 2.0 * maxi / nbins;
    bins = range(-maxi, stop=maxi, step=dx)

    c = 10;
    sel = abs.(p[:,c]) .<= maxi;
    p = p[sel,:];
    mu = mean(p[:,c]);
    sigma = std(p[:,c]);

    fig = 1;
    figure(fig);
    clf();
    plt[:hist](p[:,c], bins);
    gca()[:set_xlim](-maxi, maxi);
    xlabel(xlab);
    ylabel(ylab);
    xl = gca()[:get_xlim]();
    yl = gca()[:get_ylim]();
    xx = xl[1] + (xl[2] - xl[1]) * 0.02;
    yy = yl[1] + (yl[2] - yl[1]) * 0.98;
    ss = @sprintf("mu = %.3e\nsigma = %.3e\n%s", mu, sigma, desc);
    text(xx, yy, ss, verticalalignment="top");
    savefig(imgName);
end

function draw_h2(p, imgName2; maxi=0.1, n=4, xlab="relative differences", ylab="counts", desc="")

    b = 10 .^ range(log10(maxi), stop=log10(maxi)-n+1, length=n);
    bins2 = vcat(-b, 0, reverse(b));
    bins2_loc = -(n+1):(n+1);

    c = 10;
    h = hist_bins(p[:,c], bins2);
    counts = vcat(h[2][1], h[1], h[2][2]);

    fig = 2;
    figure(fig);
    clf();
    plt[:bar](bins2_loc[1:end-1], counts, width=1, align="edge");
    bins2_labels = vcat("< "*string(-maxi), bins2, "> "*string(maxi));
    gca()[:set_xlim](-(n+1), n+1);
    gca()[:set_xticks](bins2_loc);
    gca()[:set_xticklabels](bins2_labels);
    xlabel(xlab);
    ylabel(ylab);
    savefig(imgName2);

end

function draw_hist_mag(p, imgName3; min=0.9, max=1.1, nbins=50)
    sel = abs.(p[:, 10]) .> 1e-10;
    p = p[sel, 6];
    sel = min .<= p .<= max;
    p = p[sel];

    figure(0);
    clf();
    plt[:hist](p, nbins);
    gca()[:set_xlim](min, max);
    xlabel("magnification");
    ylabel("counts");
    savefig(imgName3);
end

function draw_hist_new(files...; maxi=0.1, nbins=100, n=4, suff="-hist.pdf", suff2="-hist2.pdf", xrange=[], yrange=[], fRow=nothing, fSel=nothing, xlab="relative differences", ylab="counts")

    for f in files
        o = filter_data(f, xrange=xrange, yrange=yrange, fRow=fRow, fSel=fSel);
        r = o[1];
        desc = o[2];

        imgName = replace(f, r"-(out|diff).dat$" => suff);
        draw_h1(r, imgName; maxi=maxi, nbins=nbins, xlab=xlab, ylab=ylab, desc=desc);

        imgName2 = replace(f, r"-(out|diff).dat$" => suff2);
        draw_h2(r, imgName2; maxi=maxi, n=n, xlab=xlab, ylab=ylab, desc=desc);
    end
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
    draw_hist_new(ARGS...);
end
