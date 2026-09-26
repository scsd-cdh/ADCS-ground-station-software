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
matlab/   Algorithm development (source of truth until deployed as C++)
cpp/      Production ground-station code (eventual deploy target)
ui/       Ground-station operator interface (not started)
docs/     Architecture notes and reference material
scripts/  Build/test/deploy tooling spanning multiple components
```

Inside `matlab/`, `src/core/` holds the codegen-safe functions (SGP4, TEME to
ECEF, communication-window search) that are converted to C++; `src/lib/` holds
MATLAB-only classes (TLE sources, services, timers, telemetry). See
[matlab/README.md](matlab/README.md) and [cpp/README.md](cpp/README.md).

## Install Guide

Requirements:

- MATLAB R2025b (earlier releases may work) with the Aerospace Toolbox
  (only for `reference/` cross-checks and the original satelliteScenario prototype)
- MATLAB Coder, to generate C++
- A [MATLAB Coder-supported C++ compiler](https://www.mathworks.com/support/requirements/supported-compilers.html)
  (e.g. Visual Studio, MinGW-w64, GCC) and CMake 3.20+, to build the C++

Setup:

```bash
git clone https://github.com/scsd-cdh/ADCS-ground-station-software.git
cd ADCS-ground-station-software
cp .env.example .env      # then edit; see "Configuration"
git config core.hooksPath .githooks   # enable the pre-commit check (needs Python 3)
```

## Configuration

Settings are read from environment variables, or from `.env` at the repo root
(real environment variables win). `.env` is gitignored; `.env.example` lists every
variable: NORAD ID, ground-station latitude/longitude/altitude, minimum
elevation, TLE source URL and poll interval, and data directory.

Runtime output (TLE caches, telemetry logs, demo results) goes to `<repo>/output/`
by default, for both the MATLAB and C++ code. It is gitignored and created on
demand; set `ADCS_DATA_DIR` (absolute, or relative to the repo root) to move it.

## Usage

Run once per MATLAB session:

```matlab
run matlab/setup.m
```

- Offline end-to-end check (SGP4 vectors, windows, toolbox cross-checks):
  `run_demo`
- Live operation with TLEs from CelesTrak (Ctrl+C to stop): `run_live`
- SGP4 test vectors only: `test_sgp4`

### Generating C++

The generated C++ and its MATLAB reference numbers must always match the MATLAB
sources. Whenever anything in `matlab/codegen/cpp_inputs.txt` changes
(`matlab/src/core`, the demo scenario, ...), regenerate both:

```matlab
run matlab/setup.m
build_cpp
export_cpp_fixture
```

This is enforced: `scripts/check_cpp_fixture.py` (pre-commit hook and CI) fails
if the committed fixture is stale, and CMake refuses to build stale generated code.


Following MathWorks' [class interface workflow](https://www.mathworks.com/help/coder/ug/generate-cpp-code-with-class-interface.html),
`build_cpp` turns `matlab/src/core` into one C++ class, `adcs::AdcsCore`, with
`tleToSatrec`, `sgp4init`, `sgp4prop`, `temeToEcef` and `computeWindowsCore`
as methods:

```matlab
run matlab/setup.m
build_cpp          % writes cpp/generated/adcs_core/ (gitignored)
```

It emits sources only (`GenCodeOnly`, so no MATLAB-side compiler is needed) and
relocates them with `packNGo` into `cpp/generated/adcs_core/portable/`. That
folder is self-contained (it includes MATLAB's `tmwtypes.h`), so building does
not need MATLAB.

Build and test the C++ with CMake:

```bash
cd cpp
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Run the demo in C++ (same output as MATLAB's `run_demo`):

```bash
./build/run_demo
```

Tests (`ctest`): `test_sgp4` runs the Vallado SGP4 vectors, and `test_windows`
checks TEME-to-ECEF and the window search against MATLAB results committed in
`cpp/tests/data/`. Details in [cpp/README.md](cpp/README.md).
