classdef CommunicationManager < handle
    % CommunicationManager  Owns the uplink queue and downlink buffer.
    %   Window opens  -> flush queued commands through the ground station and
    %                    start polling for telemetry.
    %   Window closes -> stop polling and drain the downlink buffer to the log.
    % Outside a window nothing is transmitted; commands wait in the queue.
    properties (SetAccess = private)
        uplinkQueue    = Command.empty
        sentCommands   = Command.empty
        downlinkBuffer = TelemetryPacket.empty
        lastTelemetry  = []
        windowOpen (1,1) logical = false
    end
    properties
        groundStation
        telemetryLog
        downlinkPollSec (1,1) double = 1
    end
    properties (Access = private)
        pollTimer = []
        listeners = {}
    end
    events
        TelemetryReceived    % OperatorUI listens and reads lastTelemetry
    end

    methods
        function obj = CommunicationManager(groundStation, telemetryLog)
            obj.groundStation = groundStation;
            obj.telemetryLog  = telemetryLog;
        end

        function attach(obj, calculator)
            % Subscribe to a CommunicationWindowCalculator's window events.
            obj.listeners{end+1} = addlistener(calculator, 'WindowOpened', @(~,~) obj.onWindowOpen());
            obj.listeners{end+1} = addlistener(calculator, 'WindowClosed', @(~,~) obj.onWindowClose());
        end

        function q = getUplinkQueue(obj)
            q = obj.uplinkQueue;
        end

        function updateUplinkQueue(obj, queue)
            arguments
                obj
                queue Command
            end
            if ~isempty(queue)
                [~, idx] = sort([queue.startTime]);
                queue = queue(idx);
            end
            obj.uplinkQueue = queue(:).';
            if obj.windowOpen
                obj.flushUplink();
            end
        end

        function pkt = receiveTelemetry(obj)
            % Pull pending packets from the ground station; returns the latest.
            pkt = TelemetryPacket.empty;
            if ~obj.windowOpen, return; end
            new = obj.groundStation.receive();
            if isempty(new), return; end
            obj.downlinkBuffer = [obj.downlinkBuffer, new(:).'];
            pkt = new(end);
            obj.lastTelemetry = pkt;
            notify(obj, 'TelemetryReceived');
        end

        function onWindowOpen(obj)
            if obj.windowOpen, return; end
            obj.windowOpen = true;
            obj.flushUplink();
            obj.pollTimer = timer('ExecutionMode', 'fixedSpacing', 'Period', obj.downlinkPollSec, ...
                                  'BusyMode', 'drop', 'TimerFcn', @(~,~) obj.receiveTelemetry());
            start(obj.pollTimer);
        end

        function onWindowClose(obj)
            if ~obj.windowOpen, return; end
            if ~isempty(obj.pollTimer) && isvalid(obj.pollTimer)
                stop(obj.pollTimer);
                delete(obj.pollTimer);
            end
            obj.pollTimer = [];
            obj.windowOpen = false;
            if ~isempty(obj.downlinkBuffer)
                obj.telemetryLog.append(obj.downlinkBuffer);
                obj.downlinkBuffer = TelemetryPacket.empty;
            end
        end

        function delete(obj)
            if ~isempty(obj.pollTimer) && isvalid(obj.pollTimer)
                stop(obj.pollTimer);
                delete(obj.pollTimer);
            end
        end
    end

    methods (Access = private)
        function flushUplink(obj)
            if isempty(obj.uplinkQueue), return; end
            if obj.groundStation.transmit(obj.uplinkQueue)
                obj.sentCommands = [obj.sentCommands, obj.uplinkQueue];
                obj.uplinkQueue = Command.empty;
            else
                warning('CommunicationManager:uplink', 'Uplink failed; commands kept in queue.');
            end
        end
    end
end
