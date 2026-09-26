#include "adcs/window_predictor.h"

#include <array>
#include <cmath>
#include <cstdio>
#include <stdexcept>

namespace adcs {
namespace {

// Days since 1970-01-01 for a civil date (H. Hinnant's algorithm).
long daysFromCivil(long y, unsigned m, unsigned d) {
  y -= m <= 2;
  const long era = (y >= 0 ? y : y - 399) / 400;
  const unsigned yoe = static_cast<unsigned>(y - era * 400);
  const unsigned doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1;
  const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + static_cast<long>(doe) - 719468;
}

void civilFromDays(long z, long &y, unsigned &m, unsigned &d) {
  z += 719468;
  const long era = (z >= 0 ? z : z - 146096) / 146097;
  const unsigned doe = static_cast<unsigned>(z - era * 146097);
  const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  y = static_cast<long>(yoe) + era * 400;
  const unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  const unsigned mp = (5 * doy + 2) / 153;
  d = doy - (153 * mp + 2) / 5 + 1;
  m = mp < 10 ? mp + 3 : mp - 9;
  y += m <= 2;
}

bool checksumOk(const std::string &line) {
  int sum = 0;
  for (int i = 0; i < 68; ++i) {
    if (line[i] >= '0' && line[i] <= '9') sum += line[i] - '0';
    else if (line[i] == '-') sum += 1;
  }
  return sum % 10 == line[68] - '0';
}

// computeWindowsCore on a posix time grid.
// Rows: [startPosix endPosix maxElevDeg startTruncated endTruncated].
std::vector<std::array<double, 5>> windowsOnGrid(AdcsCore &core, const std::vector<double> &times,
                                                 const std::vector<double> &ecef, const GroundStation &gs,
                                                 double minElevDeg) {
  const int n = static_cast<int>(times.size());
  coder::array<double, 1U> tSec;
  coder::array<double, 2U> pos, win;
  tSec.set_size(n);
  pos.set_size(n, 3);
  for (int i = 0; i < n; ++i) {
    tSec[i] = times[i] - times[0];
    for (int k = 0; k < 3; ++k) pos.at(i, k) = ecef[3 * i + k];
  }
  core.computeWindowsCore(tSec, pos, gs.latDeg, gs.lonDeg, gs.altM, minElevDeg, win);

  std::vector<std::array<double, 5>> rows;
  for (int k = 0; k < win.size(0); ++k) {
    rows.push_back({times[0] + win.at(k, 0), times[0] + win.at(k, 1), win.at(k, 2), win.at(k, 3), win.at(k, 4)});
  }
  return rows;
}

} // namespace

Tle parseTle(const std::string &l1, const std::string &l2) {
  if (l1.size() != 69 || l2.size() != 69 || l1[0] != '1' || l2[0] != '2')
    throw std::runtime_error("TLE: need two valid 69-character lines");
  if (!checksumOk(l1) || !checksumOk(l2)) throw std::runtime_error("TLE: checksum failed");
  if (l1.compare(2, 5, l2, 2, 5) != 0)
    throw std::runtime_error("TLE: satellite numbers differ between lines");

  const int yy = std::stoi(l1.substr(18, 2));
  const double doy = std::stod(l1.substr(20, 12));
  const long year = yy + (yy < 57 ? 2000 : 1900); // NORAD pivot
  Tle tle;
  tle.line1 = l1;
  tle.line2 = l2;
  tle.epochPosix = (static_cast<double>(daysFromCivil(year, 1, 1)) + (doy - 1.0)) * 86400.0;
  return tle;
}

std::vector<double> propagateEcef(AdcsCore &core, const Tle &tle, const std::vector<double> &times) {
  const int n = static_cast<int>(times.size());
  struct0_T satrec;
  core.tleToSatrec(tle.line1.c_str(), tle.line2.c_str(), &satrec);
  if (satrec.error != 0)
    throw std::runtime_error("SGP4 init failed (error " + std::to_string(static_cast<int>(satrec.error)) +
                             "; 7 = deep-space orbit, not supported)");

  coder::array<double, 2U> rT, vT, rE, vE;
  coder::array<double, 1U> jd;
  rT.set_size(n, 3);
  vT.set_size(n, 3);
  jd.set_size(n);
  for (int i = 0; i < n; ++i) {
    double r[3], v[3], err;
    core.sgp4prop(&satrec, (times[i] - tle.epochPosix) / 60.0, r, v, &err);
    if (err != 0)
      throw std::runtime_error("SGP4 error " + std::to_string(static_cast<int>(err)) + " at " + formatUtc(times[i]));
    for (int k = 0; k < 3; ++k) {
      rT.at(i, k) = r[k] * 1000.0; // km -> m
      vT.at(i, k) = v[k] * 1000.0;
    }
    jd[i] = 2440587.5 + times[i] / 86400.0;
  }
  core.temeToEcef(rT, vT, jd, rE, vE);

  std::vector<double> out(3 * static_cast<size_t>(n));
  for (int i = 0; i < n; ++i)
    for (int k = 0; k < 3; ++k) out[3 * i + k] = rE.at(i, k);
  return out;
}

std::vector<Window> predictWindows(AdcsCore &core, const Tle &tle, const GroundStation &gs, double startPosix,
                                   const WindowSettings &s) {
  const int n = static_cast<int>(std::floor(s.horizonHours * 3600.0 / s.stepSec + 1e-9)) + 1;
  std::vector<double> t(n);
  for (int i = 0; i < n; ++i) t[i] = startPosix + i * s.stepSec;

  std::vector<Window> out;
  for (const auto &w : windowsOnGrid(core, t, propagateEcef(core, tle, t), gs, s.minElevationDeg)) {
    double peak = w[2];
    if (s.peakRefineStepSec > 0) {
      // Resample the pass finely: coarse samples miss near-overhead peaks by ~0.2 deg.
      const int m = static_cast<int>(std::floor((w[1] - w[0]) / s.peakRefineStepSec + 1e-9)) + 1;
      std::vector<double> tf(m);
      for (int i = 0; i < m; ++i) tf[i] = w[0] + i * s.peakRefineStepSec;
      const auto fine = windowsOnGrid(core, tf, propagateEcef(core, tle, tf), gs, -90.0);
      if (!fine.empty() && fine[0][2] > peak) peak = fine[0][2];
    }
    out.push_back({w[0], w[1], peak, w[3] != 0, w[4] != 0});
  }
  return out;
}

std::string formatUtc(double posix) {
  static const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  const long secs = static_cast<long>(std::floor(posix));
  long days = secs / 86400, rem = secs % 86400;
  if (rem < 0) {
    rem += 86400;
    --days;
  }
  long y;
  unsigned m, d;
  civilFromDays(days, y, m, d);
  char buf[32];
  std::snprintf(buf, sizeof buf, "%02u-%s-%04ld %02ld:%02ld:%02ld", d, months[m - 1], y, rem / 3600,
                (rem / 60) % 60, rem % 60);
  return buf;
}

} // namespace adcs
