//
// Sponsored License - for use in support of a program or activity
// sponsored by MathWorks.  Not for government, commercial or other
// non-sponsored organizational use.
// File: AdcsCore.h
//
// MATLAB Coder version            : 25.2
// C/C++ source code generated on  : 26-Sep-2026 11:30:14
//

#ifndef ADCSCORE_H
#define ADCSCORE_H

// Include Files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace adcs {
struct struct0_T;

}

// Type Definitions
namespace adcs {
class AdcsCore {
public:
  AdcsCore();
  ~AdcsCore();
  void tleToSatrec(const char line1[69], const char line2[69],
                   struct0_T *satrec);
  void sgp4init(double ecco, double inclo, double nodeo, double argpo,
                double mo, double noKozai, double bstar, struct0_T *satrec);
  void sgp4prop(const struct0_T *satrec, double tsince, double r[3],
                double v[3], double *err);
  void temeToEcef(const ::coder::array<double, 2U> &rTeme,
                  const ::coder::array<double, 2U> &vTeme,
                  const ::coder::array<double, 1U> &jdUt1,
                  ::coder::array<double, 2U> &rEcef,
                  ::coder::array<double, 2U> &vEcef);
  void computeWindowsCore(const ::coder::array<double, 1U> &tSec,
                          const ::coder::array<double, 2U> &posEcef,
                          double gsLatDeg, double gsLonDeg, double gsAltM,
                          double minElevDeg, ::coder::array<double, 2U> &win);
};

} // namespace adcs

#endif
//
// File trailer for AdcsCore.h
//
// [EOF]
//
