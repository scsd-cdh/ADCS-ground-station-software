% test_sgp4  Checks the native SGP4 against published Vallado (SGP4-VER) vectors.
% Satellite 00005 at t = 0 and t = 360 min, TEME frame, km and km/s.
l1 = '1 00005U 58002B   00179.78495062  .00000023  00000-0  28098-4 0  4753';
l2 = '2 00005  34.2682 348.7242 1859667 331.7664  19.3264 10.82419157413667';

sr = tleToSatrec(l1, l2);
[r0, v0, e0] = sgp4prop(sr, 0);
[r1, v1, e1] = sgp4prop(sr, 360);
assert(e0 == 0 && e1 == 0, 'SGP4 returned an error code');

assert(norm(r0 - [ 7022.46529266  -1400.08296755      0.03995155]) < 1e-6, 'r(0) mismatch');
assert(norm(v0 - [    1.893841015      6.405893759      4.534807250]) < 1e-7, 'v(0) mismatch');
assert(norm(r1 - [-7154.03120202  -3783.17682504  -3536.19412294]) < 1e-6, 'r(360) mismatch');
assert(norm(v1 - [    4.741887409     -4.151817765     -2.093935425]) < 1e-7, 'v(360) mismatch');

% TLE parsing / checksum
tle = TLEData.parse(string(l1) + newline + string(l2), "test");
assert(tle.satelliteId == 5);
assert(abs(tle.epoch - datetime(2000,6,27,18,50,19.733568,'TimeZone','UTC')) < seconds(1e-3));
bad = strjoin([string(l1(1:68)) + "0"; string(l2)], newline);
assert(throws(@() TLEData.parse(bad, "test")), 'bad checksum should be rejected');

% Deep-space orbits must be rejected rather than silently mis-propagated
geo = sgp4init(0.0001, deg2rad(0.05), 0, 0, 0, 2*pi/1436.07, 0);
assert(geo.error == 7);

disp('test_sgp4: all checks passed');

function tf = throws(f)
    tf = false;
    try, f(); catch, tf = true; end
end
