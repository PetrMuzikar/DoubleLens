#!/usr/bin/env julia

if length(ARGS) > 0
    n = parse(Int64, ARGS[1]);
else
    n = 100;
end

par = split(readline());

dom = par[1];

if dom == "a"
    xS = parse(Float64, par[3]);
    yS = parse(Float64, par[4]);
    r1 = parse(Float64, par[5]);
    r2 = parse(Float64, par[6]);

    r = r1 + (r2 - r1) * rand(n);
    phi = 2 * pi * rand(n);
    x = xS + r .* cos(phi);
    y = yS + r .* sin(phi);

elseif dom == "r"
    x1 = parse(Float64, par[3]);
    x2 = parse(Float64, par[4]);
    y1 = parse(Float64, par[5]);
    y2 = parse(Float64, par[6]);

    x = x1 + (x2 - x1) * rand(n);
    y = y1 + (y2 - y1) * rand(n);
else
    println(STDERR, "Bad domain type!");
    exit(1);
end

rays = hcat(x, y);

for k in 1:n
    @printf("%18.10e %18.10e\n", x[k], y[k]);
end
