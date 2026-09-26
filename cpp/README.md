# C++

Production ground-station code. This is the eventual deploy target: algorithms
start life in [`matlab/`](../matlab) and are converted here once validated.

```text
generated/adcs_core/portable/   C++ from MATLAB Coder (gitignored; run matlab/codegen/build_cpp.m)
include/adcs/, src/             hand-written layer (adcs_windows): TLE parsing, propagation, pass prediction, config (adcs::dataDir)
apps/run_demo.cpp               C++ counterpart of matlab/scripts/run_demo.m
tests/                          ctest: test_sgp4 (Vallado vectors), test_windows (vs MATLAB reference)
tests/data/demo_reference.txt   MATLAB results the C++ must reproduce (committed)
CMakeLists.txt                  adcs_core (generated) + adcs_windows + run_demo + tests
```

## Build

Requires CMake 3.20+ and a C++17 compiler with OpenMP (the generated class uses
OpenMP locks to make its methods thread-safe). The generated code must exist
first: in MATLAB, `run matlab/setup.m; build_cpp`.

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Run the demo (same TLE, station and 10 deg mask as `run_demo.m`; station and mask
follow the `ADCS_*` environment variables):

```bash
./build/run_demo        # build
un_demo.exe on Windows
```

`run_demo` writes `cpp_demo_windows.csv` to `<repo>/output/` (or `ADCS_DATA_DIR`), the
same folder the MATLAB demo uses; the repo root is fixed at CMake configure time.

Use `adcs::predictWindows` from `adcs/window_predictor.h`, or call
`adcs::AdcsCore` directly (`tleToSatrec`, `sgp4init`, `sgp4prop`, `temeToEcef`,
`computeWindowsCore`).

## Keeping C++ and MATLAB in sync

`test_windows` compares the C++ pipeline against numbers MATLAB produced for the
demo scenario (6 passes over 24 h, plus ECEF samples). After changing anything
listed in [`matlab/codegen/cpp_inputs.txt`](../matlab/codegen/cpp_inputs.txt)
(`matlab/src/core`, the demo scenario, ...), regenerate both:

```matlab
run matlab/setup.m
build_cpp
export_cpp_fixture   % rewrites cpp/tests/data/demo_reference.txt
```

Enforced by a SHA-256 stamp over those files:

| Where | Check |
| --- | --- |
| `.githooks/pre-commit`, CI (`cpp-fixture.yml`) | `scripts/check_cpp_fixture.py`: fixture stamp equals current sources (no MATLAB needed) |
| `cpp/CMakeLists.txt` | generated code's `inputs.sha256` equals the fixture stamp, else configure fails |
| MATLAB `test_cpp_fixture` | re-runs the scenario and compares with the committed numbers (catches hand edits) |

Enable the hook once per clone: `git config core.hooksPath .githooks`.
