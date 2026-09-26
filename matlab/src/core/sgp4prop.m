function [r, v, err] = sgp4prop(satrec, tsince)
%SGP4PROP  Propagate to tsince [minutes from epoch].
%   r [km], v [km/s] in TEME (1x3 each). err: 0 ok, 1 eccentricity out of
%   range, 2 mean motion <= 0, 4 negative semi-latus rectum, 6 decayed.
%#codegen
[re, xke, j2, ~, ~] = sgp4Consts();
x2o3   = 2.0 / 3.0;
twopi  = 2.0 * pi;
vkmpersec = re * xke / 60.0;
r = zeros(1, 3);
v = zeros(1, 3);
err = 0;
if satrec.error ~= 0
    err = satrec.error;
    return
end
t = tsince;

% ---- secular gravity and atmospheric drag ----
xmdf   = satrec.mo + satrec.mdot * t;
argpdf = satrec.argpo + satrec.argpdot * t;
nodedf = satrec.nodeo + satrec.nodedot * t;
argpm  = argpdf;
mm     = xmdf;
t2     = t * t;
nodem  = nodedf + satrec.nodecf * t2;
tempa  = 1.0 - satrec.cc1 * t;
tempe  = satrec.bstar * satrec.cc4 * t;
templ  = satrec.t2cof * t2;

if satrec.isimp ~= 1
    delomg = satrec.omgcof * t;
    delm   = satrec.xmcof * ((1.0 + satrec.eta * cos(xmdf))^3 - satrec.delmo);
    temp   = delomg + delm;
    mm     = xmdf + temp;
    argpm  = argpdf - temp;
    t3     = t2 * t;
    t4     = t3 * t;
    tempa  = tempa - satrec.d2 * t2 - satrec.d3 * t3 - satrec.d4 * t4;
    tempe  = tempe + satrec.bstar * satrec.cc5 * (sin(mm) - satrec.sinmao);
    templ  = templ + satrec.t3cof * t3 + t4 * (satrec.t4cof + t * satrec.t5cof);
end

nm    = satrec.no;
em    = satrec.ecco;
inclm = satrec.inclo;
if nm <= 0.0
    err = 2;  return
end
am = real((xke / nm)^x2o3) * tempa * tempa;
nm = xke / real(am^1.5);
em = em - tempe;
if em >= 1.0 || em < -0.001
    err = 1;  return
end
if em < 1.0e-6
    em = 1.0e-6;
end
mm  = mm + satrec.no * templ;
xlm = mm + argpm + nodem;
nodem = rem(nodem, twopi);
argpm = rem(argpm, twopi);
xlm   = rem(xlm, twopi);
mm    = rem(xlm - argpm - nodem, twopi);

sinip = sin(inclm);
cosip = cos(inclm);
ep = em;  xincp = inclm;  argpp = argpm;  nodep = nodem;  mp = mm;

% ---- long-period periodics ----
axnl = ep * cos(argpp);
temp = 1.0 / (am * (1.0 - ep * ep));
aynl = ep * sin(argpp) + temp * satrec.aycof;
xl   = mp + argpp + nodep + temp * satrec.xlcof * axnl;

% ---- Kepler's equation ----
u = rem(xl - nodep, twopi);
eo1 = u;
tem5 = 9999.9;
ktr = 1;
sineo1 = 0.0;
coseo1 = 1.0;
while abs(tem5) >= 1.0e-12 && ktr <= 10
    sineo1 = sin(eo1);
    coseo1 = cos(eo1);
    tem5 = 1.0 - coseo1 * axnl - sineo1 * aynl;
    tem5 = (u - aynl * coseo1 + axnl * sineo1 - eo1) / tem5;
    if abs(tem5) >= 0.95
        if tem5 > 0.0
            tem5 = 0.95;
        else
            tem5 = -0.95;
        end
    end
    eo1 = eo1 + tem5;
    ktr = ktr + 1;
end

% ---- short-period preliminary quantities ----
ecose = axnl * coseo1 + aynl * sineo1;
esine = axnl * sineo1 - aynl * coseo1;
el2   = axnl * axnl + aynl * aynl;
pl    = am * (1.0 - el2);
if pl < 0.0
    err = 4;  return
end
rl     = am * (1.0 - ecose);
rdotl  = sqrt(am) * esine / rl;
rvdotl = sqrt(pl) / rl;
betal  = sqrt(1.0 - el2);
temp   = esine / (1.0 + betal);
sinu   = am / rl * (sineo1 - aynl - axnl * temp);
cosu   = am / rl * (coseo1 - axnl + aynl * temp);
su     = atan2(sinu, cosu);
sin2u  = (cosu + cosu) * sinu;
cos2u  = 1.0 - 2.0 * sinu * sinu;
temp   = 1.0 / pl;
temp1  = 0.5 * j2 * temp;
temp2  = temp1 * temp;

% ---- short-period periodics ----
mrt   = rl * (1.0 - 1.5 * temp2 * betal * satrec.con41) + ...
        0.5 * temp1 * satrec.x1mth2 * cos2u;
su    = su - 0.25 * temp2 * satrec.x7thm1 * sin2u;
xnode = nodep + 1.5 * temp2 * cosip * sin2u;
xinc  = xincp + 1.5 * temp2 * cosip * sinip * cos2u;
mvt   = rdotl - nm * temp1 * satrec.x1mth2 * sin2u / xke;
rvdot = rvdotl + nm * temp1 * (satrec.x1mth2 * cos2u + 1.5 * satrec.con41) / xke;

% ---- orientation vectors ----
sinsu = sin(su);   cossu = cos(su);
snod  = sin(xnode); cnod  = cos(xnode);
sini  = sin(xinc);  cosi  = cos(xinc);
xmx = -snod * cosi;
xmy = cnod * cosi;
ux = xmx * sinsu + cnod * cossu;
uy = xmy * sinsu + snod * cossu;
uz = sini * sinsu;
vx = xmx * cossu - cnod * sinsu;
vy = xmy * cossu - snod * sinsu;
vz = sini * cossu;

if mrt < 1.0
    err = 6;  return          % decayed
end
r = [mrt * ux, mrt * uy, mrt * uz] * re;
v = [mvt * ux + rvdot * vx, mvt * uy + rvdot * vy, mvt * uz + rvdot * vz] * vkmpersec;
end
