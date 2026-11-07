// Copyright (c) 2026, Space Concordia Contributors
// Checks adcs::contactState: CelesTrak usage limits (5 pulls/day, 90 min apart,
// failed attempts count), cache fallback, response validation, ADCS_TLE_FILE
// override and the propagated state. HTTP and the clock are faked: no network.
#include <cmath>
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>

#include "adcs/contact_state.h"

namespace fs = std::filesystem;

namespace {
int failures = 0;

void check(bool ok, const std::string &what) {
  if (!ok) {
    std::printf("FAIL %s\n", what.c_str());
    ++failures;
  }
}

// Appends the TLE checksum to a 68-character line.
std::string withChecksum(std::string line) {
  int sum = 0;
  for (const char c : line) sum += (c >= '0' && c <= '9') ? c - '0' : (c == '-');
  return line + std::to_string(sum % 10);
}

// ISS, epoch 2024-01-01T12:00:00Z + extraDays.
std::string issLine1(int extraDays = 0) {
  char epoch[16];
  std::snprintf(epoch, sizeof epoch, "%03d.50000000", 1 + extraDays);
  return withChecksum(std::string("1 25544U 98067A   24") + epoch + "  .00016717  00000-0  10270-3 0  999");
}
const char kIssLine2[] ="2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.50377579 26624";
const double kIssEpoch = 1704110400;  // 2024-01-01T12:00:00Z

// Same TLE for another catalog number ("99999", Alpha-5 "A0001", ...).
std::string renumber(const std::string &line, const std::string &id) {
  return withChecksum(line.substr(0, 2) + id + line.substr(7, 61));
}

// CelesTrak FORMAT=TLE reply: name line, CRLF line ends.
std::string celestrakReply(int extraDays = 0) {
  return "ISS (ZARYA)             \r\n" + issLine1(extraDays) + "\r\n" + kIssLine2 + "\r\n";
}

struct Fake {
  double now = kIssEpoch;
  int calls = 0;
  std::string lastUrl;
  std::string reply = celestrakReply();
  bool fail = false;
};

adcs::ContactConfig config(Fake *fake, const fs::path &dir) {
  adcs::ContactConfig c;
  c.noradId = 25544;
  c.cacheDir = dir;
  c.now = [fake] { return fake->now; };
  c.httpGet = [fake](const std::string &url, int) {
    ++fake->calls;
    fake->lastUrl = url;
    if (fake->fail) throw std::runtime_error("timeout");
    return fake->reply;
  };
  return c;
}

fs::path freshDir(const char *name) {
  const fs::path dir = fs::temp_directory_path() / "adcs_test_contact_state" / name;
  fs::remove_all(dir);
  fs::create_directories(dir);
  return dir;
}

double norm(const double v[3]) { return std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); }

void testPullAndState() {
  Fake fake;
  const auto dir = freshDir("pull");
  const auto s = adcs::contactState(kIssEpoch + 600, config(&fake, dir));
  check(fake.calls == 1, "first call pulls");
  check(fake.lastUrl == "https://celestrak.org/NORAD/elements/gp.php?CATNR=25544&FORMAT=TLE", "URL " + fake.lastUrl);
  check(s.source == adcs::TleSource::CelesTrak, "source celestrak");
  check(s.warning.empty(), "no warning: " + s.warning);
  check(fs::exists(dir / "celestrak_25544.tle"), "cache written");
  check(std::abs(s.tleAgeSec - 600) < 1e-3, "TLE age");
  check(s.tle.noradId == 25544, "norad id");
  // ISS: ~6650-6800 km radius, ~7.66 km/s.
  check(norm(s.rTemeKm) > 6600 && norm(s.rTemeKm) < 6850, "ISS radius");
  check(std::abs(norm(s.vTemeKmS) - 7.66) < 0.1, "ISS speed");
  // TEME -> ECEF is a rotation about z.
  check(std::abs(norm(s.rEcefKm) - norm(s.rTemeKm)) < 1e-6, "ECEF radius");
  check(std::abs(s.rEcefKm[2] - s.rTemeKm[2]) < 1e-6, "ECEF z");
  // Earth-relative speed is lower for a prograde orbit.
  check(norm(s.vEcefKmS) < norm(s.vTemeKmS), "ECEF speed");
}

void testLimits() {
  Fake fake;
  const auto dir = freshDir("limits");
  const double t0 = fake.now;
  auto c = config(&fake, dir);

  adcs::contactState(t0, c);
  check(fake.calls == 1, "pull 1");

  fake.now = t0 + 89 * 60;  // under 90 min: cache, no warning
  auto s = adcs::contactState(fake.now, c);
  check(fake.calls == 1, "no pull within 90 min");
  check(s.source == adcs::TleSource::Cache && s.warning.empty(), "cache within 90 min, no warning");

  // Pulls 2-5, 91 min apart.
  for (int i = 1; i <= 4; ++i) {
    fake.now = t0 + i * 91 * 60;
    adcs::contactState(fake.now, c);
  }
  check(fake.calls == 5, "5 pulls, got " + std::to_string(fake.calls));

  fake.now = t0 + 5 * 91 * 60;  // 90 min since the last one, but 5 in 24 h
  s = adcs::contactState(fake.now, c);
  check(fake.calls == 5, "6th pull in 24 h refused");
  check(s.source == adcs::TleSource::Cache, "cache at daily limit");

  // The limit lives in the log on disk: a new process (fresh config) still respects it.
  auto c2 = config(&fake, dir);
  adcs::contactState(fake.now, c2);
  check(fake.calls == 5, "limit survives restart");

  fake.now = t0 + 86400 + 1;  // first pull is out of the 24 h window
  s = adcs::contactState(fake.now, c);
  check(fake.calls == 6 && s.source == adcs::TleSource::CelesTrak, "pull again after 24 h");
}

void testFailureFallsBackAndCounts() {
  Fake fake;
  const auto dir = freshDir("failure");
  const double t0 = fake.now;
  auto c = config(&fake, dir);
  adcs::contactState(t0, c);

  fake.fail = true;
  fake.now = t0 + 91 * 60;
  auto s = adcs::contactState(fake.now, c);
  check(fake.calls == 2, "pull attempted");
  check(s.source == adcs::TleSource::Cache, "failed pull uses cache");
  check(s.warning.find("CelesTrak pull failed: timeout") != std::string::npos, "warning: " + s.warning);

  fake.fail = false;
  fake.now = t0 + 100 * 60;  // failed attempt counts: 9 min since it
  adcs::contactState(fake.now, c);
  check(fake.calls == 2, "failed attempt counts toward the 90 min spacing");

  // CelesTrak's answer for objects it has no data for, and a reply for the wrong object.
  for (const std::string &reply :
       {std::string("No GP data found\r\n"),
        renumber(issLine1(), "99999") + "\n" + renumber(kIssLine2, "99999") + "\n"}) {
    fake.reply = reply;
    fake.now += 91 * 60;
    s = adcs::contactState(fake.now, c);
    check(s.source == adcs::TleSource::Cache && !s.warning.empty(), "bad reply rejected: " + s.warning);
  }
}

void testNoCacheThrows() {
  Fake fake;
  fake.fail = true;
  const auto dir = freshDir("nocache");
  bool threw = false;
  try {
    adcs::contactState(fake.now, config(&fake, dir));
  } catch (const std::runtime_error &e) {
    threw = std::string(e.what()).find("no cached TLE") != std::string::npos;
  }
  check(threw, "no TLE at all throws");
}

void testKeepsNewerCache() {
  Fake fake;
  const auto dir = freshDir("older");
  auto c = config(&fake, dir);
  fake.reply = celestrakReply(1);  // epoch +1 day
  adcs::contactState(fake.now, c);
  fake.reply = celestrakReply(0);  // older epoch
  fake.now += 91 * 60;
  const auto s = adcs::contactState(fake.now, c);
  check(s.source == adcs::TleSource::Cache && std::abs(s.tle.epochPosix - (kIssEpoch + 86400)) < 1e-3,
        "older reply does not replace newer cache");
}

void testTleFileAndVallado() {
  // Vallado SGP4-VER satellite 00005 at t = 360 min (same vectors as test_sgp4).
  Fake fake;
  const auto dir = freshDir("file");
  const fs::path file = dir / "manual.tle";
  std::ofstream(file) << "1 00005U 58002B   00179.78495062  .00000023  00000-0  28098-4 0  4753\n"
                         "2 00005  34.2682 348.7242 1859667 331.7664  19.3264 10.82419157413667\n";
  auto c = config(&fake, dir);
  c.noradId = 5;
  c.tleFile = file;
  const adcs::Tle tle = adcs::parseTle("1 00005U 58002B   00179.78495062  .00000023  00000-0  28098-4 0  4753",
                                       "2 00005  34.2682 348.7242 1859667 331.7664  19.3264 10.82419157413667");
  const auto s = adcs::contactState(tle.epochPosix + 360 * 60, c);
  check(fake.calls == 0, "TLE file: no network");
  check(s.source == adcs::TleSource::File, "source file");
  const double r1[3] = {-7154.03120202, -3783.17682504, -3536.19412294};
  const double v1[3] = {4.741887409, -4.151817765, -2.093935425};
  for (int k = 0; k < 3; ++k) {
    check(std::abs(s.rTemeKm[k] - r1[k]) < 1e-5, "Vallado r");
    check(std::abs(s.vTemeKmS[k] - v1[k]) < 1e-7, "Vallado v");
  }
  check(s.warning.find("from the contact time") == std::string::npos, "6 h old TLE is not stale");
  const auto old = adcs::contactState(tle.epochPosix + 3 * 86400, c);
  check(old.warning.find("from the contact time") != std::string::npos, "3 day old TLE warns: " + old.warning);
}

void testParsing() {
  const std::string good = issLine1();
  const std::string bad = good.substr(0, 68) + static_cast<char>('0' + (good[68] - '0' + 1) % 10);
  bool threw = false;
  try {
    adcs::parseTle(bad, kIssLine2);
  } catch (const std::runtime_error &) {
    threw = true;
  }
  check(threw, "bad checksum rejected");

  const auto a5 = adcs::parseTle(renumber(issLine1(), "A0001"), renumber(kIssLine2, "A0001"));
  check(a5.noradId == 100001, "Alpha-5 A0001 -> 100001, got " + std::to_string(a5.noradId));

  check(adcs::isoUtc(kIssEpoch) == "2024-01-01T12:00:00.000Z", "isoUtc " + adcs::isoUtc(kIssEpoch));
  check(adcs::parseUtc("2024-01-01T12:00:00Z") == kIssEpoch, "parseUtc ISO");
  check(adcs::parseUtc("1704110400.5") == kIssEpoch + 0.5, "parseUtc posix");
  check(adcs::parseUtc("2024-02-29T00:00:00Z") == kIssEpoch - 43200 + 59 * 86400.0, "parseUtc leap day");
  for (const char *badTime : {"2026-02-31T00:00:00Z", "2025-02-29T00:00:00Z", "2026-10-29T-1:00:00Z", "noon"}) {
    threw = false;
    try {
      adcs::parseUtc(badTime);
    } catch (const std::runtime_error &) {
      threw = true;
    }
    check(threw, std::string("parseUtc rejects ") + badTime);
  }
}

}  // namespace

int main() {
  try {
    testPullAndState();
    testLimits();
    testFailureFallsBackAndCounts();
    testNoCacheThrows();
    testKeepsNewerCache();
    testTleFileAndVallado();
    testParsing();
  } catch (const std::exception &e) {
    std::printf("FAIL unexpected exception: %s\n", e.what());
    ++failures;
  }
  if (failures == 0) std::puts("test_contact_state: all checks passed");
  return failures == 0 ? 0 : 1;
}
