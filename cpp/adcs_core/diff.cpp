//
// Sponsored License - for use in support of a program or activity
// sponsored by MathWorks.  Not for government, commercial or other
// non-sponsored organizational use.
// File: diff.cpp
//
// MATLAB Coder version            : 25.2
// C/C++ source code generated on  : 26-Sep-2026 11:30:14
//

// Include Files
#include "diff.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
//
// Arguments    : const ::coder::array<double, 1U> &x
//                ::coder::array<double, 1U> &y
// Return Type  : void
//
namespace adcs {
namespace coder {
void diff(const ::coder::array<double, 1U> &x, ::coder::array<double, 1U> &y)
{
  double work_data;
  int dimSize;
  dimSize = x.size(0);
  y.set_size(x.size(0) - 1);
  work_data = x[0];
  for (int m{2}; m <= dimSize; m++) {
    double tmp2;
    tmp2 = work_data;
    work_data = x[m - 1];
    y[m - 2] = work_data - tmp2;
  }
}

} // namespace coder
} // namespace adcs

//
// File trailer for diff.cpp
//
// [EOF]
//
