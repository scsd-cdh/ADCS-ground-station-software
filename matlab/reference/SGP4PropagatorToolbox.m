classdef SGP4PropagatorToolbox < handle
    % SGP4PropagatorToolbox  Reference propagator (needs Aerospace Toolbox). Use it only to cross-check SGP4Propagator.
    % Wraps the SGP4 propagator inside Aerospace Toolbox's satelliteScenario
    % (same engine as your existing script). Not MATLAB Coder compatible; for a
    % codegen/flight-software path, replace the body of propagate() with a
    % ported Vallado SGP4 and keep this interface.
    properties (SetAccess = private)
        stepSizeSec (1,1) double
        tle = []
    end
    properties (Access = private)
        tleFile (1,1) string = ""
    end

    methods
        function obj = SGP4PropagatorToolbox(stepSizeSec)
            arguments
                stepSizeSec (1,1) double {mustBePositive} = 30
            end
            obj.stepSizeSec = stepSizeSec;
        end

        function updateInitialConditions(obj, tle)
            obj.removeTempFile();
            nm = tle.name;
            if strlength(nm) == 0, nm = "SAT"; end
            f = string(tempname) + ".tle";
            writelines([nm; tle.line1; tle.line2], f);
            obj.tleFile = f;
            obj.tle = tle;
        end

        function pos = propagate(obj, t)
            % t: datetime vector. Returns SpacecraftPosition at exactly those times.
            arguments
                obj
                t (:,1) datetime
            end
            if strlength(obj.tleFile) == 0
                error('SGP4PropagatorToolbox:noTLE', 'Call updateInitialConditions(tle) first.');
            end
            t.TimeZone = 'UTC';
            t0 = min(t);
            t1 = max(t) + seconds(obj.stepSizeSec);   % pad so the last query is inside the grid
            sc  = satelliteScenario(t0, t1, obj.stepSizeSec);
            sat = satellite(sc, obj.tleFile);
            [p, v] = states(sat, "CoordinateFrame", "ecef");   % 3xN, m and m/s
            grid = (0:size(p,2)-1)' * obj.stepSizeSec;
            q = seconds(t - t0);
            P = interp1(grid, p.', q, 'spline');
            V = interp1(grid, v.', q, 'spline');
            pos = SpacecraftPosition(P, V, t);
        end

        function delete(obj)
            obj.removeTempFile();
        end
    end

    methods (Access = private)
        function removeTempFile(obj)
            if strlength(obj.tleFile) > 0 && isfile(obj.tleFile)
                delete(obj.tleFile);
            end
            obj.tleFile = "";
        end
    end
end
