classdef SGP4Propagator < handle
    % SGP4Propagator  Native SGP4 (near-Earth), output in ECEF. No toolbox needed.
    %   updateInitialConditions(tle) swaps in new mean elements atomically: if the
    %   new TLE is rejected (e.g. deep-space orbit), the previous one stays active.
    %   propagate(t) accepts any datetime vector (UTC), cost ~ tens of us / point.
    properties (SetAccess = private)
        tle    = []
        satrec = []
        epoch          % datetime, UTC
    end

    methods
        function updateInitialConditions(obj, tle)
            sr = tleToSatrec(tle.line1, tle.line2);
            if sr.error ~= 0
                error('SGP4Propagator:init', ...
                      'SGP4 init failed for NORAD %g (error %d; 7 = deep-space orbit, not supported).', ...
                      tle.satelliteId, sr.error);
            end
            obj.satrec = sr;
            obj.tle    = tle;
            obj.epoch  = tle.epoch;
        end

        function pos = propagate(obj, t)
            arguments
                obj
                t (:,1) datetime
            end
            if isempty(obj.satrec)
                error('SGP4Propagator:noTLE', 'Call updateInitialConditions(tle) first.');
            end
            t.TimeZone = 'UTC';
            sr = obj.satrec;
            tsince = minutes(t - obj.epoch);
            n  = numel(t);
            rT = zeros(n, 3);
            vT = zeros(n, 3);
            for k = 1:n
                [r, v, err] = sgp4prop(sr, tsince(k));
                if err ~= 0
                    error('SGP4Propagator:propagate', 'SGP4 error %d at %s.', err, string(t(k)));
                end
                rT(k, :) = r;
                vT(k, :) = v;
            end
            jd = 2440587.5 + posixtime(t) / 86400;
            [rE, vE] = temeToEcef(rT * 1000, vT * 1000, jd);   % km -> m
            pos = SpacecraftPosition(rE, vE, t);
        end
    end
end
