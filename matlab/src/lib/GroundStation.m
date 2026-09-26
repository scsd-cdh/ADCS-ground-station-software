classdef GroundStation < handle
    % GroundStation  Radio front end. Swap the two transport methods for the real
    % transceiver interface once it is defined.
    properties (SetAccess = private)
        latitude  (1,1) double
        longitude (1,1) double
        altitude  (1,1) double
    end
    properties
        spacecraft   % anything with receiveSequence(seq) and sendDownlink()
    end
    methods
        function obj = GroundStation(lat, lon, alt, spacecraft)
            obj.latitude = lat;  obj.longitude = lon;  obj.altitude = alt;
            if nargin > 3, obj.spacecraft = spacecraft; end
        end
        function ok = transmit(obj, commands)
            ok = ~isempty(obj.spacecraft);
            if ok, obj.spacecraft.receiveSequence(commands); end
        end
        function pkts = receive(obj)
            pkts = TelemetryPacket.empty;
            if isempty(obj.spacecraft), return; end
            p = obj.spacecraft.sendDownlink();
            if ~isempty(p), pkts = p; end
        end
    end
end
