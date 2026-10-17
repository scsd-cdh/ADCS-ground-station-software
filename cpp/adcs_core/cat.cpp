//
// Sponsored License - for use in support of a program or activity
// sponsored by MathWorks.  Not for government, commercial or other
// non-sponsored organizational use.
// File: cat.cpp
//
// MATLAB Coder version            : 25.2
// C/C++ source code generated on  : 26-Sep-2026 11:30:14
//

// Include Files
#include "cat.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include "omp.h"

// Function Definitions
//
// Arguments    : ::coder::array<double, 2U> &in1
//                const ::coder::array<double, 2U> &in2
//                const ::coder::array<double, 2U> &in3
// Return Type  : void
//
namespace adcs {
void binary_expand_op(::coder::array<double, 2U> &in1,
                      const ::coder::array<double, 2U> &in2,
                      const ::coder::array<double, 2U> &in3)
{
  ::coder::array<double, 1U> b_in2;
  ::coder::array<double, 1U> c_in2;
  int b_loop_ub;
  int loop_ub;
  int stride_0_0;
  int stride_1_0;
  if (in3.size(0) == 1) {
    loop_ub = in2.size(0);
  } else {
    loop_ub = in3.size(0);
  }
  b_in2.set_size(loop_ub);
  stride_0_0 = (in2.size(0) != 1);
  stride_1_0 = (in3.size(0) != 1);
  b_loop_ub = (loop_ub < 1600);
  if (b_loop_ub) {
    for (int i{0}; i < loop_ub; i++) {
      b_in2[i] = in2[i * stride_0_0] +
                 7.29211514670698E-5 * in3[i * stride_1_0 + in3.size(0)];
    }
  } else {
#pragma omp parallel for num_threads(omp_get_max_threads())

    for (int i = 0; i < loop_ub; i++) {
      b_in2[i] = in2[i * stride_0_0] +
                 7.29211514670698E-5 * in3[i * stride_1_0 + in3.size(0)];
    }
  }
  c_in2.set_size(loop_ub);
  if (b_loop_ub) {
    for (int i1{0}; i1 < loop_ub; i1++) {
      c_in2[i1] = in2[i1 * stride_0_0 + in2.size(0)] -
                  7.29211514670698E-5 * in3[i1 * stride_1_0];
    }
  } else {
#pragma omp parallel for num_threads(omp_get_max_threads())

    for (int i1 = 0; i1 < loop_ub; i1++) {
      c_in2[i1] = in2[i1 * stride_0_0 + in2.size(0)] -
                  7.29211514670698E-5 * in3[i1 * stride_1_0];
    }
  }
  in1.set_size(loop_ub, 3);
  for (int i2{0}; i2 < loop_ub; i2++) {
    in1[i2] = b_in2[i2];
  }
  for (int i2{0}; i2 < loop_ub; i2++) {
    in1[i2 + in1.size(0)] = c_in2[i2];
  }
  b_loop_ub = in2.size(0);
  for (int i2{0}; i2 < b_loop_ub; i2++) {
    in1[i2 + in1.size(0) * 2] = in2[i2 + in2.size(0) * 2];
  }
}

//
// Arguments    : ::coder::array<double, 2U> &in1
//                const ::coder::array<double, 1U> &in2
//                const ::coder::array<double, 2U> &in3
//                const ::coder::array<double, 1U> &in4
//                const ::coder::array<double, 1U> &in5
// Return Type  : void
//
void binary_expand_op_1(::coder::array<double, 2U> &in1,
                        const ::coder::array<double, 1U> &in2,
                        const ::coder::array<double, 2U> &in3,
                        const ::coder::array<double, 1U> &in4,
                        const ::coder::array<double, 1U> &in5)
{
  ::coder::array<double, 1U> b_in2;
  ::coder::array<double, 1U> b_in5;
  int b_in2_tmp;
  int b_loop_ub;
  int in2_tmp;
  int in5_tmp;
  int loop_ub;
  int stride_0_0;
  int stride_1_0;
  int stride_2_0;
  if (in3.size(0) == 1) {
    loop_ub = in4.size(0);
  } else {
    loop_ub = in3.size(0);
  }
  if (loop_ub == 1) {
    if (in3.size(0) == 1) {
      loop_ub = in2.size(0);
    } else {
      loop_ub = in3.size(0);
    }
  }
  b_in2.set_size(loop_ub);
  stride_0_0 = (in2.size(0) != 1);
  stride_1_0 = (in3.size(0) != 1);
  stride_2_0 = (in4.size(0) != 1);
  if (static_cast<int>(loop_ub < 1600)) {
    for (int i{0}; i < loop_ub; i++) {
      in2_tmp = i * stride_1_0;
      b_in2[i] = in2[i * stride_0_0] * in3[in2_tmp] +
                 in4[i * stride_2_0] * in3[in2_tmp + in3.size(0)];
    }
  } else {
#pragma omp parallel for num_threads(omp_get_max_threads()) private(b_in2_tmp)

    for (int i = 0; i < loop_ub; i++) {
      b_in2_tmp = i * stride_1_0;
      b_in2[i] = in2[i * stride_0_0] * in3[b_in2_tmp] +
                 in4[i * stride_2_0] * in3[b_in2_tmp + in3.size(0)];
    }
  }
  if (in3.size(0) == 1) {
    b_loop_ub = in2.size(0);
  } else {
    b_loop_ub = in3.size(0);
  }
  if (b_loop_ub == 1) {
    if (in3.size(0) == 1) {
      b_loop_ub = in5.size(0);
    } else {
      b_loop_ub = in3.size(0);
    }
  }
  b_in5.set_size(b_loop_ub);
  stride_2_0 = (in5.size(0) != 1);
  if (static_cast<int>(b_loop_ub < 1600)) {
    for (int i1{0}; i1 < b_loop_ub; i1++) {
      in2_tmp = i1 * stride_1_0;
      b_in5[i1] = in5[i1 * stride_2_0] * in3[in2_tmp] +
                  in2[i1 * stride_0_0] * in3[in2_tmp + in3.size(0)];
    }
  } else {
#pragma omp parallel for num_threads(omp_get_max_threads()) private(in5_tmp)

    for (int i1 = 0; i1 < b_loop_ub; i1++) {
      in5_tmp = i1 * stride_1_0;
      b_in5[i1] = in5[i1 * stride_2_0] * in3[in5_tmp] +
                  in2[i1 * stride_0_0] * in3[in5_tmp + in3.size(0)];
    }
  }
  in1.set_size(loop_ub, 3);
  for (int i2{0}; i2 < loop_ub; i2++) {
    in1[i2] = b_in2[i2];
  }
  for (int i2{0}; i2 < b_loop_ub; i2++) {
    in1[i2 + in1.size(0)] = b_in5[i2];
  }
  in2_tmp = in3.size(0);
  for (int i2{0}; i2 < in2_tmp; i2++) {
    in1[i2 + in1.size(0) * 2] = in3[i2 + in3.size(0) * 2];
  }
}

} // namespace adcs

//
// File trailer for cat.cpp
//
// [EOF]
//
