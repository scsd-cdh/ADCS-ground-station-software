// Copyright (c) 2026, Space Concordia Contributors
// The ADCS function the ground-station (SDR) software calls once per contact.
//
// At the start of each contact the ground station uplinks the spacecraft's
// position before anything else; the spacecraft has no GPS and uses it to
// correct its onboard estimate. This pulls the latest TLE from CelesTrak
// (NORAD data), propagates it with SGP4 to the contact time and returns the
// state for that packet. Pass prediction and contact sequencing belong to
// the ground-station software (Gpredict + SDR), not to ADCS.
#pragma once

#include <filesystem>
#include <functional>
#include <string>

#include "adcs/tle.h"

namespace adcs {

enum class TleSource {
  CelesTrak,  // pulled during this call
  Cache,     // last successful pull (rate limit reached, or the pull failed)
  File,      // ADCS_TLE_FILE override
};

struct ContactState {
  double timePosix;  // time the state applies to (the contact time), UTC
  double rTemeKm[3];  // SGP4 output frame (TEME)
  double vTemeKmS[3];
  double rEcefKm[3];  // Earth-fixed (GMST rotation; polar motion and UT1-UTC ignored)
  double vEcefKmS[3];
  Tle tle;           // the TLE that was propagated
  // timePosix - TLE epoch. SGP4 has no covariance; this is the accuracy proxy
  // (about 1 km at epoch, growing by a few km per day).
  double tleAgeSec;
  TleSource source;
  // Non-empty when something degraded but a state was still produced
  // (CelesTrak pull failed and the cache was used, TLE older than staleTleSec).
  std::string warning;
};

struct ContactConfig {
  int noradId = 0;                                                  // ADCS_NORAD_ID (required)
  std::string celestrakUrl = "https://celestrak.org/NORAD/elements/gp.php";  // ADCS_TLE_SOURCE_URL
  // ADCS_TLE_FILE: use this TLE instead of CelesTrak (first ~48 h after
  // deployment before NORAD publishes one, Space-Track or other sources).
  std::filesystem::path tleFile;
  std::filesystem::path cacheDir;  // default <ADCS_DATA_DIR>/tle

  // CelesTrak usage plan: at most 5 pulls per day, at least 90 min apart.
  // Every attempt counts, failed ones too. Enforced from a log in cacheDir,
  // so it holds across processes and restarts. Exceeding it risks an IP ban.
  double minPullIntervalSec = 90 * 60;
  int maxPullsPerDay = 5;
  int timeoutSec = 10;        // a pass lasts minutes; never block the contact on the network
  double staleTleSec = 86400;  // warn when |tleAgeSec| exceeds this

  // Replaceable for tests. Defaults: the curl program, the system clock.
  std::function<std::string(const std::string &url, int timeoutSec)> httpGet;
  std::function<double()> now;

  // Reads the ADCS_* environment variables (see .env.example).
  static ContactConfig fromEnv();
};

// Spacecraft state at contactPosix (seconds since 1970-01-01 UTC).
// Pulls from CelesTrak when the usage limits allow, otherwise uses the cached
// TLE. Throws std::runtime_error only when no usable TLE exists at all.
ContactState contactState(double contactPosix, const ContactConfig &config);

const char *toString(TleSource source);

}  // namespace adcs
