function satrec = sgp4init(ecco, inclo, nodeo, argpo, mo, noKozai, bstar)
%SGP4INIT  Initialise near-Earth SGP4 (period < 225 min) from mean elements.
%   Inputs (TLE mean elements): ecco [-], inclo/nodeo/argpo/mo [rad],
%   noKozai [rad/min] (Kozai mean motion as in the TLE), bstar [1/Earth radii].
%   satrec.error: 0 ok, 1 bad elements, 7 deep-space orbit (not implemented).
%   Port of Vallado's reference implementation (opsmode 'i'), validated
%   against the published SGP4-VER test vectors (see test_sgp4.m).
%#codegen
[re, xke, j2, j3, j4] = sgp4Consts();
j3oj2 = j3 / j2;
x2o3  = 2.0 / 3.0;

satrec = struct('error', 0, 'ecco', ecco, 'inclo', inclo, 'nodeo', nodeo, ...
    'argpo', argpo, 'mo', mo, 'no', 0, 'bstar', bstar, 'con41', 0, 'isimp', 0, ...
    'eta', 0, 'cc1', 0, 'cc4', 0, 'cc5', 0, 'x1mth2', 0, 'mdot', 0, ...
    'argpdot', 0, 'nodedot', 0, 'omgcof', 0, 'xmcof', 0, 'nodecf', 0, ...
    't2cof', 0, 'xlcof', 0, 'aycof', 0, 'delmo', 0, 'sinmao', 0, 'x7thm1', 0, ...
    'd2', 0, 'd3', 0, 'd4', 0, 't3cof', 0, 't4cof', 0, 't5cof', 0);

if noKozai <= 0 || ecco < 0 || ecco >= 1
    satrec.error = 1;
    return
end

% ---- initl: un-Kozai the mean motion, auxiliary quantities ----
eccsq  = ecco * ecco;
omeosq = 1.0 - eccsq;
rteosq = sqrt(max(omeosq, 0));   % ecco < 1 is guaranteed above; keeps codegen real-valued
cosio  = cos(inclo);
cosio2 = cosio * cosio;

ak   = real((xke / noKozai)^x2o3);
d1   = 0.75 * j2 * (3.0 * cosio2 - 1.0) / (rteosq * omeosq);
del1 = d1 / (ak * ak);
adel = ak * (1.0 - del1 * del1 - del1 * (1.0/3.0 + 134.0 * del1 * del1 / 81.0));
del1 = d1 / (adel * adel);
no   = noKozai / (1.0 + del1);
satrec.no = no;

ao    = real((xke / no)^x2o3);
sinio = sin(inclo);
po    = ao * omeosq;
con42 = 1.0 - 5.0 * cosio2;
con41 = -con42 - cosio2 - cosio2;
posq  = po * po;
rp    = ao * (1.0 - ecco);
satrec.con41 = con41;

if 2.0 * pi / no >= 225.0
    satrec.error = 7;      % deep-space (SDP4) not implemented
    return
end

% ---- near-Earth initialisation ----
ss      = 78.0 / re + 1.0;
qzms2t  = ((120.0 - 78.0) / re)^4;
if rp < (220.0 / re + 1.0)
    satrec.isimp = 1;
else
    satrec.isimp = 0;
end
sfour  = ss;
qzms24 = qzms2t;
perige = (rp - 1.0) * re;
if perige < 156.0
    sfour = perige - 78.0;
    if perige < 98.0
        sfour = 20.0;
    end
    qzms24 = ((120.0 - sfour) / re)^4;
    sfour  = sfour / re + 1.0;
end
pinvsq = 1.0 / posq;

tsi   = 1.0 / (ao - sfour);
eta   = ao * ecco * tsi;
etasq = eta * eta;
eeta  = ecco * eta;
psisq = abs(1.0 - etasq);
coef  = qzms24 * tsi^4;
coef1 = coef / real(psisq^3.5);
cc2   = coef1 * no * (ao * (1.0 + 1.5 * etasq + eeta * (4.0 + etasq)) ...
        + 0.375 * j2 * tsi / psisq * con41 * (8.0 + 3.0 * etasq * (8.0 + etasq)));
cc1   = bstar * cc2;
cc3   = 0.0;
if ecco > 1.0e-4
    cc3 = -2.0 * coef * tsi * j3oj2 * no * sinio / ecco;
end
x1mth2 = 1.0 - cosio2;
cc4 = 2.0 * no * coef1 * ao * omeosq * (eta * (2.0 + 0.5 * etasq) ...
      + ecco * (0.5 + 2.0 * etasq) - j2 * tsi / (ao * psisq) * ...
      (-3.0 * con41 * (1.0 - 2.0 * eeta + etasq * (1.5 - 0.5 * eeta)) ...
      + 0.75 * x1mth2 * (2.0 * etasq - eeta * (1.0 + etasq)) * cos(2.0 * argpo)));
cc5 = 2.0 * coef1 * ao * omeosq * (1.0 + 2.75 * (etasq + eeta) + eeta * etasq);

cosio4 = cosio2 * cosio2;
temp1  = 1.5 * j2 * pinvsq * no;
temp2  = 0.5 * temp1 * j2 * pinvsq;
temp3  = -0.46875 * j4 * pinvsq * pinvsq * no;
mdot   = no + 0.5 * temp1 * rteosq * con41 + 0.0625 * temp2 * rteosq * ...
         (13.0 - 78.0 * cosio2 + 137.0 * cosio4);
argpdot = -0.5 * temp1 * con42 + 0.0625 * temp2 * ...
          (7.0 - 114.0 * cosio2 + 395.0 * cosio4) + ...
          temp3 * (3.0 - 36.0 * cosio2 + 49.0 * cosio4);
xhdot1  = -temp1 * cosio;
nodedot = xhdot1 + (0.5 * temp2 * (4.0 - 19.0 * cosio2) + ...
          2.0 * temp3 * (3.0 - 7.0 * cosio2)) * cosio;

omgcof = bstar * cc3 * cos(argpo);
xmcof  = 0.0;
if ecco > 1.0e-4
    xmcof = -x2o3 * coef * bstar / eeta;
end
nodecf = 3.5 * omeosq * xhdot1 * cc1;
t2cof  = 1.5 * cc1;
if abs(cosio + 1.0) > 1.5e-12
    xlcof = -0.25 * j3oj2 * sinio * (3.0 + 5.0 * cosio) / (1.0 + cosio);
else
    xlcof = -0.25 * j3oj2 * sinio * (3.0 + 5.0 * cosio) / 1.5e-12;
end
aycof  = -0.5 * j3oj2 * sinio;
delmo  = (1.0 + eta * cos(mo))^3;
sinmao = sin(mo);
x7thm1 = 7.0 * cosio2 - 1.0;

if satrec.isimp ~= 1
    cc1sq = cc1 * cc1;
    d2    = 4.0 * ao * tsi * cc1sq;
    temp  = d2 * tsi * cc1 / 3.0;
    d3    = (17.0 * ao + sfour) * temp;
    d4    = 0.5 * temp * ao * tsi * (221.0 * ao + 31.0 * sfour) * cc1;
    t3cof = d2 + 2.0 * cc1sq;
    t4cof = 0.25 * (3.0 * d3 + cc1 * (12.0 * d2 + 10.0 * cc1sq));
    t5cof = 0.2 * (3.0 * d4 + 12.0 * cc1 * d3 + 6.0 * d2 * d2 + ...
            15.0 * cc1sq * (2.0 * d2 + cc1sq));
    satrec.d2 = d2;  satrec.d3 = d3;  satrec.d4 = d4;
    satrec.t3cof = t3cof;  satrec.t4cof = t4cof;  satrec.t5cof = t5cof;
end

satrec.eta = eta;      satrec.cc1 = cc1;      satrec.cc4 = cc4;   satrec.cc5 = cc5;
satrec.x1mth2 = x1mth2; satrec.mdot = mdot;   satrec.argpdot = argpdot;
satrec.nodedot = nodedot; satrec.omgcof = omgcof; satrec.xmcof = xmcof;
satrec.nodecf = nodecf; satrec.t2cof = t2cof; satrec.xlcof = xlcof;
satrec.aycof = aycof;  satrec.delmo = delmo;  satrec.sinmao = sinmao;
satrec.x7thm1 = x7thm1;
end
