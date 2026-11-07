// Copyright (c) 2026, Space Concordia Contributors
#include "adcs/tle.h"

#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <string>

namespace adcs {
namespace {

struct CivilDate {
  int64_t year;
  unsigned month;  // 1-12
  unsigned day;    // 1-31
};

// Days since 1970-01-01 for a civil date (H. Hinnant's algorithm).
int64_t daysFromCivil(int64_t y, unsigned m, unsigned d) {
  y -= m <= 2;
  const int64_t era = (y >= 0 ? y : y - 399) / 400;
  const unsigned yoe = static_cast<unsigned>(y - era * 400);
  const unsigned doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1;
  const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + static_cast<int64_t>(doe) - 719468;
}

CivilDate civilFromDays(int64_t z) {
  z += 719468;
  const int64_t era = (z >= 0 ? z : z - 146096) / 146097;
  const unsigned doe = static_cast<unsigned>(z - era * 146097);
  const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  const unsigned mp = (5 * doy + 2) / 153;
  CivilDate date;
  date.day = doy - (153 * mp + 2) / 5 + 1;
  date.month = mp < 10 ? mp + 3 : mp - 9;
  date.year = static_cast<int64_t>(yoe) + era * 400 + (date.month <= 2);
  return date;
}

bool checksumOk(const std::string &line) {
  int sum = 0;
  for (int i = 0; i < 68; ++i) {
    if (line[i] >= '0' && line[i] <= '9') {
      sum += line[i] - '0';
    } else if (line[i] == '-') {
      sum += 1;
    }
  }
  return sum % 10 == line[68] - '0';
}

// Columns 3-7. Alpha-5 (catalog numbers above 99999): leading letter A-Z,
// skipping I and O, stands for 10-33.
int catalogNumber(const std::string &field) {
  const char c = field[0];
  int lead;
  if (c == ' ' || (c >= '0' && c <= '9')) {
    lead = c == ' ' ? 0 : c - '0';
  } else if (c >= 'A' && c <= 'Z' && c != 'I' && c != 'O') {
    lead = 10 + (c - 'A') - (c > 'I') - (c > 'O');
  } else {
    throw std::runtime_error("TLE: bad catalog number '" + field + "'");
  }
  for (int i = 1; i < 5; ++i)
    if (!std::isdigit(static_cast<unsigned char>(field[i])) && field[i] != ' ')
      throw std::runtime_error("TLE: bad catalog number '" + field + "'");
  return lead * 10000 + std::stoi(field.substr(1));
}

}  // namespace

Tle parseTle(const std::string &l1, const std::string &l2) {
  if (l1.size() != 69 || l2.size() != 69 || l1[0] != '1' || l2[0] != '2')
    throw std::runtime_error("TLE: need two valid 69-character lines");
  if (!checksumOk(l1) || !checksumOk(l2)) throw std::runtime_error("TLE: checksum failed");
  if (l1.compare(2, 5, l2, 2, 5) != 0)
    throw std::runtime_error("TLE: satellite numbers differ between lines");

  Tle tle;
  tle.line1 = l1;
  tle.line2 = l2;
  try {  // stoi/stod throw std::invalid_argument on blank or garbled fields
    const int yy = std::stoi(l1.substr(18, 2));
    const double doy = std::stod(l1.substr(20, 12));
    const int64_t year = yy + (yy < 57 ? 2000 : 1900);  // NORAD pivot
    tle.noradId = catalogNumber(l1.substr(2, 5));
    tle.epochPosix = (static_cast<double>(daysFromCivil(year, 1, 1)) + (doy - 1.0)) * 86400.0;
  } catch (const std::logic_error &) {
    throw std::runtime_error("TLE: bad epoch or catalog number in '" + l1 + "'");
  }
  return tle;
}

std::string isoUtc(double posix) {
  const double whole = std::floor(posix);
  int64_t ms = std::llround((posix - whole) * 1000.0);
  int64_t secs = static_cast<int64_t>(whole);
  if (ms == 1000) {
    ms = 0;
    ++secs;
  }
  int64_t days = secs / 86400, rem = secs % 86400;
  if (rem < 0) {
    rem += 86400;
    --days;
  }
  const CivilDate date = civilFromDays(days);
  char buf[40];
  std::snprintf(buf, sizeof buf, "%04d-%02u-%02uT%02d:%02d:%02d.%03dZ", static_cast<int>(date.year), date.month,
                date.day, static_cast<int>(rem / 3600), static_cast<int>((rem / 60) % 60), static_cast<int>(rem % 60),
                static_cast<int>(ms));
  return buf;
}

double parseUtc(const std::string &text) {
  int y, mo, d, h, mi, n = 0;
  double s;
  if (std::sscanf(text.c_str(), "%4d-%2d-%2dT%2d:%2d:%lf%n", &y, &mo, &d, &h, &mi, &s, &n) == 6) {
    const std::string rest = text.substr(static_cast<size_t>(n));
    if ((rest.empty() || rest == "Z") && mo >= 1 && mo <= 12 && d >= 1 && d <= 31 && h >= 0 && h < 24 && mi >= 0 &&
        mi < 60 && s >= 0 && s < 61) {
      const int64_t days = daysFromCivil(y, mo, d);
      const CivilDate date = civilFromDays(days);  // round trip rejects dates like Feb 31
      if (date.year == y && static_cast<int>(date.month) == mo && static_cast<int>(date.day) == d)
        return static_cast<double>(days) * 86400.0 + h * 3600.0 + mi * 60.0 + s;
    }
  } else {
    size_t used = 0;
    try {
      const double posix = std::stod(text, &used);
      if (used == text.size() && std::isfinite(posix)) return posix;
    } catch (const std::exception &) {
    }
  }
  throw std::runtime_error("bad UTC time '" + text + "' (use 2027-06-01T12:00:00Z or posix seconds)");
}

}  // namespace adcs
