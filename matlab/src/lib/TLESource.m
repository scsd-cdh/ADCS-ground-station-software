classdef (Abstract) TLESource < handle
    % TLESource  Interface: anything that can provide the latest TLE.
    methods (Abstract)
        tle = fetchLatestTLE(obj)
    end
end
