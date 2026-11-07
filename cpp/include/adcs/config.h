// Copyright (c) 2026, Space Concordia Contributors
// Runtime configuration (ADCS_* environment variables, see .env.example).
#pragma once

#include <filesystem>

namespace adcs {

// Folder for the TLE cache and CelesTrak pull log (created if missing).
// ADCS_DATA_DIR, if set; relative paths are relative to the repo root.
// Default: <repo>/output. The repo root is fixed at CMake configure time.
std::filesystem::path dataDir();

}  // namespace adcs
