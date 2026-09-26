function T = commWindowsScenario(tleFile, gsLLA, t0, durationHours, minElevDeg, sampleSec)
%COMMWINDOWSSCENARIO  Reference implementation using satelliteScenario access().
%   Use it to cross-check computeWindowsCore. gsLLA = [latDeg lonDeg altM].
sc  = satelliteScenario(t0, t0 + hours(durationHours), sampleSec);
sat = satellite(sc, tleFile);
gs  = groundStation(sc, gsLLA(1), gsLLA(2), Altitude=gsLLA(3), MinElevationAngle=minElevDeg);
ac  = access(sat, gs);
T   = accessIntervals(ac);
end
