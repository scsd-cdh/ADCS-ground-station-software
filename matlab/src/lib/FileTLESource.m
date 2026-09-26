classdef FileTLESource < TLESource
    % FileTLESource  Reads a TLE from a local file (testing / offline use).
    properties (SetAccess = private)
        path (1,1) string
    end
    methods
        function obj = FileTLESource(path)
            obj.path = string(path);
        end
        function tle = fetchLatestTLE(obj)
            tle = TLEData.parse(string(fileread(obj.path)), "file");
        end
    end
end
