function setup()
%SETUP  Add the MATLAB sources of this repo to the path. Run once per session.
%   src/core   codegen-safe functions (ported to C++)
%   src/lib    MATLAB-only classes (timers, network, events)
%   reference  toolbox-based cross-checks (Aerospace Toolbox)
%   codegen    C++ generation entry points
root = fileparts(mfilename('fullpath'));
addpath(fullfile(root, 'src', 'core'), fullfile(root, 'src', 'lib'), ...
        fullfile(root, 'reference'), fullfile(root, 'codegen'), ...
        fullfile(root, 'tests'));
end
