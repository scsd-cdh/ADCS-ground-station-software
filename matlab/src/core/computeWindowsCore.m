function win = computeWindowsCore(tSec, posEcef, gsLatDeg, gsLonDeg, gsAltM, minElevDeg)
%COMPUTEWINDOWSCORE  Pass windows above an elevation mask (codegen-friendly).
%   tSec    Nx1  sample times [s] (any epoch, increasing)
%   posEcef Nx3  spacecraft ECEF position [m]
%   win     Mx5  [startSec endSec maxElevDeg startTruncated endTruncated]
%               maxElevDeg is parabola-refined between samples.
%               AOS/LOS are linearly interpolated between samples.
%               "Truncated" = window touches the first/last sample, so the true
%               edge lies outside the propagated horizon.
%#codegen
tSec = tSec(:);
n = numel(tSec);
elev = elevationAngles(posEcef, gsLatDeg, gsLonDeg, gsAltM);

vis    = double(elev >= minElevDeg);
d      = diff([0; vis; 0]);
starts = find(d == 1);
ends   = find(d == -1) - 1;
m      = numel(starts);
win    = zeros(m, 5);

for k = 1:m
    i0 = starts(k);
    i1 = ends(k);
    if i0 > 1
        e0 = elev(i0-1); e1 = elev(i0);
        tS = tSec(i0-1) + (minElevDeg - e0)/(e1 - e0) * (tSec(i0) - tSec(i0-1));
        sTrunc = 0;
    else
        tS = tSec(i0); sTrunc = 1;
    end
    if i1 < n
        e0 = elev(i1); e1 = elev(i1+1);
        tE = tSec(i1) + (e0 - minElevDeg)/(e0 - e1) * (tSec(i1+1) - tSec(i1));
        eTrunc = 0;
    else
        tE = tSec(i1); eTrunc = 1;
    end
    % peak elevation: highest sample, refined by a parabola through its neighbours
    [pk, im] = max(elev(i0:i1));
    im = im + i0 - 1;
    if im > 1 && im < n
        y0 = elev(im-1); y1 = elev(im); y2 = elev(im+1);
        den = y0 - 2*y1 + y2;
        if den < 0
            p = 0.5 * (y0 - y2) / den;
            pk = max(pk, y1 - 0.25 * (y0 - y2) * p);
        end
    end
    win(k, :) = [tS, tE, pk, sTrunc, eTrunc];
end
end

function elev = elevationAngles(posEcef, latDeg, lonDeg, altM)
% Elevation of each ECEF point as seen from a WGS84 geodetic station.
a  = 6378137.0;
f  = 1/298.257223563;
e2 = f * (2 - f);
lat = latDeg * pi/180;
lon = lonDeg * pi/180;
N   = a / sqrt(1 - e2 * sin(lat)^2);
gs  = [(N + altM) * cos(lat) * cos(lon), ...
       (N + altM) * cos(lat) * sin(lon), ...
       (N * (1 - e2) + altM) * sin(lat)];
up  = [cos(lat)*cos(lon), cos(lat)*sin(lon), sin(lat)];
d   = posEcef - gs;
rng = sqrt(sum(d.^2, 2));
elev = asind((d * up.') ./ rng);
end
