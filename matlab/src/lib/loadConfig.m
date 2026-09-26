function cfg = loadConfig()
%LOADCONFIG  Runtime configuration from environment variables.
%   Precedence: process environment > <repo>/.env file > defaults below.
%   See .env.example at the repo root for the variable list.
root = fileparts(fileparts(fileparts(fileparts(mfilename('fullpath')))));
file = readDotEnv(fullfile(root, '.env'));

cfg.noradId         = num('ADCS_NORAD_ID',            25544, file);   % ISS until SC-FREYR is catalogued
cfg.gsLatDeg        = num('ADCS_GS_LAT_DEG',          45.45810, file);
cfg.gsLonDeg        = num('ADCS_GS_LON_DEG',          -73.64031, file);
cfg.gsAltM          = num('ADCS_GS_ALT_M',            50, file);      % placeholder altitude
cfg.minElevationDeg = num('ADCS_MIN_ELEVATION_DEG',   10, file);
cfg.tleSourceUrl    = str('ADCS_TLE_SOURCE_URL',      "https://celestrak.org/NORAD/elements/gp.php", file);
cfg.tlePollSec      = num('ADCS_TLE_POLL_INTERVAL_SEC', 7200, file);
cfg.dataDir         = resolveDataDir(str('ADCS_DATA_DIR', "output", file), root);   % default <repo>/output
end

function d = resolveDataDir(d, root)
% Relative paths are relative to the repo root. The folder is created if missing.
d = char(d);
if ~(startsWith(d, filesep) || ~isempty(regexp(d, '^[A-Za-z]:', 'once')))
    d = fullfile(root, d);
end
if ~isfolder(d), mkdir(d); end
d = string(d);
end

function v = str(name, default, file)
v = string(getenv(name));
if strlength(v) == 0 && isfield(file, name), v = string(file.(name)); end
if strlength(v) == 0, v = string(default); end
end

function v = num(name, default, file)
s = str(name, "", file);
if strlength(s) == 0, v = default; return; end
v = str2double(s);
if isnan(v), error('loadConfig:badValue', '%s must be numeric, got "%s".', name, s); end
end

function s = readDotEnv(path)
s = struct();
if ~isfile(path), return; end
for line = reshape(splitlines(string(fileread(path))), 1, [])
    line = strtrim(line);
    if strlength(line) == 0 || startsWith(line, "#"), continue; end
    kv = split(line, "=");
    if numel(kv) < 2, continue; end
    key = strtrim(kv(1));
    s.(key) = strip(strtrim(strjoin(kv(2:end), "=")), 'both', '"');
end
end
