//
// Sponsored License - for use in support of a program or activity
// sponsored by MathWorks.  Not for government, commercial or other
// non-sponsored organizational use.
// File: minOrMax.cpp
//
// MATLAB Coder version            : 25.2
// C/C++ source code generated on  : 26-Sep-2026 11:30:14
//

// Include Files
#include "minOrMax.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>

// Function Definitions
//
// Arguments    : const ::coder::array<double, 1U> &x
//                int &idx
// Return Type  : double
//
namespace adcs {
namespace coder {
namespace internal {
double maximum(const ::coder::array<double, 1U> &x, int &idx)
{
  double ex;
  int last;
  last = x.size(0);
  if (x.size(0) <= 2) {
    if (x.size(0) == 1) {
      ex = x[0];
      idx = 1;
    } else {
      ex = x[x.size(0) - 1];
      if ((x[0] < ex) || (std::isnan(x[0]) && (!std::isnan(ex)))) {
        idx = x.size(0);
      } else {
        ex = x[0];
        idx = 1;
      }
    }
  } else {
    int b_idx;
    if (!std::isnan(x[0])) {
      b_idx = 1;
    } else {
      int k;
      boolean_T exitg1;
      b_idx = 0;
      k = 2;
      exitg1 = false;
      while ((!exitg1) && (k <= last)) {
        if (!std::isnan(x[k - 1])) {
          b_idx = k;
          exitg1 = true;
        } else {
          k++;
        }
      }
    }
    if (b_idx == 0) {
      ex = x[0];
      idx = 1;
    } else {
      ex = x[b_idx - 1];
      idx = b_idx;
      b_idx++;
      for (int b_k{b_idx}; b_k <= last; b_k++) {
        double d;
        d = x[b_k - 1];
        if (ex < d) {
          ex = d;
          idx = b_k;
        }
      }
    }
  }
  return ex;
}

} // namespace internal
} // namespace coder
} // namespace adcs

//
// File trailer for minOrMax.cpp
//
// [EOF]
//
