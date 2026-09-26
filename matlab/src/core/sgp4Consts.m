function [re, xke, j2, j3, j4] = sgp4Consts()
%SGP4CONSTS  WGS-72 constants used by SGP4 (as in the NORAD element sets).
%#codegen
mu  = 398600.8;                 % km^3/s^2
re  = 6378.135;                 % km
xke = 60.0 / sqrt(max(re^3 / mu, 0));   % 1/min (sqrt(GM) in Earth radii^1.5 / min)
j2  = 0.001082616;
j3  = -0.00000253881;
j4  = -0.00000165597;
end
