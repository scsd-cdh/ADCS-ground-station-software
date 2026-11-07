// Copyright (c) 2026, Space Concordia Contributors
#include "adcs/contact_state.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "AdcsCore.h"
#include "adcs/config.h"
#include "tleToSatrec_types.h"

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

namespace fs = std::filesystem;

namespace adcs {
namespace {

struct LatestTle {
  Tle tle;
  TleSource source;
  std::string warning;
};

std::string readFile(const fs::path &path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) throw std::runtime_error("cannot read " + path.string());
  std::ostringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

// Write-then-rename, so a crash mid-write never leaves a truncated file.
void writeFile(const fs::path &path, const std::string &text) {
  const fs::path tmp = path.string() + ".tmp";
  {
    std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
    if (!(out << text) || !out.flush()) throw std::runtime_error("cannot write " + tmp.string());
  }
  fs::rename(tmp, path);
}

std::string join(const std::vector<std::string> &parts) {
  std::string out;
  for (const auto &p : parts) out += (out.empty() ? "" : "; ") + p;
  return out;
}

std::string shorten(std::string s) {
  while (!s.empty() && (s.back() == '\n' || s.back() == '\r' || s.back() == ' ')) s.pop_back();
  return s.size() > 200 ? s.substr(0, 200) + "..." : s;
}

// First "1 ..."/"2 ..." line pair in text (CelesTrak FORMAT=TLE puts a name line first).
Tle extractTle(const std::string &text, int noradId) {
  std::istringstream in(text);
  std::string line, line1;
  while (std::getline(in, line)) {
    while (!line.empty() && (line.back() == '\r' || line.back() == ' ' || line.back() == '\t')) line.pop_back();
    if (line.rfind("1 ", 0) == 0) {
      line1 = line;
    } else if (line.rfind("2 ", 0) == 0 && !line1.empty()) {
      const Tle tle = parseTle(line1, line);
      if (tle.noradId != noradId)
        throw std::runtime_error("TLE is for satellite " + std::to_string(tle.noradId) + ", expected " +
                                 std::to_string(noradId));
      return tle;
    }
  }
  // CelesTrak answers unknown or not-yet-catalogued objects with "No GP data found".
  throw std::runtime_error("no TLE in response: " + shorten(text));
}

// Pull times (posix) in the last 24 h.
std::vector<double> recentPulls(const fs::path &log, double now) {
  std::vector<double> out;
  std::ifstream in(log);
  for (std::string line; std::getline(in, line);) {
    try {
      const double t = std::stod(line);
      if (t > now - 86400) out.push_back(t);  // future entries (clock stepped back) still count
    } catch (const std::exception &) {
    }
  }
  return out;
}

std::string curlGet(const std::string &url, int timeoutSec) {
  // The URL goes through the shell: allow only plain URL characters.
  for (const char c : url)
    if (!std::isalnum(static_cast<unsigned char>(c)) && std::string(":/._-?=&").find(c) == std::string::npos)
      throw std::runtime_error("unsupported character in URL: " + url);
  const std::string cmd = "curl -fsS --max-time " + std::to_string(timeoutSec) + " \"" + url + "\" 2>&1";
  FILE *pipe = popen(cmd.c_str(), "r");
  if (!pipe) throw std::runtime_error("cannot run curl");
  std::string out;
  char buf[4096];
  for (size_t n; (n = std::fread(buf, 1, sizeof buf, pipe)) > 0;) out.append(buf, n);
  if (pclose(pipe) != 0) throw std::runtime_error("curl: " + shorten(out));
  return out;
}

double systemNow() {
  return std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
}

LatestTle latestTle(const ContactConfig &c) {
  if (!c.tleFile.empty()) return {extractTle(readFile(c.tleFile), c.noradId), TleSource::File, ""};

  const fs::path dir = c.cacheDir.empty() ? dataDir() / "tle" : c.cacheDir;
  fs::create_directories(dir);
  const fs::path cachePath = dir / ("celestrak_" + std::to_string(c.noradId) + ".tle");
  const fs::path logPath = dir / "celestrak_pulls.log";  // shared by all satellites: the limit is per IP
  const double now = c.now ? c.now() : systemNow();

  std::optional<Tle> cached;
  std::vector<std::string> notes;
  if (fs::exists(cachePath)) {
    try {
      cached = extractTle(readFile(cachePath), c.noradId);
    } catch (const std::exception &e) {
      notes.push_back(std::string("cached TLE unusable: ") + e.what());
    }
  }

  std::vector<double> pulls = recentPulls(logPath, now);
  const double last = pulls.empty() ? -std::numeric_limits<double>::infinity()
                                    : *std::max_element(pulls.begin(), pulls.end());
  std::string skipped;  // why no pull was made; normal, not a warning, while the cache is usable
  if (static_cast<int>(pulls.size()) >= c.maxPullsPerDay) {
    skipped = "daily CelesTrak limit reached (" + std::to_string(c.maxPullsPerDay) + " pulls in 24 h)";
  } else if (now - last < c.minPullIntervalSec) {
    skipped = "last CelesTrak pull was " + std::to_string(static_cast<int64_t>((now - last) / 60)) +
              " min ago (minimum " + std::to_string(static_cast<int64_t>(c.minPullIntervalSec / 60)) + ")";
  } else {
    // Log the attempt before making it: failed requests count against the limit too.
    pulls.push_back(now);
    std::ostringstream log;
    log.precision(15);
    for (const double t : pulls) log << t << '\n';
    writeFile(logPath, log.str());

    const std::string url = c.celestrakUrl + "?CATNR=" + std::to_string(c.noradId) + "&FORMAT=TLE";
    try {
      const std::string body = c.httpGet ? c.httpGet(url, c.timeoutSec) : curlGet(url, c.timeoutSec);
      const Tle fresh = extractTle(body, c.noradId);
      if (!cached || fresh.epochPosix >= cached->epochPosix) {
        writeFile(cachePath, fresh.line1 + '\n' + fresh.line2 + '\n');
        return {fresh, TleSource::CelesTrak, join(notes)};
      }
      notes.push_back("CelesTrak returned an older TLE than the cached one");
    } catch (const std::exception &e) {
      notes.push_back(std::string("CelesTrak pull failed: ") + e.what());
    }
  }

  if (!cached) {
    if (!skipped.empty()) notes.push_back(skipped);
    notes.push_back("no cached TLE in " + dir.string() + " (set ADCS_TLE_FILE to supply one)");
    throw std::runtime_error(join(notes));
  }
  if (!notes.empty()) notes.push_back("using cached TLE");
  return {*cached, TleSource::Cache, join(notes)};
}

}  // namespace

ContactConfig ContactConfig::fromEnv() {
  ContactConfig c;
  if (const char *v = std::getenv("ADCS_NORAD_ID"); v && *v) {
    char *end = nullptr;
    const int64_t id = std::strtoll(v, &end, 10);
    if (*end != '\0' || id <= 0 || id > 339999) throw std::runtime_error(std::string("bad ADCS_NORAD_ID '") + v + "'");
    c.noradId = static_cast<int>(id);
  }
  if (const char *v = std::getenv("ADCS_TLE_SOURCE_URL"); v && *v) {
    c.celestrakUrl = v;
  }
  if (const char *v = std::getenv("ADCS_TLE_FILE"); v && *v) {
    c.tleFile = v;
  }
  c.cacheDir = dataDir() / "tle";
  return c;
}

ContactState contactState(double contactPosix, const ContactConfig &config) {
  if (config.noradId <= 0) throw std::runtime_error("ADCS_NORAD_ID is not set");
  const LatestTle latest = latestTle(config);
  const Tle &tle = latest.tle;

  AdcsCore core;
  struct0_T satrec;
  core.tleToSatrec(tle.line1.c_str(), tle.line2.c_str(), &satrec);
  if (satrec.error != 0)
    throw std::runtime_error("SGP4 init failed (error " + std::to_string(static_cast<int>(satrec.error)) +
                             "; 7 = deep-space orbit, not supported)");
  double r[3], v[3], err;
  core.sgp4prop(&satrec, (contactPosix - tle.epochPosix) / 60.0, r, v, &err);
  if (err != 0)
    throw std::runtime_error("SGP4 error " + std::to_string(static_cast<int>(err)) + " at " + isoUtc(contactPosix));

  coder::array<double, 2U> rT, vT, rE, vE;
  coder::array<double, 1U> jd;
  rT.set_size(1, 3);
  vT.set_size(1, 3);
  jd.set_size(1);
  for (int k = 0; k < 3; ++k) {
    rT.at(0, k) = r[k];
    vT.at(0, k) = v[k];
  }
  jd[0] = 2440587.5 + contactPosix / 86400.0;
  core.temeToEcef(rT, vT, jd, rE, vE);

  ContactState s;
  s.timePosix = contactPosix;
  for (int k = 0; k < 3; ++k) {
    s.rTemeKm[k] = r[k];
    s.vTemeKmS[k] = v[k];
    s.rEcefKm[k] = rE.at(0, k);
    s.vEcefKmS[k] = vE.at(0, k);
  }
  s.tle = tle;
  s.tleAgeSec = contactPosix - tle.epochPosix;
  s.source = latest.source;
  s.warning = latest.warning;
  if (std::abs(s.tleAgeSec) > config.staleTleSec) {
    if (!s.warning.empty()) s.warning += "; ";
    s.warning += "TLE is " + std::to_string(std::lround(s.tleAgeSec / 3600)) + " h from the contact time";
  }
  return s;
}

const char *toString(TleSource source) {
  switch (source) {
  case TleSource::CelesTrak: return "celestrak";
  case TleSource::Cache: return "cache";
  case TleSource::File: return "file";
  }
  return "?";
}

}  // namespace adcs
