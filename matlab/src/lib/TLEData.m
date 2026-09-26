classdef TLEData
    % TLEData  Parsed two-line element set (value class).
    %   tle = TLEData.parse(rawText, "celestrak")
    properties (SetAccess = private)
        name        (1,1) string = ""
        line1       (1,1) string = ""
        line2       (1,1) string = ""
        satelliteId (1,1) double = NaN     % NORAD catalog number
        epoch                              % datetime, UTC
        source      (1,1) string = ""
    end

    methods
        function tf = isNewerThan(obj, other)
            tf = isempty(other) || obj.epoch > other.epoch;
        end
    end

    methods (Static)
        function obj = parse(raw, source)
            % Accepts 2-line or 3-line (name + 2 lines) TLE text.
            arguments
                raw    (1,1) string
                source (1,1) string = "unknown"
            end
            lines = strtrim(splitlines(raw));
            lines = lines(strlength(lines) > 0);
            i1 = find(startsWith(lines, "1 "), 1);
            i2 = find(startsWith(lines, "2 "), 1);
            if isempty(i1) || isempty(i2) || strlength(lines(i1)) ~= 69 || strlength(lines(i2)) ~= 69
                error('TLEData:format', 'Could not find two valid 69-character TLE lines.');
            end
            l1 = char(lines(i1));
            l2 = char(lines(i2));
            if ~TLEData.checksumOK(l1) || ~TLEData.checksumOK(l2)
                error('TLEData:checksum', 'TLE checksum failed.');
            end
            if ~strcmp(l1(3:7), l2(3:7))
                error('TLEData:mismatch', 'Satellite numbers differ between line 1 and line 2.');
            end

            obj = TLEData();
            obj.line1 = string(l1);
            obj.line2 = string(l2);
            obj.source = source;
            obj.satelliteId = str2double(l1(3:7));
            if i1 > 1
                obj.name = regexprep(lines(1), "^0 ", "");
            end
            yy  = str2double(l1(19:20));
            doy = str2double(l1(21:32));
            year = yy + 2000*(yy < 57) + 1900*(yy >= 57);   % NORAD pivot
            obj.epoch = datetime(year, 1, 1, 'TimeZone', 'UTC') + days(doy - 1);
        end

        function c = checksum(body)
            % Modulo-10 checksum over the first 68 characters.
            body = char(body);
            digits = body(body >= '0' & body <= '9') - '0';
            c = mod(sum(digits) + sum(body == '-'), 10);
        end

        function tf = checksumOK(line)
            line = char(line);
            tf = numel(line) == 69 && TLEData.checksum(line(1:68)) == (line(69) - '0');
        end

        function out = withChecksum(line)
            % Recompute the checksum digit (useful for hand-edited/test TLEs).
            c = char(line);
            c = c(1:68);
            out = string([c, num2str(TLEData.checksum(c))]);
        end
    end
end
