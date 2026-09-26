// Runtime configuration shared with the MATLAB side (matlab/src/lib/loadConfig.m).
#pragma once

#include <filesystem>

namespace adcs {

// Folder for TLE caches, telemetry logs and demo results (created if missing).
// ADCS_DATA_DIR, if set; relative paths are relative to the repo root.
// Default: <repo>/output. The repo root is fixed at CMake configure time.
std::filesystem::path dataDir();

} // namespace adcs
