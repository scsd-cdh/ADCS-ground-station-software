function [rEcef, vEcef] = temeToEcef(rTeme, vTeme, jdUt1)
%TEMETOECEF  TEME -> ECEF (rotation by GMST; polar motion ignored, UT1 ~ UTC).
%   rTeme, vTeme: Nx3 (any length unit, velocity per second). jdUt1: Nx1.
%   Neglecting UT1-UTC (<0.9 s) and polar motion costs at most a few hundred
%   metres, well inside SGP4's own ~1 km accuracy.
%#codegen
th = gmst(jdUt1(:));
c = cos(th);
s = sin(th);
rEcef = [ c .* rTeme(:,1) + s .* rTeme(:,2), ...
         -s .* rTeme(:,1) + c .* rTeme(:,2), ...
          rTeme(:,3)];
vp = [ c .* vTeme(:,1) + s .* vTeme(:,2), ...
      -s .* vTeme(:,1) + c .* vTeme(:,2), ...
       vTeme(:,3)];
w = 7.29211514670698e-5;                      % Earth rotation rate [rad/s]
vEcef = [vp(:,1) + w * rEcef(:,2), ...
         vp(:,2) - w * rEcef(:,1), ...
         vp(:,3)];
end

function th = gmst(jdUt1)
% IAU-82 Greenwich mean sidereal time [rad].
tut1 = (jdUt1 - 2451545.0) / 36525.0;
sec  = -6.2e-6 * tut1.^3 + 0.093104 * tut1.^2 + ...
       (876600 * 3600 + 8640184.812866) * tut1 + 67310.54841;
th = rem(deg2rad(sec / 240.0), 2 * pi);
end
