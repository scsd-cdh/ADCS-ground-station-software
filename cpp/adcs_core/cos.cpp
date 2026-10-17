//
// Sponsored License - for use in support of a program or activity
// sponsored by MathWorks.  Not for government, commercial or other
// non-sponsored organizational use.
// File: cos.cpp
//
// MATLAB Coder version            : 25.2
// C/C++ source code generated on  : 26-Sep-2026 11:30:14
//

// Include Files
#include "cos.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include "omp.h"
#include <cmath>

// Function Definitions
//
// Arguments    : ::coder::array<double, 1U> &x
// Return Type  : void
//
namespace adcs {
namespace coder {
void b_cos(::coder::array<double, 1U> &x)
{
  int nx;
  nx = x.size(0);
  if (static_cast<int>(nx < 1600)) {
    for (int k{0}; k < nx; k++) {
      x[k] = std::cos(x[k]);
    }
  } else {
#pragma omp parallel for num_threads(omp_get_max_threads())

    for (int k = 0; k < nx; k++) {
      x[k] = std::cos(x[k]);
    }
  }
}

} // namespace coder
} // namespace adcs

//
// File trailer for cos.cpp
//
// [EOF]
//
