// Copyright (c) 2026, Space Concordia Contributors
// Command-line wrapper around adcs::contactState for the ground-station (SDR)
// software, which is likely Python: run once per contact, read JSON from stdout.
//
//   contact_state                        state now
//   contact_state 2027-06-01T12:00:00Z   state at a given UTC time (or posix seconds)
//
// Settings come from the ADCS_* environment variables (see .env.example).
// Exit code 0 with JSON on stdout; 1 with the error on stderr.
#include "adcs/contact_state.h"

#include <chrono>
#include <cstdio>
#include <exception>
#include <string>

namespace {

std::string jsonString(const std::string &s) {
  std::string out = "\"";
  for (const char c : s) {
    if (c == '"' || c == '\\') out += '\\';
    if (static_cast<unsigned char>(c) < 0x20) {
      char buf[8];
      std::snprintf(buf, sizeof buf, "\\u%04x", c);
      out += buf;
    } else {
      out += c;
    }
  }
  return out + '"';
}

std::string jsonVec(const double v[3]) {
  char buf[96];
  std::snprintf(buf, sizeof buf, "[%.9f, %.9f, %.9f]", v[0], v[1], v[2]);
  return buf;
}

}  // namespace

int main(int argc, char **argv) {
  try {
    if (argc > 2) {
      std::fprintf(stderr, "usage: contact_state [UTC time, e.g. 2027-06-01T12:00:00Z]\n");
      return 1;
    }
    const double t = argc == 2 ? adcs::parseUtc(argv[1])
                               : std::chrono::duration<double>(
                                     std::chrono::system_clock::now().time_since_epoch())
                                     .count();
    const adcs::ContactState s = adcs::contactState(t, adcs::ContactConfig::fromEnv());

    std::printf("{\n");
    std::printf("  \"norad_id\": %d,\n", s.tle.noradId);
    std::printf("  \"time_utc\": %s,\n", jsonString(adcs::isoUtc(s.timePosix)).c_str());
    std::printf("  \"time_posix\": %.3f,\n", s.timePosix);
    std::printf("  \"r_teme_km\": %s,\n", jsonVec(s.rTemeKm).c_str());
    std::printf("  \"v_teme_km_s\": %s,\n", jsonVec(s.vTemeKmS).c_str());
    std::printf("  \"r_ecef_km\": %s,\n", jsonVec(s.rEcefKm).c_str());
    std::printf("  \"v_ecef_km_s\": %s,\n", jsonVec(s.vEcefKmS).c_str());
    std::printf("  \"tle_line1\": %s,\n", jsonString(s.tle.line1).c_str());
    std::printf("  \"tle_line2\": %s,\n", jsonString(s.tle.line2).c_str());
    std::printf("  \"tle_epoch_utc\": %s,\n", jsonString(adcs::isoUtc(s.tle.epochPosix)).c_str());
    std::printf("  \"tle_age_sec\": %.3f,\n", s.tleAgeSec);
    std::printf("  \"tle_source\": %s,\n", jsonString(adcs::toString(s.source)).c_str());
    std::printf("  \"warning\": %s\n", jsonString(s.warning).c_str());
    std::printf("}\n");
    return 0;
  } catch (const std::exception &e) {
    std::fprintf(stderr, "error: %s\n", e.what());
    return 1;
  }
}
