// Hand-written layer over the MATLAB-Coder-generated adcs::AdcsCore.
// Mirrors matlab/src/lib/SGP4Propagator.m and CommunicationWindowCalculator.m
// (propagate -> TEME->ECEF -> elevation-mask windows -> fine peak refinement).
#pragma once

#include "AdcsCore.h"
#include "tleToSatrec_types.h"

#include <string>
#include <vector>

namespace adcs {

struct GroundStation {
  double latDeg;
  double lonDeg;
  double altM;
};

struct Tle {
  std::string line1;
  std::string line2;
  double epochPosix; // seconds since 1970-01-01 UTC
};

struct Window {
  double startPosix;
  double endPosix;
  double maxElevationDeg;
  bool startTruncated; // window touches the first sample of the horizon
  bool endTruncated;   // window touches the last sample of the horizon
  double durationSec() const { return endPosix - startPosix; }
};

struct WindowSettings {
  double minElevationDeg = 10;
  double horizonHours = 24;
  double stepSec = 30;
  double peakRefineStepSec = 1; // 0 disables peak refinement
};

// Validates length, checksum and satellite numbers; throws std::runtime_error.
Tle parseTle(const std::string &line1, const std::string &line2);

// ECEF position [m] of the spacecraft at each posix time (SGP4 + TEME->ECEF).
// Returns Nx3, row-major: out[3*i + {0,1,2}].
std::vector<double> propagateEcef(AdcsCore &core, const Tle &tle,
                                  const std::vector<double> &posixTimes);

// Communication windows in [startPosix, startPosix + horizon].
std::vector<Window> predictWindows(AdcsCore &core, const Tle &tle,
                                   const GroundStation &gs, double startPosix,
                                   const WindowSettings &settings);

// "01-Jan-2024 16:25:19" (UTC, fractional seconds truncated).
std::string formatUtc(double posix);

} // namespace adcs
