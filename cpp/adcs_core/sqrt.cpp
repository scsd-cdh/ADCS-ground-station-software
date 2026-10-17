//
// Sponsored License - for use in support of a program or activity
// sponsored by MathWorks.  Not for government, commercial or other
// non-sponsored organizational use.
// File: sqrt.cpp
//
// MATLAB Coder version            : 25.2
// C/C++ source code generated on  : 26-Sep-2026 11:30:14
//

// Include Files
#include "sqrt.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>
#include <emmintrin.h>

// Function Definitions
//
// Arguments    : ::coder::array<double, 1U> &x
// Return Type  : void
//
namespace adcs {
namespace coder {
void b_sqrt(::coder::array<double, 1U> &x)
{
  int nx;
  int scalarLB;
  int vectorUB;
  nx = x.size(0);
  scalarLB = (nx / 2) << 1;
  vectorUB = scalarLB - 2;
  for (int k{0}; k <= vectorUB; k += 2) {
    __m128d r;
    r = _mm_loadu_pd(&x[k]);
    _mm_storeu_pd(&x[k], _mm_sqrt_pd(r));
  }
  for (int k{scalarLB}; k < nx; k++) {
    x[k] = std::sqrt(x[k]);
  }
}

} // namespace coder
} // namespace adcs

//
// File trailer for sqrt.cpp
//
// [EOF]
//
