//
// Sponsored License - for use in support of a program or activity
// sponsored by MathWorks.  Not for government, commercial or other
// non-sponsored organizational use.
// File: rem.cpp
//
// MATLAB Coder version            : 25.2
// C/C++ source code generated on  : 26-Sep-2026 11:30:14
//

// Include Files
#include "rem.h"
#include "rt_nonfinite.h"
#include <cfloat>
#include <cmath>

// Function Definitions
//
// Arguments    : double x
// Return Type  : double
//
namespace adcs {
namespace coder {
double b_rem(double x)
{
  double r;
  if (std::isnan(x) || std::isinf(x)) {
    r = rtNaN;
  } else {
    r = std::abs(x / 6.2831853071795862);
    if (!(std::abs(r - std::floor(r + 0.5)) > DBL_EPSILON * r)) {
      r = 0.0 * x;
    } else {
      r = std::fmod(x, 6.2831853071795862);
    }
  }
  return r;
}

} // namespace coder
} // namespace adcs

//
// File trailer for rem.cpp
//
// [EOF]
//
