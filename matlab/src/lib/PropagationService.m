classdef PropagationService < handle
    % PropagationService  Keeps SGP4 running and its initial conditions current.
    %   * stepTimer  (every propagationPeriodSec, default 1 s): propagates to "now"
    %                and publishes the result in `latest` + StateUpdated event.
    %   * tleTimer   (every tleCheckPeriodSec, default 60 s): asks the TLESource for
    %                the latest TLE. If its epoch is newer, the propagator's initial
    %                conditions are replaced immediately, TLEUpdated fires, and the
    %                state is re-propagated on the spot.
    %   The two run on separate timers so a slow network request cannot stall
    %   propagation. MATLAB timer callbacks run one at a time, so the swap of
    %   initial conditions is never interleaved with a propagate call.
    %   NOTE: network traffic is throttled inside CelestrakClient (PollIntervalSec,
    %   default 2 h, matching CelesTrak's data refresh). Checking more often than
    %   the source refreshes gains nothing and risks being blocked.
    properties
        propagationPeriodSec (1,1) double = 1
        tleCheckPeriodSec    (1,1) double = 60
        staleTleDays         (1,1) double = 7      % warn when TLE older than this
        nowFcn (1,1) function_handle = @() datetime('now', 'TimeZone', 'UTC')
    end
    properties (SetAccess = private)
        source
        propagator
        currentTLE = []
        latest     = []          % SpacecraftPosition (single sample, ECEF, m)
    end
    properties (Access = private)
        tleTimer  = []
        stepTimer = []
        staleWarned (1,1) logical = false
    end
    events
        TLEUpdated
        StateUpdated
    end

    methods
        function obj = PropagationService(source, propagator, options)
            arguments
                source
                propagator
                options.PropagationPeriodSec (1,1) double = 1
                options.TleCheckPeriodSec    (1,1) double = 60
                options.NowFcn (1,1) function_handle = @() datetime('now', 'TimeZone', 'UTC')
            end
            obj.source = source;
            obj.propagator = propagator;
            obj.propagationPeriodSec = options.PropagationPeriodSec;
            obj.tleCheckPeriodSec    = options.TleCheckPeriodSec;
            obj.nowFcn = options.NowFcn;
        end

        function tf = checkTLE(obj)
            % Returns true if a newer TLE was found and applied.
            tf = false;
            try
                tle = obj.source.fetchLatestTLE();
            catch ME
                warning('PropagationService:tle', '%s', ME.message);
                return
            end
            if tle.isNewerThan(obj.currentTLE)
                try
                    obj.propagator.updateInitialConditions(tle);
                catch ME
                    warning('PropagationService:init', '%s', ME.message);
                    return
                end
                obj.currentTLE = tle;
                obj.staleWarned = false;
                tf = true;
                notify(obj, 'TLEUpdated');
                obj.step();
            end
            obj.warnIfStale();
        end

        function step(obj)
            if isempty(obj.currentTLE), return; end
            try
                obj.latest = obj.propagator.propagate(obj.nowFcn());
            catch ME
                warning('PropagationService:propagate', '%s', ME.message);
                return
            end
            notify(obj, 'StateUpdated');
        end

        function age = tleAgeDays(obj)
            if isempty(obj.currentTLE), age = NaN; return; end
            age = days(obj.nowFcn() - obj.currentTLE.epoch);
        end

        function startService(obj)
            if obj.isRunning(), return; end
            obj.checkTLE();                         % first fetch immediately
            obj.tleTimer = timer('ExecutionMode', 'fixedSpacing', 'Period', obj.tleCheckPeriodSec, ...
                'BusyMode', 'drop', 'TimerFcn', @(~,~) obj.checkTLE());
            obj.stepTimer = timer('ExecutionMode', 'fixedSpacing', 'Period', obj.propagationPeriodSec, ...
                'BusyMode', 'drop', 'TimerFcn', @(~,~) obj.step());
            start(obj.tleTimer);
            start(obj.stepTimer);
        end

        function stopService(obj)
            for tm = {obj.tleTimer, obj.stepTimer}
                if ~isempty(tm{1}) && isvalid(tm{1})
                    stop(tm{1});
                    delete(tm{1});
                end
            end
            obj.tleTimer = [];
            obj.stepTimer = [];
        end

        function tf = isRunning(obj)
            tf = ~isempty(obj.stepTimer) && isvalid(obj.stepTimer);
        end

        function delete(obj)
            obj.stopService();
        end
    end

    methods (Access = private)
        function warnIfStale(obj)
            if ~obj.staleWarned && obj.tleAgeDays() > obj.staleTleDays
                warning('PropagationService:stale', ...
                    'TLE is %.1f days old; SGP4 accuracy degrades with age.', obj.tleAgeDays());
                obj.staleWarned = true;
            end
        end
    end
end
