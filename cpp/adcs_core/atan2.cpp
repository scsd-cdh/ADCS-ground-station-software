//
// Sponsored License - for use in support of a program or activity
// sponsored by MathWorks.  Not for government, commercial or other
// non-sponsored organizational use.
// File: atan2.cpp
//
// MATLAB Coder version            : 25.2
// C/C++ source code generated on  : 26-Sep-2026 11:30:14
//

// Include Files
#include "atan2.h"
#include "rt_nonfinite.h"
#include "rt_defines.h"
#include <cmath>

// Function Definitions
//
// Arguments    : double y
//                double x
// Return Type  : double
//
namespace adcs {
namespace coder {
double b_atan2(double y, double x)
{
  double r;
  if (std::isnan(y) || std::isnan(x)) {
    r = rtNaN;
  } else if (std::isinf(y) && std::isinf(x)) {
    int i;
    int i1;
    if (y > 0.0) {
      i = 1;
    } else {
      i = -1;
    }
    if (x > 0.0) {
      i1 = 1;
    } else {
      i1 = -1;
    }
    r = std::atan2(static_cast<double>(i), static_cast<double>(i1));
  } else if (x == 0.0) {
    if (y > 0.0) {
      r = RT_PI / 2.0;
    } else if (y < 0.0) {
      r = -(RT_PI / 2.0);
    } else {
      r = 0.0;
    }
  } else {
    r = std::atan2(y, x);
  }
  return r;
}

} // namespace coder
} // namespace adcs

//
// File trailer for atan2.cpp
//
// [EOF]
//
