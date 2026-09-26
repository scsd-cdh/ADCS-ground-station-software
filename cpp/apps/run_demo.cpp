// C++ counterpart of matlab/scripts/run_demo.m: fixed sample TLE, "now" pinned to the
// TLE epoch, 24 h look-ahead, 30 s step, 10 deg mask. Prints the pass table.
// Station and mask come from the same ADCS_* environment variables as the MATLAB side.
#include "adcs/config.h"
#include "adcs/window_predictor.h"

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>

namespace {
double env(const char *name, double fallback) {
  const char *v = std::getenv(name);
  return (v && *v) ? std::atof(v) : fallback;
}
} // namespace

int main() {
  try {
    const adcs::GroundStation gs{env("ADCS_GS_LAT_DEG", 45.45810), env("ADCS_GS_LON_DEG", -73.64031),
                                 env("ADCS_GS_ALT_M", 50)};
    adcs::WindowSettings settings;
    settings.minElevationDeg = env("ADCS_MIN_ELEVATION_DEG", 10);

    // Same lines as run_demo.m (ISS, checksums recomputed).
    const adcs::Tle tle = adcs::parseTle("1 25544U 98067A   24001.50000000  .00016717  00000-0  10270-3 0  9997",
                                         "2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.50377579 26624");
    std::printf("TLE epoch: %s UTC\n", adcs::formatUtc(tle.epochPosix).c_str());

    adcs::AdcsCore core;
    const auto windows = adcs::predictWindows(core, tle, gs, tle.epochPosix, settings);

    std::printf("\n%-22s %-22s %12s %12s %12s\n", "Start (UTC)", "End (UTC)", "DurationSec", "MaxElevDeg",
                "UsableSec");
    for (const auto &w : windows) {
      std::printf("%-22s %-22s %12.2f %12.3f %12.2f\n", adcs::formatUtc(w.startPosix).c_str(),
                  adcs::formatUtc(w.endPosix).c_str(), w.durationSec(), w.maxElevationDeg,
                  0.6 * w.durationSec()); // 60 % usable-sky planning assumption, as in MATLAB
    }
    std::printf("\n%zu windows\n", windows.size());

    // Same output folder as the MATLAB demo (ADCS_DATA_DIR, default <repo>/output).
    const auto csv = adcs::dataDir() / "cpp_demo_windows.csv";
    std::ofstream out(csv);
    out << "start_utc,end_utc,duration_sec,max_elevation_deg\n";
    for (const auto &w : windows)
      out << adcs::formatUtc(w.startPosix) << ',' << adcs::formatUtc(w.endPosix) << ',' << w.durationSec() << ','
          << w.maxElevationDeg << '\n';
    std::printf("wrote %s\n", csv.string().c_str());
    return 0;
  } catch (const std::exception &e) {
    std::fprintf(stderr, "error: %s\n", e.what());
    return 1;
  }
}
