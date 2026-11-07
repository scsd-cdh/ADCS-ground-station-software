//
// Sponsored License - for use in support of a program or activity
// sponsored by MathWorks.  Not for government, commercial or other
// non-sponsored organizational use.
// File: str2double.h
//
// MATLAB Coder version            : 25.2
// C/C++ source code generated on  : 26-Sep-2026 11:30:14
//

#ifndef STR2DOUBLE_H
#define STR2DOUBLE_H

// Include Files
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace adcs {
namespace coder {
namespace internal {
creal_T b_str2double(const char s[2]);

creal_T c_str2double(const char s[8]);

creal_T d_str2double(const char s[9]);

creal_T e_str2double(const char s[11]);

creal_T str2double(const char s[5]);

} // namespace internal
} // namespace coder
} // namespace adcs

#endif
//
// File trailer for str2double.h
//
// [EOF]
//
