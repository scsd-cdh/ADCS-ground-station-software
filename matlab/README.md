# MATLAB

Algorithm prototyping and development. This is the working source of truth for
new ADCS ground-station algorithms until they are ported to [`cpp/`](../cpp)
for production deployment.

Layout mirrors [`cpp/src/`](../cpp/src) by domain so a script's eventual C++
home is unambiguous:

- `comms/` — communication-window calculation, TLE retrieval
- `orbit-propagation/` — SGP4 propagation, orbit determination
- `telemetry/` — ADCS telemetry/attitude processing
- `common/` — shared utility functions used across domains
