classdef SpacecraftStub < handle
    % SpacecraftStub  Stand-in for the external Spacecraft (loopback testing).
    properties (SetAccess = private)
        receivedSequence = Command.empty
    end
    methods
        function receiveSequence(obj, seq)
            obj.receivedSequence = [obj.receivedSequence, seq(:).'];
        end
        function pkt = sendDownlink(~)
            pkt = TelemetryPacket(StateOfChargePercent = 80 + 5*rand, RawPayload = "stub");
        end
    end
end
