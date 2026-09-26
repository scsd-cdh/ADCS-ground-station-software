classdef CommunicationWindow
    properties (SetAccess = private)
        startTime         % datetime, UTC
        endTime           % datetime, UTC
        maxElevationDeg (1,1) double
        usableSkyFraction (1,1) double = 1
    end
    properties (Dependent)
        durationSec
        usableDurationSec   % duration * usableSkyFraction (planning estimate)
    end
    methods
        function obj = CommunicationWindow(startTime, endTime, maxElevationDeg, usableSkyFraction)
            obj.startTime = startTime;
            obj.endTime = endTime;
            obj.maxElevationDeg = maxElevationDeg;
            if nargin > 3, obj.usableSkyFraction = usableSkyFraction; end
        end
        function d = get.durationSec(obj),       d = seconds(obj.endTime - obj.startTime); end
        function d = get.usableDurationSec(obj), d = obj.durationSec * obj.usableSkyFraction; end
    end
end
