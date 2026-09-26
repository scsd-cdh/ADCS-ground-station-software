% run_demo  Offline end-to-end test using a local sample TLE and pinned time.
clear; clc;
run(fullfile(fileparts(mfilename('fullpath')), '..', 'setup.m'));
cfg = loadConfig();
test_sgp4;                                   % validate the SGP4 port first

gsLLA = [cfg.gsLatDeg, cfg.gsLonDeg, cfg.gsAltM];
l1 = TLEData.withChecksum("1 25544U 98067A   24001.50000000  .00016717  00000-0  10270-3 0  9999");
l2 = TLEData.withChecksum("2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.50377579 26629");
tleFile = fullfile(cfg.dataDir, "demo.tle");
writelines(["ISS"; l1; l2], tleFile);
tle0 = TLEData.parse(string(fileread(tleFile)), "file");
fprintf('TLE epoch: %s\n', string(tle0.epoch));

src  = FileTLESource(tleFile);
prop = SGP4Propagator();
svc  = PropagationService(src, prop, NowFcn=@() tle0.epoch);   % pin "now" (TLE is old)
calc = CommunicationWindowCalculator(svc, gsLLA(1), gsLLA(2), gsLLA(3), ...
            MinElevationDeg=cfg.minElevationDeg, HorizonHours=24, StepSec=30);

sc  = SpacecraftStub();
gs  = GroundStation(gsLLA(1), gsLLA(2), gsLLA(3), sc);
log = TelemetryLog(fullfile(cfg.dataDir, "scfreyr_telemetry.jsonl"));
mgr = CommunicationManager(gs, log);
mgr.attach(calc);

svc.checkTLE();          % applies the TLE -> TLEUpdated -> calc.recompute()
T = calc.windowTable()

% Cross-check 1: native SGP4 vs Aerospace Toolbox SGP4 (needs the toolbox)
ref = SGP4PropagatorToolbox(30);
ref.updateInitialConditions(tle0);
tt = (tle0.epoch : minutes(10) : tle0.epoch + hours(24))';
pa = prop.propagate(tt);   pb = ref.propagate(tt);
fprintf('Native vs toolbox SGP4, max position difference over 24 h: %.1f m\n', ...
        max(vecnorm(pa.position - pb.position, 2, 2)));

% Cross-check 2: windows vs satelliteScenario access()
Tref = commWindowsScenario(tleFile, gsLLA, tle0.epoch, 24, cfg.minElevationDeg, 30)

% Manager behaviour: queue a command, simulate one pass
mgr.updateUplinkQueue(Command("C1", OperationMode.Imaging, tle0.epoch + hours(2), tle0.epoch + hours(2.1)));
mgr.onWindowOpen();
pause(2.5);
mgr.onWindowClose();
fprintf('Sent commands: %d, log: %s\n', numel(mgr.sentCommands), log.storagePath);
