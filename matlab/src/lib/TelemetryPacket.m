classdef TelemetryPacket
    properties (SetAccess = private)
        attitude             (1,4) double = [0 0 0 1]   % quaternion [x y z w]
        magneticField        (1,3) double = zeros(1,3)  % T
        stateOfChargePercent (1,1) double = NaN
        timestamp                                       % datetime, UTC
        rawPayload           (1,1) string = ""
    end
    methods
        function obj = TelemetryPacket(options)
            arguments
                options.Attitude             (1,4) double = [0 0 0 1]
                options.MagneticField        (1,3) double = zeros(1,3)
                options.StateOfChargePercent (1,1) double = NaN
                options.Timestamp            (1,1) datetime = datetime('now', 'TimeZone', 'UTC')
                options.RawPayload           (1,1) string = ""
            end
            obj.attitude = options.Attitude;
            obj.magneticField = options.MagneticField;
            obj.stateOfChargePercent = options.StateOfChargePercent;
            obj.timestamp = options.Timestamp;
            obj.rawPayload = options.RawPayload;
        end
    end
end
