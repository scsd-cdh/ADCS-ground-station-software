# MATLAB

Algorithm development. Source of truth for the ground-station algorithms until
they are deployed as C++ from [`cpp/`](../cpp).

```text
setup.m          add this tree to the MATLAB path (run once per session)
src/core/        codegen-safe functions -> converted to C++ (SGP4, TEME->ECEF, window search)
src/lib/         MATLAB-only classes: TLE sources, services, timers, events, telemetry, loadConfig
reference/       Aerospace Toolbox cross-checks (not converted)
scripts/         run_demo (offline end-to-end), run_live (CelesTrak)
tests/           test_sgp4 (Vallado vectors); export_cpp_fixture + test_cpp_fixture (C++ reference data)
codegen/         build_cpp.m (MATLAB Coder entry point), cpp_inputs.txt (files that require regeneration)
communication-window/  original satelliteScenario prototype
```

## Rule for converting to C++

Only `src/core/` is converted. Keep it free of classes, timers, network, file
I/O and toolbox calls, and keep the `%#codegen` directive in each file. Anything
that needs those goes in `src/lib/` and calls into `core`.

## Generating C++

Requires MATLAB Coder. `build_cpp` follows MathWorks'
[Generate Class Interface in C++ Code](https://www.mathworks.com/help/coder/ug/generate-cpp-code-with-class-interface.html):
`TargetLang = 'C++'`, `CppInterfaceStyle = 'Methods'`, one `codegen` call with
all entry points, producing class `adcs::AdcsCore`.

```matlab
run matlab/setup.m
build_cpp          % writes cpp/generated/adcs_core/ and .../portable/ (gitignored)
```

The output is relocated with `packNGo` so it builds without MATLAB (see
[`cpp/README.md`](../cpp/README.md)). Generated code is not committed; hand-written C++ in `cpp/src/` wraps it. To add
a function, put it in `src/core/` and add it (with `-args` types) to `build_cpp.m`.

## Configuration

Copy `.env.example` to `.env` at the repo root. Real environment variables
override `.env`; see `src/lib/loadConfig.m`. Output (TLE caches, logs) defaults to
`<repo>/output/`, shared with the C++ code.
