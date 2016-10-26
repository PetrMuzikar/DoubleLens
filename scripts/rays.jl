#!/usr/bin/env julia

n = 100;

#x1 = -1e-5;
#x2 = 2e-5;
#y1 = -1.5e-5;
#y2 = 1.5e-5;
#x = x1 + (x2 - x1) * rand(n);
#y = y1 + (y2 - y1) * rand(n);

xS = -7.071500000011e-07;
yS = -1.359999999284e-07;
r1 = 1.001241124736e+00;
r2 = 1.001263502437e+00;
r = r1 + (r2 - r1) * rand(n);
phi = 2 * pi * rand(n);
x = r .* cos(phi);
y = r .* sin(phi);

rays = hcat(x, y);

#writedlm(STDOUT, rays);

for k in 1:n
    @printf("%18.10e %18.10e\n", x[k], y[k]);
end
