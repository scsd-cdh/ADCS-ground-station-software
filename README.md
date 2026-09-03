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
matlab/   Algorithm prototyping (source of truth until ported to C++)
cpp/      Production ground-station code (eventual deploy target)
ui/       Ground-station operator interface
docs/     Architecture notes and reference material
scripts/  Build/test/deploy tooling spanning multiple components
```

`matlab/` and `cpp/src/` are organized by domain (`comms/`, `orbit-propagation/`,
`telemetry/`, `common/`) with matching subfolder names, so a MATLAB script's
eventual C++ home is unambiguous when it's ported over. See each folder's
README for details.

## Usage

## Install Guide
