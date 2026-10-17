//
// Sponsored License - for use in support of a program or activity
// sponsored by MathWorks.  Not for government, commercial or other
// non-sponsored organizational use.
// File: sum.cpp
//
// MATLAB Coder version            : 25.2
// C/C++ source code generated on  : 26-Sep-2026 11:30:14
//

// Include Files
#include "sum.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <emmintrin.h>

// Function Definitions
//
// Arguments    : const ::coder::array<double, 2U> &x
//                ::coder::array<double, 1U> &y
// Return Type  : void
//
namespace adcs {
namespace coder {
void sum(const ::coder::array<double, 2U> &x, ::coder::array<double, 1U> &y)
{
  if (x.size(0) == 0) {
    y.set_size(0);
  } else {
    int scalarLB;
    int vectorUB;
    int vstride;
    vstride = x.size(0);
    y.set_size(x.size(0));
    for (int xj{0}; xj < vstride; xj++) {
      y[xj] = x[xj];
    }
    scalarLB = (x.size(0) / 2) << 1;
    vectorUB = scalarLB - 2;
    for (int xj{0}; xj < 2; xj++) {
      int xoffset;
      xoffset = (xj + 1) * vstride;
      for (int b_xj{0}; b_xj <= vectorUB; b_xj += 2) {
        __m128d r;
        r = _mm_loadu_pd(&y[b_xj]);
        _mm_storeu_pd(&y[b_xj],
                      _mm_add_pd(r, _mm_loadu_pd(&x[xoffset + b_xj])));
      }
      for (int b_xj{scalarLB}; b_xj < vstride; b_xj++) {
        y[b_xj] = y[b_xj] + x[xoffset + b_xj];
      }
    }
  }
}

} // namespace coder
} // namespace adcs

//
// File trailer for sum.cpp
//
// [EOF]
//
