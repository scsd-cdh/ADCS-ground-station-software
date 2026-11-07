// Copyright (c) 2026, Space Concordia Contributors
// Two-line element sets and UTC time helpers.
#pragma once

#include <string>

namespace adcs {

struct Tle {
  std::string line1;
  std::string line2;
  int noradId;       // catalog number (Alpha-5 decoded, e.g. "A0001" -> 100001)
  double epochPosix;  // seconds since 1970-01-01 UTC
};

// Validates length, checksum and satellite numbers; throws std::runtime_error.
Tle parseTle(const std::string &line1, const std::string &line2);

// "2027-06-01T12:34:56.789Z"
std::string isoUtc(double posix);

// Accepts "2027-06-01T12:34:56Z" (optional fractional seconds, optional Z)
// or plain posix seconds ("1811939696.5"); throws std::runtime_error.
double parseUtc(const std::string &text);

}  // namespace adcs
