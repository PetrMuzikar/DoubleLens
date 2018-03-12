#!/usr/bin/env julia

function histBins(data, bins)
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

