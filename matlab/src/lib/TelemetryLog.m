classdef TelemetryLog < handle
    % TelemetryLog  Append-only JSON-lines file (easy to ingest into a DB / Grafana).
    properties (SetAccess = private)
        storagePath (1,1) string
    end
    methods
        function obj = TelemetryLog(storagePath)
            obj.storagePath = string(storagePath);
            folder = fileparts(obj.storagePath);
            if strlength(folder) > 0 && ~isfolder(folder), mkdir(folder); end
        end
        function append(obj, data)
            fid = fopen(obj.storagePath, 'a');
            if fid < 0, error('TelemetryLog:open', 'Cannot open %s', obj.storagePath); end
            cleaner = onCleanup(@() fclose(fid));
            for k = 1:numel(data)
                p = data(k);
                ts = p.timestamp;
                ts.TimeZone = 'UTC';
                ts.Format = 'yyyy-MM-dd''T''HH:mm:ss.SSS''Z''';     % ISO 8601, UTC
                s = struct('timestamp', char(string(ts)), ...
                           'attitude', p.attitude, 'magneticField', p.magneticField, ...
                           'stateOfChargePercent', p.stateOfChargePercent, ...
                           'rawPayload', char(p.rawPayload));
                fprintf(fid, '%s\n', jsonencode(s));
            end
        end
    end
end
