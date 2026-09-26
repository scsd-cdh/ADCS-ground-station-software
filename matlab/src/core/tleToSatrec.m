function satrec = tleToSatrec(line1, line2)
%TLETOSATREC  Build an SGP4 record from the two 69-character TLE lines.
%   Lines are assumed already validated (see TLEData.parse).
%#codegen
l1 = char(line1);
l2 = char(line2);
d2r = pi / 180;

sgn = 1;
if l1(54) == '-'
    sgn = -1;
end
bstar = sgn * toReal(l1(55:59)) * 1e-5 * 10^toReal(l1(60:61));

inclo = toReal(l2(9:16))  * d2r;
nodeo = toReal(l2(18:25)) * d2r;
ecco  = toReal(['0.' l2(27:33)]);
argpo = toReal(l2(35:42)) * d2r;
mo    = toReal(l2(44:51)) * d2r;
noKozai = toReal(l2(53:63)) * 2 * pi / 1440;      % rev/day -> rad/min

satrec = sgp4init(ecco, inclo, nodeo, argpo, mo, noKozai, bstar);
end

function x = toReal(s)
% str2double is complex-valued under codegen; TLE fields are always real.
x = real(str2double(s));
end
