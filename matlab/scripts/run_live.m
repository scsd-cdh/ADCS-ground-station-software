% run_live  Continuous operation with live TLEs from CelesTrak.
%   * SGP4 is propagated to "now" every second.
%   * The TLE source is checked every minute; the network is only hit every
%     PollIntervalSec (CelesTrak refreshes about every 2 h, don't go much lower).
%   * The moment a newer TLE appears, SGP4's initial conditions are replaced and
%     the pass list is recomputed.
% MATLAB timers only fire while MATLAB is idle, hence the pause loop below.
clear; clc;
run(fullfile(fileparts(mfilename('fullpath')), '..', 'setup.m'));
cfg     = loadConfig();
gsLLA   = [cfg.gsLatDeg, cfg.gsLonDeg, cfg.gsAltM];

src  = CelestrakClient(cfg.noradId, SourceUrl=cfg.tleSourceUrl, PollIntervalSec=cfg.tlePollSec, ...
                     CacheFile=fullfile(cfg.dataDir, "live.tle"));
prop = SGP4Propagator();
svc  = PropagationService(src, prop, PropagationPeriodSec=1, TleCheckPeriodSec=60);
calc = CommunicationWindowCalculator(svc, gsLLA(1), gsLLA(2), gsLLA(3), MinElevationDeg=cfg.minElevationDeg);

gs  = GroundStation(gsLLA(1), gsLLA(2), gsLLA(3), SpacecraftStub());
mgr = CommunicationManager(gs, TelemetryLog(fullfile(cfg.dataDir, "live_telemetry.jsonl")));
mgr.attach(calc);

L = { addlistener(svc,  'TLEUpdated',   @(~,~) fprintf('[%s] NEW TLE applied, epoch %s\n', string(datetime('now')), string(svc.currentTLE.epoch)));
      addlistener(calc, 'WindowOpened', @(~,~) fprintf('[%s] WINDOW OPEN\n',  string(datetime('now'))));
      addlistener(calc, 'WindowClosed', @(~,~) fprintf('[%s] WINDOW CLOSE\n', string(datetime('now')))) };

svc.startService();
calc.startPolling();
disp(calc.windowTable());

cleanupObj = onCleanup(@() stopAll(calc, svc));   % Ctrl+C stops the timers
disp('Running. Press Ctrl+C to stop.');
while true
    pause(10);
    if ~isempty(svc.latest)
        r = svc.latest.position(end, :) / 1000;
        fprintf('%s  ECEF [km]: %9.1f %9.1f %9.1f   |r| = %.1f km   TLE age %.2f d\n', ...
            string(svc.latest.timestamp(end)), r, norm(r), svc.tleAgeDays());
    end
end

function stopAll(calc, svc)
    calc.stopPolling();
    svc.stopService();
end
