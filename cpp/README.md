# C++

Production ground-station code. This is the eventual deploy target: algorithms
start life in [`matlab/`](../matlab) and are ported here once validated.

Layout mirrors `matlab/` by domain so a MATLAB script's C++ port has an obvious
home:

- `src/comms/` — communication-window calculation, TLE retrieval
- `src/orbit-propagation/` — SGP4 propagation, orbit determination
- `src/telemetry/` — ADCS telemetry/attitude processing
- `src/common/` — shared utility code used across domains
- `include/adcs/` — public headers, namespaced under `adcs::`
- `tests/` — unit tests

Built with CMake; see [`CMakeLists.txt`](CMakeLists.txt).
