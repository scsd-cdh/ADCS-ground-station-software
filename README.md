# ADCS Ground Station Software

## About

This monorepo contains the ground-station software for:

- monitoring and controlling the CubeSat
- ADCS telemetry and command interfaces
- communication-window calculation
- SGP4 orbit propagation
- combining real-time TLE retrieval

It provides a view for sending control commands to the spacecraft, receiving and displaying ADCS status, attitude, and other information that the ADCS subsystem is required to produce.

## Repository structure

```text
cpp/   Ground-station code (C++17): SGP4 propagation, communication windows
ui/    Ground-station operator interface (not started)
```

## Build and run

Requirements: CMake 3.20+ and a C++17 compiler with OpenMP (GCC/MinGW, Clang or Visual Studio).

```bash
git clone https://github.com/scsd-cdh/ADCS-ground-station-software.git
cd ADCS-ground-station-software/cpp

cmake -B build              # configure (once)
cmake --build build         # build
ctest --test-dir build      # run the tests
./build/run_demo            # predict passes for a sample TLE
```

With Visual Studio, build output goes to `build/Release/` (or `build/Debug/`):
use `cmake --build build --config Release` and `ctest --test-dir build -C Release`.

## Configuration

Optional. `run_demo` reads these environment variables and falls back to defaults
(the Loyola ground station, 10 deg mask); `.env.example` lists them all.

| Variable | Meaning |
| --- | --- |
| `ADCS_GS_LAT_DEG`, `ADCS_GS_LON_DEG`, `ADCS_GS_ALT_M` | Ground-station location |
| `ADCS_MIN_ELEVATION_DEG` | Minimum elevation for a communication window |
| `ADCS_DATA_DIR` | Output folder (default `<repo>/output`, gitignored) |
