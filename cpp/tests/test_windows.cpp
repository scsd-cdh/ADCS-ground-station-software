// Checks the generated temeToEcef / computeWindowsCore (through adcs/window_predictor.h)
// against numbers exported from MATLAB by matlab/tests/export_cpp_fixture.m.
#include "adcs/window_predictor.h"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>

namespace {
int failures = 0;
void check(bool ok, const char *what, double got, double want) {
  if (!ok) {
    std::printf("FAIL %s: got %.9g, want %.9g\n", what, got, want);
    ++failures;
  }
}
struct Ecef {
  double t, x, y, z;
};
} // namespace

int main() {
  std::ifstream in(std::string(ADCS_TEST_DATA_DIR) + "/demo_reference.txt");
  if (!in) {
    std::puts("cannot open demo_reference.txt");
    return 2;
  }

  std::string l1, l2;
  adcs::GroundStation gs{};
  adcs::WindowSettings s;
  double epoch = 0;
  std::vector<adcs::Window> want;
  std::vector<Ecef> ecef;

  for (std::string line; std::getline(in, line);) {
    std::istringstream ss(line);
    std::string key;
    ss >> key;
    if (key == "tle1") l1 = line.substr(5);
    else if (key == "tle2") l2 = line.substr(5);
    else if (key == "epoch_posix") ss >> epoch;
    else if (key == "gs") ss >> gs.latDeg >> gs.lonDeg >> gs.altM;
    else if (key == "params") ss >> s.minElevationDeg >> s.horizonHours >> s.stepSec >> s.peakRefineStepSec;
    else if (key == "window") {
      adcs::Window w{};
      ss >> w.startPosix >> w.endPosix >> w.maxElevationDeg;
      want.push_back(w);
    } else if (key == "ecef") {
      Ecef e{};
      ss >> e.t >> e.x >> e.y >> e.z;
      ecef.push_back(e);
    }
  }

  adcs::AdcsCore core;
  const adcs::Tle tle = adcs::parseTle(l1, l2);
  check(std::fabs(tle.epochPosix - epoch) < 1e-3, "TLE epoch", tle.epochPosix, epoch);

  // temeToEcef: SGP4 + rotation to ECEF at sample times.
  std::vector<double> times;
  for (const auto &e : ecef) times.push_back(e.t);
  const auto pos = adcs::propagateEcef(core, tle, times);
  for (size_t i = 0; i < ecef.size(); ++i) {
    const double d = std::sqrt(std::pow(pos[3 * i] - ecef[i].x, 2) + std::pow(pos[3 * i + 1] - ecef[i].y, 2) +
                               std::pow(pos[3 * i + 2] - ecef[i].z, 2));
    check(d < 1e-3, "ECEF position error [m]", d, 0.0);
  }

  // computeWindowsCore: full pass list incl. peak refinement.
  const auto got = adcs::predictWindows(core, tle, gs, tle.epochPosix, s);
  check(got.size() == want.size(), "window count", static_cast<double>(got.size()), static_cast<double>(want.size()));
  for (size_t i = 0; i < got.size() && i < want.size(); ++i) {
    check(std::fabs(got[i].startPosix - want[i].startPosix) < 0.05, "window start", got[i].startPosix,
          want[i].startPosix);
    check(std::fabs(got[i].endPosix - want[i].endPosix) < 0.05, "window end", got[i].endPosix, want[i].endPosix);
    check(std::fabs(got[i].maxElevationDeg - want[i].maxElevationDeg) < 1e-3, "max elevation",
          got[i].maxElevationDeg, want[i].maxElevationDeg);
  }

  if (failures == 0)
    std::printf("test_windows: all checks passed (%zu windows, %zu ECEF samples)\n", want.size(), ecef.size());
  return failures == 0 ? 0 : 1;
}
