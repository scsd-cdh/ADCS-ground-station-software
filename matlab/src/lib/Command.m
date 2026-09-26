classdef Command
    properties (SetAccess = private)
        id            (1,1) string
        mode          (1,1) OperationMode = OperationMode.Standby
        startTime     (1,1) datetime
        endTime       (1,1) datetime
        exitCondition (1,1) string = ""
        parameters    (1,1) string = ""
    end
    methods
        function obj = Command(id, mode, startTime, endTime, options)
            arguments
                id        (1,1) string
                mode      (1,1) OperationMode
                startTime (1,1) datetime
                endTime   (1,1) datetime
                options.ExitCondition (1,1) string = ""
                options.Parameters    (1,1) string = ""
            end
            if endTime < startTime
                error('Command:times', 'endTime must not precede startTime.');
            end
            obj.id = id;  obj.mode = mode;
            obj.startTime = startTime;  obj.endTime = endTime;
            obj.exitCondition = options.ExitCondition;
            obj.parameters = options.Parameters;
        end
    end
end
