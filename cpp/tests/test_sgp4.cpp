// C++ counterpart of matlab/tests/test_sgp4.m: checks the generated adcs::AdcsCore
// against the published Vallado SGP4-VER vectors (satellite 00005, TEME, km, km/s).
#include "AdcsCore.h"
#include "tleToSatrec_types.h"

#include <cmath>
#include <cstdio>

namespace {
int failures = 0;

void expectNear(const char *what, const double got[3], const double want[3], double tol) {
  const double d = std::sqrt((got[0] - want[0]) * (got[0] - want[0]) +
                             (got[1] - want[1]) * (got[1] - want[1]) +
                             (got[2] - want[2]) * (got[2] - want[2]));
  if (!(d < tol)) {
    std::printf("FAIL %s: |diff| = %g (tol %g)\n", what, d, tol);
    ++failures;
  }
}
} // namespace

int main() {
  const char l1[] = "1 00005U 58002B   00179.78495062  .00000023  00000-0  28098-4 0  4753";
  const char l2[] = "2 00005  34.2682 348.7242 1859667 331.7664  19.3264 10.82419157413667";

  adcs::AdcsCore core;
  adcs::struct0_T sr;
  core.tleToSatrec(l1, l2, &sr);

  double r[3], v[3], err;

  core.sgp4prop(&sr, 0.0, r, v, &err);
  if (err != 0) { std::printf("FAIL sgp4 error code at t=0: %g\n", err); ++failures; }
  const double r0[3] = {7022.46529266, -1400.08296755, 0.03995155};
  const double v0[3] = {1.893841015, 6.405893759, 4.534807250};
  expectNear("r(0)", r, r0, 1e-6);
  expectNear("v(0)", v, v0, 1e-7);

  core.sgp4prop(&sr, 360.0, r, v, &err);
  if (err != 0) { std::printf("FAIL sgp4 error code at t=360: %g\n", err); ++failures; }
  const double r1[3] = {-7154.03120202, -3783.17682504, -3536.19412294};
  const double v1[3] = {4.741887409, -4.151817765, -2.093935425};
  expectNear("r(360)", r, r1, 1e-6);
  expectNear("v(360)", v, v1, 1e-7);

  // Deep-space orbits must be rejected (error 7), not silently mis-propagated.
  const double pi = 3.14159265358979323846;
  adcs::struct0_T geo;
  core.sgp4init(0.0001, 0.05 * pi / 180.0, 0, 0, 0, 2 * pi / 1436.07, 0, &geo);
  if (geo.error != 7) { std::printf("FAIL deep-space error = %g, expected 7\n", geo.error); ++failures; }

  if (failures == 0) std::puts("test_sgp4: all checks passed");
  return failures == 0 ? 0 : 1;
}
