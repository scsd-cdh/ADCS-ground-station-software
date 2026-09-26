classdef CelestrakClient < TLESource
    % CelestrakClient  Polls CelesTrak's GP API for a TLE by NORAD ID.
    %   c = CelestrakClient(25544, PollIntervalSec=7200, CacheFile="iss.tle");
    %   tle = c.fetchLatestTLE();
    % Network access is throttled to one request per PollIntervalSec no matter
    % how often fetchLatestTLE()/hasNewData() are called (CelesTrak asks for
    % infrequent polling; >= 2 h is a safe default).
    properties (SetAccess = private)
        sourceUrl       (1,1) string
        noradId         (1,1) double
        pollIntervalSec (1,1) double
        cacheFile       (1,1) string
        timeoutSec      (1,1) double
        lastTLE         = []
        lastAttempt     = NaT
        newSinceLastRead (1,1) logical = false
    end

    methods
        function obj = CelestrakClient(noradId, options)
            arguments
                noradId (1,1) double
                options.SourceUrl       (1,1) string = "https://celestrak.org/NORAD/elements/gp.php"
                options.PollIntervalSec (1,1) double = 7200
                options.CacheFile       (1,1) string = ""
                options.TimeoutSec      (1,1) double = 15
            end
            obj.noradId         = noradId;
            obj.sourceUrl       = options.SourceUrl;
            obj.pollIntervalSec = options.PollIntervalSec;
            obj.cacheFile       = options.CacheFile;
            obj.timeoutSec      = options.TimeoutSec;
        end

        function tle = fetchLatestTLE(obj)
            if obj.isDue()
                obj.refresh();
            end
            if isempty(obj.lastTLE)
                error('CelestrakClient:noData', 'No TLE available yet (fetch failed and no cache).');
            end
            tle = obj.lastTLE;
            obj.newSinceLastRead = false;
        end

        function tf = hasNewData(obj)
            if obj.isDue()
                obj.refresh();
            end
            tf = obj.newSinceLastRead;
        end
    end

    methods (Access = private)
        function tf = isDue(obj)
            tf = isnat(obj.lastAttempt) || ...
                 seconds(datetime('now') - obj.lastAttempt) >= obj.pollIntervalSec;
        end

        function refresh(obj)
            obj.lastAttempt = datetime('now');   % also throttles retries after failure
            try
                opts = weboptions('Timeout', obj.timeoutSec);
                raw  = string(webread(obj.sourceUrl, 'CATNR', num2str(obj.noradId), ...
                                      'FORMAT', 'TLE', opts));
                tle = TLEData.parse(raw, "celestrak");
                if tle.satelliteId ~= obj.noradId
                    error('CelestrakClient:id', 'Returned NORAD ID does not match request.');
                end
                if strlength(obj.cacheFile) > 0
                    writelines(raw, obj.cacheFile);
                end
            catch ME
                warning('CelestrakClient:fetch', 'CelesTrak fetch failed: %s', ME.message);
                if isempty(obj.lastTLE) && strlength(obj.cacheFile) > 0 && isfile(obj.cacheFile)
                    try
                        tle = TLEData.parse(string(fileread(obj.cacheFile)), "cache");
                    catch
                        return
                    end
                else
                    return
                end
            end
            if tle.isNewerThan(obj.lastTLE)
                obj.lastTLE = tle;
                obj.newSinceLastRead = true;
            end
        end
    end
end
