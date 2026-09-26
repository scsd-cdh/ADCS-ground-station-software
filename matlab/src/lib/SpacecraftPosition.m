classdef SpacecraftPosition
    % SpacecraftPosition  Sampled state, ECEF (WGS84), SI units.
    properties (SetAccess = private)
        position  (:,3) double = zeros(0,3)   % m
        velocity  (:,3) double = zeros(0,3)   % m/s
        timestamp (:,1) datetime              % UTC
    end
    methods
        function obj = SpacecraftPosition(position, velocity, timestamp)
            obj.position  = position;
            obj.velocity  = velocity;
            obj.timestamp = timestamp;
        end
    end
end
