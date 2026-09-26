classdef CommunicationWindowCalculator < handle
    % CommunicationWindowCalculator  Keeps the pass list current and announces
    % window open/close. It uses a PropagationService for the orbit: whenever the
    % service applies a new TLE (TLEUpdated) the pass list is recomputed at once;
    % tick() (periodic) refreshes the look-ahead horizon and detects window edges.
    % startPolling() drives tick() from a MATLAB timer.
    %
    % minElevationDeg defines the windows. usableSkyFraction (planning estimate) is
    % NOT applied to the windows; it is only reported as
    % CommunicationWindow.usableDurationSec.
    properties
        groundStationLat  (1,1) double
        groundStationLon  (1,1) double
        groundStationAlt  (1,1) double = 0        % m
        minElevationDeg   (1,1) double = 10
        usableSkyFraction (1,1) double = 0.6
        pollIntervalSec   (1,1) double = 5        % tick period
        horizonHours      (1,1) double = 24
        stepSec           (1,1) double = 30
        peakRefineStepSec (1,1) double = 1        % 0 disables exact peak refinement
    end
    properties (SetAccess = private)
        service
        windows    = CommunicationWindow.empty
        horizonEnd = NaT
        wasWithin (1,1) logical = false
    end
    properties (Access = private)
        tmr = []
        tleListener = []
    end
    events
        WindowOpened
        WindowClosed
    end

    methods
        function obj = CommunicationWindowCalculator(service, lat, lon, alt, options)
            arguments
                service
                lat (1,1) double
                lon (1,1) double
                alt (1,1) double = 0
                options.MinElevationDeg   (1,1) double = 10
                options.UsableSkyFraction (1,1) double = 0.6
                options.PollIntervalSec   (1,1) double = 5
                options.HorizonHours      (1,1) double = 24
                options.StepSec           (1,1) double = 30
                options.PeakRefineStepSec (1,1) double = 1
            end
            obj.service = service;
            obj.groundStationLat = lat;
            obj.groundStationLon = lon;
            obj.groundStationAlt = alt;
            obj.minElevationDeg   = options.MinElevationDeg;
            obj.usableSkyFraction = options.UsableSkyFraction;
            obj.pollIntervalSec   = options.PollIntervalSec;
            obj.horizonHours      = options.HorizonHours;
            obj.stepSec           = options.StepSec;
            obj.peakRefineStepSec = options.PeakRefineStepSec;
            obj.tleListener = addlistener(service, 'TLEUpdated', @(~,~) obj.recompute());
        end

        function recompute(obj)
            % Re-propagate the look-ahead horizon and rebuild the pass list.
            if isempty(obj.service.currentTLE), return; end
            now = obj.service.nowFcn();
            t = (now : seconds(obj.stepSec) : now + hours(obj.horizonHours))';
            pos = obj.service.propagator.propagate(t);
            obj.windows = obj.computeCommunicationWindows(pos);
            obj.horizonEnd = t(end);
            obj.updateWindowState(now);
        end

        function tick(obj)
            if isempty(obj.service.currentTLE), return; end
            now = obj.service.nowFcn();
            if isnat(obj.horizonEnd) || now > obj.horizonEnd - hours(2)
                obj.recompute();                 % also updates window state
            else
                obj.updateWindowState(now);
            end
        end

        function out = computeCommunicationWindows(obj, position)
            t0 = position.timestamp(1);
            w = computeWindowsCore(seconds(position.timestamp - t0), position.position, ...
                    obj.groundStationLat, obj.groundStationLon, obj.groundStationAlt, ...
                    obj.minElevationDeg);
            out = CommunicationWindow.empty;
            for k = 1:size(w, 1)
                ws = t0 + seconds(w(k,1));
                we = t0 + seconds(w(k,2));
                pk = w(k,3);
                if obj.peakRefineStepSec > 0
                    % Resample the pass finely: coarse samples miss near-overhead peaks by ~0.2 deg.
                    tf = (ws : seconds(obj.peakRefineStepSec) : we)';
                    pf = obj.service.propagator.propagate(tf);
                    wf = computeWindowsCore(seconds(tf - tf(1)), pf.position, ...
                            obj.groundStationLat, obj.groundStationLon, obj.groundStationAlt, -90);
                    pk = max(pk, wf(1,3));
                end
                out(end+1) = CommunicationWindow(ws, we, pk, obj.usableSkyFraction); %#ok<AGROW>
            end
        end

        function tf = isWithinWindow(obj, t)
            tf = false;
            for k = 1:numel(obj.windows)
                if t >= obj.windows(k).startTime && t <= obj.windows(k).endTime
                    tf = true;
                    return
                end
            end
        end

        function T = windowTable(obj)
            n = numel(obj.windows);
            Start = NaT(n, 1, 'TimeZone', 'UTC');
            End   = NaT(n, 1, 'TimeZone', 'UTC');
            DurationSec     = zeros(n, 1);
            MaxElevationDeg = zeros(n, 1);
            UsableSec       = zeros(n, 1);
            for k = 1:n
                w = obj.windows(k);
                Start(k) = w.startTime;
                End(k)   = w.endTime;
                DurationSec(k)     = w.durationSec;
                MaxElevationDeg(k) = w.maxElevationDeg;
                UsableSec(k)       = w.usableDurationSec;
            end
            T = table(Start, End, DurationSec, MaxElevationDeg, UsableSec);
        end

        function startPolling(obj)
            if ~isempty(obj.tmr) && isvalid(obj.tmr), return; end
            obj.tmr = timer('ExecutionMode', 'fixedSpacing', 'Period', obj.pollIntervalSec, ...
                            'BusyMode', 'drop', 'TimerFcn', @(~,~) obj.tick());
            start(obj.tmr);
        end

        function stopPolling(obj)
            if ~isempty(obj.tmr) && isvalid(obj.tmr)
                stop(obj.tmr);
                delete(obj.tmr);
            end
            obj.tmr = [];
        end

        function delete(obj)
            obj.stopPolling();
        end
    end

    methods (Access = private)
        function updateWindowState(obj, now)
            inNow = obj.isWithinWindow(now);
            if inNow && ~obj.wasWithin
                obj.wasWithin = true;
                notify(obj, 'WindowOpened');
            elseif ~inNow && obj.wasWithin
                obj.wasWithin = false;
                notify(obj, 'WindowClosed');
            end
        end
    end
end
