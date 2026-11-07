//
// Sponsored License - for use in support of a program or activity
// sponsored by MathWorks.  Not for government, commercial or other
// non-sponsored organizational use.
// File: str2double.cpp
//
// MATLAB Coder version            : 25.2
// C/C++ source code generated on  : 26-Sep-2026 11:30:14
//

// Include Files
#include "str2double.h"
#include "rt_nonfinite.h"
#include <cstdio>

// Variable Definitions
namespace adcs {
static const boolean_T bv[128]{
    false, false, false, false, false, false, false, false, false, true,  true,
    true,  true,  true,  false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, true,  true,  true,  true,  true,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false};

}

// Function Declarations
namespace adcs {
namespace coder {
namespace internal {
static boolean_T b_copydigits(char s1[4], int &idx, const char s[2], int &k,
                              boolean_T allowpoint);

static boolean_T b_copyexponent(char s1[4], int &idx, const char s[2], int &k);

static boolean_T b_copysign(char s1[7], int &idx, const char s[5], int &k,
                            boolean_T &success);

static boolean_T b_isUnitImag(const char s[8], int k);

static boolean_T b_readNonFinite(const char s[2], int &k, double &fv);

static boolean_T b_readfloat(char s1[4], int &idx, const char s[2], int &k,
                             boolean_T &b_finite, double &nfv,
                             boolean_T &foundsign, boolean_T &success);

static boolean_T c_copydigits(char s1[10], int &idx, const char s[8], int &k,
                              boolean_T allowpoint);

static boolean_T c_copyexponent(char s1[10], int &idx, const char s[8], int &k);

static boolean_T c_copysign(char s1[4], int &idx, const char s[2], int &k,
                            boolean_T &success);

static boolean_T c_isUnitImag(const char s[9], int k);

static boolean_T c_readNonFinite(const char s[8], int &k, double &fv);

static boolean_T c_readfloat(char s1[10], int &idx, const char s[8], int &k,
                             boolean_T &b_finite, double &nfv,
                             boolean_T &foundsign, boolean_T &success);

static boolean_T copydigits(char s1[7], int &idx, const char s[5], int &k,
                            boolean_T allowpoint);

static boolean_T copyexponent(char s1[7], int &idx, const char s[5], int &k);

static boolean_T d_copydigits(char s1[11], int &idx, const char s[9], int &k,
                              boolean_T allowpoint);

static boolean_T d_copyexponent(char s1[11], int &idx, const char s[9], int &k);

static boolean_T d_copysign(char s1[10], int &idx, const char s[8], int &k,
                            boolean_T &success);

static boolean_T d_isUnitImag(const char s[11], int k);

static boolean_T d_readNonFinite(const char s[9], int &k, double &fv);

static boolean_T d_readfloat(char s1[11], int &idx, const char s[9], int &k,
                             boolean_T &b_finite, double &nfv,
                             boolean_T &foundsign, boolean_T &success);

static boolean_T e_copydigits(char s1[13], int &idx, const char s[11], int &k,
                              boolean_T allowpoint);

static boolean_T e_copyexponent(char s1[13], int &idx, const char s[11],
                                int &k);

static boolean_T e_copysign(char s1[11], int &idx, const char s[9], int &k,
                            boolean_T &success);

static boolean_T e_readNonFinite(const char s[11], int &k, double &fv);

static boolean_T e_readfloat(char s1[13], int &idx, const char s[11], int &k,
                             boolean_T &b_finite, double &nfv,
                             boolean_T &foundsign, boolean_T &success);

static boolean_T f_copysign(char s1[13], int &idx, const char s[11], int &k,
                            boolean_T &success);

static boolean_T isUnitImag(const char s[5], int k);

static boolean_T readNonFinite(const char s[5], int &k, double &fv);

static boolean_T readfloat(char s1[7], int &idx, const char s[5], int &k,
                           boolean_T &b_finite, double &nfv,
                           boolean_T &foundsign, boolean_T &success);

} // namespace internal
} // namespace coder
} // namespace adcs

// Function Definitions
//
// Arguments    : char s1[4]
//                int &idx
//                const char s[2]
//                int &k
//                boolean_T allowpoint
// Return Type  : boolean_T
//
namespace adcs {
namespace coder {
namespace internal {
static boolean_T b_copydigits(char s1[4], int &idx, const char s[2], int &k,
                              boolean_T allowpoint)
{
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (k <= 2);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (k <= 2))) {
    char c;
    c = s[k - 1];
    if ((c >= '0') && (c <= '9')) {
      s1[idx - 1] = c;
      idx++;
      k++;
    } else if (c == '.') {
      if (allowpoint && (!haspoint)) {
        success = true;
      } else {
        success = false;
      }
      if (success) {
        s1[idx - 1] = '.';
        idx++;
        haspoint = true;
      }
      k++;
    } else if (c == ',') {
      k++;
    } else {
      exitg1 = true;
    }
  }
  return success;
}

//
// Arguments    : char s1[4]
//                int &idx
//                const char s[2]
//                int &k
// Return Type  : boolean_T
//
static boolean_T b_copyexponent(char s1[4], int &idx, const char s[2], int &k)
{
  boolean_T success;
  success = true;
  if (k <= 2) {
    char c;
    c = s[k - 1];
    if ((c == 'E') || (c == 'e')) {
      int b_k;
      int kexp;
      boolean_T b_success;
      s1[idx - 1] = 'e';
      idx++;
      k++;
      while ((k <= 2) && (s[1] == ',')) {
        k = 3;
      }
      if (k <= 2) {
        if (s[1] == '-') {
          s1[idx - 1] = '-';
          idx++;
          k = 3;
        } else if (s[1] == '+') {
          k = 3;
        }
      }
      kexp = k;
      b_k = k;
      b_success = b_copydigits(s1, idx, s, b_k, false);
      k = b_k;
      if ((!b_success) || (b_k <= kexp)) {
        success = false;
      }
    }
  }
  return success;
}

//
// Arguments    : char s1[7]
//                int &idx
//                const char s[5]
//                int &k
//                boolean_T &success
// Return Type  : boolean_T
//
static boolean_T b_copysign(char s1[7], int &idx, const char s[5], int &k,
                            boolean_T &success)
{
  boolean_T b_success;
  boolean_T exitg1;
  boolean_T foundsign;
  boolean_T isneg;
  isneg = false;
  foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (k <= 5)) {
    char c;
    c = s[k - 1];
    if (c == '-') {
      isneg = !isneg;
      foundsign = true;
      k++;
    } else if (c == ',') {
      k++;
    } else if (c == '+') {
      foundsign = true;
      k++;
    } else if (!bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)]) {
      exitg1 = true;
    } else {
      k++;
    }
  }
  b_success = (k <= 5);
  success = b_success;
  if (b_success && isneg) {
    if ((idx >= 2) && (s1[idx - 2] == '-')) {
      s1[idx - 2] = ' ';
    } else {
      s1[idx - 1] = '-';
      idx++;
    }
  }
  return foundsign;
}

//
// Arguments    : const char s[8]
//                int k
// Return Type  : boolean_T
//
static boolean_T b_isUnitImag(const char s[8], int k)
{
  boolean_T p;
  p = false;
  if (k <= 8) {
    if (s[k - 1] == 'j') {
      p = true;
    } else if (s[k - 1] == 'i') {
      if (k >= 7) {
        p = true;
      } else {
        int b_k;
        char c[3];
        b_k = k;
        for (int j{0}; j < 3; j++) {
          c[j] = '\x00';
          while ((b_k <= 8) && (s[b_k - 1] == ',')) {
            b_k++;
          }
          if (b_k <= 8) {
            c[j] = s[b_k - 1];
          }
          b_k++;
        }
        if ((((c[0] != 'I') && (c[0] != 'i')) ||
             ((c[1] != 'N') && (c[1] != 'n')) ||
             ((c[2] != 'F') && (c[2] != 'f'))) &&
            (((c[0] != 'N') && (c[0] != 'n')) ||
             ((c[1] != 'A') && (c[1] != 'a')) ||
             ((c[2] != 'N') && (c[2] != 'n')))) {
          p = true;
        }
      }
    }
  }
  return p;
}

//
// Arguments    : const char s[2]
//                int &k
//                double &fv
// Return Type  : boolean_T
//
static boolean_T b_readNonFinite(const char s[2], int &k, double &fv)
{
  int ksaved;
  char c[3];
  boolean_T b_finite;
  ksaved = k;
  for (int j{0}; j < 3; j++) {
    c[j] = '\x00';
    while ((k <= 2) && (s[k - 1] == ',')) {
      k++;
    }
    if (k <= 2) {
      c[j] = s[k - 1];
    }
    k++;
  }
  if (((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n')) &&
      ((c[2] == 'F') || (c[2] == 'f'))) {
    b_finite = false;
    fv = rtInf;
  } else if (((c[0] == 'N') || (c[0] == 'n')) &&
             ((c[1] == 'A') || (c[1] == 'a')) &&
             ((c[2] == 'N') || (c[2] == 'n'))) {
    b_finite = false;
    fv = rtNaN;
  } else {
    b_finite = true;
    fv = 0.0;
    k = ksaved;
  }
  return b_finite;
}

//
// Arguments    : char s1[4]
//                int &idx
//                const char s[2]
//                int &k
//                boolean_T &b_finite
//                double &nfv
//                boolean_T &foundsign
//                boolean_T &success
// Return Type  : boolean_T
//
static boolean_T b_readfloat(char s1[4], int &idx, const char s[2], int &k,
                             boolean_T &b_finite, double &nfv,
                             boolean_T &foundsign, boolean_T &success)
{
  int b_idx;
  int b_k;
  boolean_T b_success;
  boolean_T isimag;
  isimag = false;
  b_finite = true;
  nfv = 0.0;
  b_idx = idx;
  b_k = k;
  foundsign = c_copysign(s1, b_idx, s, b_k, b_success);
  idx = b_idx;
  k = b_k;
  success = b_success;
  if (b_success) {
    char p_tmp;
    boolean_T exitg1;
    b_success = false;
    if (b_k <= 2) {
      p_tmp = s[b_k - 1];
      b_success = ((p_tmp == 'j') || (p_tmp == 'i'));
    }
    if (b_success) {
      success = false;
    } else {
      double b_nfv;
      b_success = b_readNonFinite(s, b_k, b_nfv);
      b_finite = b_success;
      nfv = b_nfv;
      k = b_k;
      if (b_success) {
        b_success = b_copydigits(s1, idx, s, k, true);
        success = b_success;
        if (b_success) {
          success = b_copyexponent(s1, idx, s, k);
        }
      } else if ((b_idx >= 2) && (s1[b_idx - 2] == '-')) {
        idx = b_idx - 1;
        s1[b_idx - 2] = ' ';
        nfv = -b_nfv;
      }
      exitg1 = false;
      while ((!exitg1) && (k <= 2)) {
        p_tmp = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(p_tmp) & 127U)] ||
            (p_tmp == '\x00') || (p_tmp == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
      if ((k <= 2) && (s[k - 1] == '*')) {
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 2)) {
          p_tmp = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(p_tmp) & 127U)] ||
              (p_tmp == '\x00') || (p_tmp == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
      }
      if (k <= 2) {
        p_tmp = s[k - 1];
        if ((p_tmp == 'i') || (p_tmp == 'j')) {
          k++;
          isimag = true;
        }
      }
    }
    exitg1 = false;
    while ((!exitg1) && (k <= 2)) {
      p_tmp = s[k - 1];
      if (bv[static_cast<int>(static_cast<unsigned char>(p_tmp) & 127U)] ||
          (p_tmp == '\x00') || (p_tmp == ',')) {
        k++;
      } else {
        exitg1 = true;
      }
    }
  }
  return isimag;
}

//
// Arguments    : char s1[10]
//                int &idx
//                const char s[8]
//                int &k
//                boolean_T allowpoint
// Return Type  : boolean_T
//
static boolean_T c_copydigits(char s1[10], int &idx, const char s[8], int &k,
                              boolean_T allowpoint)
{
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (k <= 8);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (k <= 8))) {
    char c;
    c = s[k - 1];
    if ((c >= '0') && (c <= '9')) {
      s1[idx - 1] = c;
      idx++;
      k++;
    } else if (c == '.') {
      if (allowpoint && (!haspoint)) {
        success = true;
      } else {
        success = false;
      }
      if (success) {
        s1[idx - 1] = '.';
        idx++;
        haspoint = true;
      }
      k++;
    } else if (c == ',') {
      k++;
    } else {
      exitg1 = true;
    }
  }
  return success;
}

//
// Arguments    : char s1[10]
//                int &idx
//                const char s[8]
//                int &k
// Return Type  : boolean_T
//
static boolean_T c_copyexponent(char s1[10], int &idx, const char s[8], int &k)
{
  boolean_T success;
  success = true;
  if (k <= 8) {
    char c;
    c = s[k - 1];
    if ((c == 'E') || (c == 'e')) {
      int b_k;
      int kexp;
      boolean_T b_success;
      s1[idx - 1] = 'e';
      idx++;
      k++;
      while ((k <= 8) && (s[k - 1] == ',')) {
        k++;
      }
      if (k <= 8) {
        if (s[k - 1] == '-') {
          s1[idx - 1] = '-';
          idx++;
          k++;
        } else if (s[k - 1] == '+') {
          k++;
        }
      }
      kexp = k;
      b_k = k;
      b_success = c_copydigits(s1, idx, s, b_k, false);
      k = b_k;
      if ((!b_success) || (b_k <= kexp)) {
        success = false;
      }
    }
  }
  return success;
}

//
// Arguments    : char s1[4]
//                int &idx
//                const char s[2]
//                int &k
//                boolean_T &success
// Return Type  : boolean_T
//
static boolean_T c_copysign(char s1[4], int &idx, const char s[2], int &k,
                            boolean_T &success)
{
  boolean_T b_success;
  boolean_T exitg1;
  boolean_T foundsign;
  boolean_T isneg;
  isneg = false;
  foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (k <= 2)) {
    char c;
    c = s[k - 1];
    if (c == '-') {
      isneg = !isneg;
      foundsign = true;
      k++;
    } else if (c == ',') {
      k++;
    } else if (c == '+') {
      foundsign = true;
      k++;
    } else if (!bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)]) {
      exitg1 = true;
    } else {
      k++;
    }
  }
  b_success = (k <= 2);
  success = b_success;
  if (b_success && isneg) {
    if ((idx >= 2) && (s1[idx - 2] == '-')) {
      s1[idx - 2] = ' ';
    } else {
      s1[idx - 1] = '-';
      idx++;
    }
  }
  return foundsign;
}

//
// Arguments    : const char s[9]
//                int k
// Return Type  : boolean_T
//
static boolean_T c_isUnitImag(const char s[9], int k)
{
  boolean_T p;
  p = false;
  if (k <= 9) {
    if (s[k - 1] == 'j') {
      p = true;
    } else if (s[k - 1] == 'i') {
      if (k >= 8) {
        p = true;
      } else {
        int b_k;
        char c[3];
        b_k = k;
        for (int j{0}; j < 3; j++) {
          c[j] = '\x00';
          while ((b_k <= 9) && (s[b_k - 1] == ',')) {
            b_k++;
          }
          if (b_k <= 9) {
            c[j] = s[b_k - 1];
          }
          b_k++;
        }
        if ((((c[0] != 'I') && (c[0] != 'i')) ||
             ((c[1] != 'N') && (c[1] != 'n')) ||
             ((c[2] != 'F') && (c[2] != 'f'))) &&
            (((c[0] != 'N') && (c[0] != 'n')) ||
             ((c[1] != 'A') && (c[1] != 'a')) ||
             ((c[2] != 'N') && (c[2] != 'n')))) {
          p = true;
        }
      }
    }
  }
  return p;
}

//
// Arguments    : const char s[8]
//                int &k
//                double &fv
// Return Type  : boolean_T
//
static boolean_T c_readNonFinite(const char s[8], int &k, double &fv)
{
  int ksaved;
  char c[3];
  boolean_T b_finite;
  ksaved = k;
  for (int j{0}; j < 3; j++) {
    c[j] = '\x00';
    while ((k <= 8) && (s[k - 1] == ',')) {
      k++;
    }
    if (k <= 8) {
      c[j] = s[k - 1];
    }
    k++;
  }
  if (((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n')) &&
      ((c[2] == 'F') || (c[2] == 'f'))) {
    b_finite = false;
    fv = rtInf;
  } else if (((c[0] == 'N') || (c[0] == 'n')) &&
             ((c[1] == 'A') || (c[1] == 'a')) &&
             ((c[2] == 'N') || (c[2] == 'n'))) {
    b_finite = false;
    fv = rtNaN;
  } else {
    b_finite = true;
    fv = 0.0;
    k = ksaved;
  }
  return b_finite;
}

//
// Arguments    : char s1[10]
//                int &idx
//                const char s[8]
//                int &k
//                boolean_T &b_finite
//                double &nfv
//                boolean_T &foundsign
//                boolean_T &success
// Return Type  : boolean_T
//
static boolean_T c_readfloat(char s1[10], int &idx, const char s[8], int &k,
                             boolean_T &b_finite, double &nfv,
                             boolean_T &foundsign, boolean_T &success)
{
  int b_idx;
  int b_k;
  boolean_T b_success;
  boolean_T isimag;
  isimag = false;
  b_finite = true;
  nfv = 0.0;
  b_idx = idx;
  b_k = k;
  foundsign = d_copysign(s1, b_idx, s, b_k, b_success);
  idx = b_idx;
  k = b_k;
  success = b_success;
  if (b_success) {
    char c;
    boolean_T exitg1;
    if (b_isUnitImag(s, b_k)) {
      success = false;
    } else {
      double b_nfv;
      b_success = c_readNonFinite(s, b_k, b_nfv);
      b_finite = b_success;
      nfv = b_nfv;
      k = b_k;
      if (b_success) {
        b_success = c_copydigits(s1, idx, s, k, true);
        success = b_success;
        if (b_success) {
          success = c_copyexponent(s1, idx, s, k);
        }
      } else if ((b_idx >= 2) && (s1[b_idx - 2] == '-')) {
        idx = b_idx - 1;
        s1[b_idx - 2] = ' ';
        nfv = -b_nfv;
      }
      exitg1 = false;
      while ((!exitg1) && (k <= 8)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
      if ((k <= 8) && (s[k - 1] == '*')) {
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 8)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
      }
      if (k <= 8) {
        c = s[k - 1];
        if ((c == 'i') || (c == 'j')) {
          k++;
          isimag = true;
        }
      }
    }
    exitg1 = false;
    while ((!exitg1) && (k <= 8)) {
      c = s[k - 1];
      if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
          (c == '\x00') || (c == ',')) {
        k++;
      } else {
        exitg1 = true;
      }
    }
  }
  return isimag;
}

//
// Arguments    : char s1[7]
//                int &idx
//                const char s[5]
//                int &k
//                boolean_T allowpoint
// Return Type  : boolean_T
//
static boolean_T copydigits(char s1[7], int &idx, const char s[5], int &k,
                            boolean_T allowpoint)
{
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (k <= 5);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (k <= 5))) {
    char c;
    c = s[k - 1];
    if ((c >= '0') && (c <= '9')) {
      s1[idx - 1] = c;
      idx++;
      k++;
    } else if (c == '.') {
      if (allowpoint && (!haspoint)) {
        success = true;
      } else {
        success = false;
      }
      if (success) {
        s1[idx - 1] = '.';
        idx++;
        haspoint = true;
      }
      k++;
    } else if (c == ',') {
      k++;
    } else {
      exitg1 = true;
    }
  }
  return success;
}

//
// Arguments    : char s1[7]
//                int &idx
//                const char s[5]
//                int &k
// Return Type  : boolean_T
//
static boolean_T copyexponent(char s1[7], int &idx, const char s[5], int &k)
{
  boolean_T success;
  success = true;
  if (k <= 5) {
    char c;
    c = s[k - 1];
    if ((c == 'E') || (c == 'e')) {
      int b_k;
      int kexp;
      boolean_T b_success;
      s1[idx - 1] = 'e';
      idx++;
      k++;
      while ((k <= 5) && (s[k - 1] == ',')) {
        k++;
      }
      if (k <= 5) {
        if (s[k - 1] == '-') {
          s1[idx - 1] = '-';
          idx++;
          k++;
        } else if (s[k - 1] == '+') {
          k++;
        }
      }
      kexp = k;
      b_k = k;
      b_success = copydigits(s1, idx, s, b_k, false);
      k = b_k;
      if ((!b_success) || (b_k <= kexp)) {
        success = false;
      }
    }
  }
  return success;
}

//
// Arguments    : char s1[11]
//                int &idx
//                const char s[9]
//                int &k
//                boolean_T allowpoint
// Return Type  : boolean_T
//
static boolean_T d_copydigits(char s1[11], int &idx, const char s[9], int &k,
                              boolean_T allowpoint)
{
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (k <= 9);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (k <= 9))) {
    char c;
    c = s[k - 1];
    if ((c >= '0') && (c <= '9')) {
      s1[idx - 1] = c;
      idx++;
      k++;
    } else if (c == '.') {
      if (allowpoint && (!haspoint)) {
        success = true;
      } else {
        success = false;
      }
      if (success) {
        s1[idx - 1] = '.';
        idx++;
        haspoint = true;
      }
      k++;
    } else if (c == ',') {
      k++;
    } else {
      exitg1 = true;
    }
  }
  return success;
}

//
// Arguments    : char s1[11]
//                int &idx
//                const char s[9]
//                int &k
// Return Type  : boolean_T
//
static boolean_T d_copyexponent(char s1[11], int &idx, const char s[9], int &k)
{
  boolean_T success;
  success = true;
  if (k <= 9) {
    char c;
    c = s[k - 1];
    if ((c == 'E') || (c == 'e')) {
      int b_k;
      int kexp;
      boolean_T b_success;
      s1[idx - 1] = 'e';
      idx++;
      k++;
      while ((k <= 9) && (s[k - 1] == ',')) {
        k++;
      }
      if (k <= 9) {
        if (s[k - 1] == '-') {
          s1[idx - 1] = '-';
          idx++;
          k++;
        } else if (s[k - 1] == '+') {
          k++;
        }
      }
      kexp = k;
      b_k = k;
      b_success = d_copydigits(s1, idx, s, b_k, false);
      k = b_k;
      if ((!b_success) || (b_k <= kexp)) {
        success = false;
      }
    }
  }
  return success;
}

//
// Arguments    : char s1[10]
//                int &idx
//                const char s[8]
//                int &k
//                boolean_T &success
// Return Type  : boolean_T
//
static boolean_T d_copysign(char s1[10], int &idx, const char s[8], int &k,
                            boolean_T &success)
{
  boolean_T b_success;
  boolean_T exitg1;
  boolean_T foundsign;
  boolean_T isneg;
  isneg = false;
  foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (k <= 8)) {
    char c;
    c = s[k - 1];
    if (c == '-') {
      isneg = !isneg;
      foundsign = true;
      k++;
    } else if (c == ',') {
      k++;
    } else if (c == '+') {
      foundsign = true;
      k++;
    } else if (!bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)]) {
      exitg1 = true;
    } else {
      k++;
    }
  }
  b_success = (k <= 8);
  success = b_success;
  if (b_success && isneg) {
    if ((idx >= 2) && (s1[idx - 2] == '-')) {
      s1[idx - 2] = ' ';
    } else {
      s1[idx - 1] = '-';
      idx++;
    }
  }
  return foundsign;
}

//
// Arguments    : const char s[11]
//                int k
// Return Type  : boolean_T
//
static boolean_T d_isUnitImag(const char s[11], int k)
{
  boolean_T p;
  p = false;
  if (k <= 11) {
    if (s[k - 1] == 'j') {
      p = true;
    } else if (s[k - 1] == 'i') {
      if (k >= 10) {
        p = true;
      } else {
        int b_k;
        char c[3];
        b_k = k;
        for (int j{0}; j < 3; j++) {
          c[j] = '\x00';
          while ((b_k <= 11) && (s[b_k - 1] == ',')) {
            b_k++;
          }
          if (b_k <= 11) {
            c[j] = s[b_k - 1];
          }
          b_k++;
        }
        if ((((c[0] != 'I') && (c[0] != 'i')) ||
             ((c[1] != 'N') && (c[1] != 'n')) ||
             ((c[2] != 'F') && (c[2] != 'f'))) &&
            (((c[0] != 'N') && (c[0] != 'n')) ||
             ((c[1] != 'A') && (c[1] != 'a')) ||
             ((c[2] != 'N') && (c[2] != 'n')))) {
          p = true;
        }
      }
    }
  }
  return p;
}

//
// Arguments    : const char s[9]
//                int &k
//                double &fv
// Return Type  : boolean_T
//
static boolean_T d_readNonFinite(const char s[9], int &k, double &fv)
{
  int ksaved;
  char c[3];
  boolean_T b_finite;
  ksaved = k;
  for (int j{0}; j < 3; j++) {
    c[j] = '\x00';
    while ((k <= 9) && (s[k - 1] == ',')) {
      k++;
    }
    if (k <= 9) {
      c[j] = s[k - 1];
    }
    k++;
  }
  if (((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n')) &&
      ((c[2] == 'F') || (c[2] == 'f'))) {
    b_finite = false;
    fv = rtInf;
  } else if (((c[0] == 'N') || (c[0] == 'n')) &&
             ((c[1] == 'A') || (c[1] == 'a')) &&
             ((c[2] == 'N') || (c[2] == 'n'))) {
    b_finite = false;
    fv = rtNaN;
  } else {
    b_finite = true;
    fv = 0.0;
    k = ksaved;
  }
  return b_finite;
}

//
// Arguments    : char s1[11]
//                int &idx
//                const char s[9]
//                int &k
//                boolean_T &b_finite
//                double &nfv
//                boolean_T &foundsign
//                boolean_T &success
// Return Type  : boolean_T
//
static boolean_T d_readfloat(char s1[11], int &idx, const char s[9], int &k,
                             boolean_T &b_finite, double &nfv,
                             boolean_T &foundsign, boolean_T &success)
{
  int b_idx;
  int b_k;
  boolean_T b_success;
  boolean_T isimag;
  isimag = false;
  b_finite = true;
  nfv = 0.0;
  b_idx = idx;
  b_k = k;
  foundsign = e_copysign(s1, b_idx, s, b_k, b_success);
  idx = b_idx;
  k = b_k;
  success = b_success;
  if (b_success) {
    char c;
    boolean_T exitg1;
    if (c_isUnitImag(s, b_k)) {
      success = false;
    } else {
      double b_nfv;
      b_success = d_readNonFinite(s, b_k, b_nfv);
      b_finite = b_success;
      nfv = b_nfv;
      k = b_k;
      if (b_success) {
        b_success = d_copydigits(s1, idx, s, k, true);
        success = b_success;
        if (b_success) {
          success = d_copyexponent(s1, idx, s, k);
        }
      } else if ((b_idx >= 2) && (s1[b_idx - 2] == '-')) {
        idx = b_idx - 1;
        s1[b_idx - 2] = ' ';
        nfv = -b_nfv;
      }
      exitg1 = false;
      while ((!exitg1) && (k <= 9)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
      if ((k <= 9) && (s[k - 1] == '*')) {
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 9)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
      }
      if (k <= 9) {
        c = s[k - 1];
        if ((c == 'i') || (c == 'j')) {
          k++;
          isimag = true;
        }
      }
    }
    exitg1 = false;
    while ((!exitg1) && (k <= 9)) {
      c = s[k - 1];
      if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
          (c == '\x00') || (c == ',')) {
        k++;
      } else {
        exitg1 = true;
      }
    }
  }
  return isimag;
}

//
// Arguments    : char s1[13]
//                int &idx
//                const char s[11]
//                int &k
//                boolean_T allowpoint
// Return Type  : boolean_T
//
static boolean_T e_copydigits(char s1[13], int &idx, const char s[11], int &k,
                              boolean_T allowpoint)
{
  boolean_T exitg1;
  boolean_T haspoint;
  boolean_T success;
  success = (k <= 11);
  haspoint = false;
  exitg1 = false;
  while ((!exitg1) && (success && (k <= 11))) {
    char c;
    c = s[k - 1];
    if ((c >= '0') && (c <= '9')) {
      s1[idx - 1] = c;
      idx++;
      k++;
    } else if (c == '.') {
      if (allowpoint && (!haspoint)) {
        success = true;
      } else {
        success = false;
      }
      if (success) {
        s1[idx - 1] = '.';
        idx++;
        haspoint = true;
      }
      k++;
    } else if (c == ',') {
      k++;
    } else {
      exitg1 = true;
    }
  }
  return success;
}

//
// Arguments    : char s1[13]
//                int &idx
//                const char s[11]
//                int &k
// Return Type  : boolean_T
//
static boolean_T e_copyexponent(char s1[13], int &idx, const char s[11], int &k)
{
  boolean_T success;
  success = true;
  if (k <= 11) {
    char c;
    c = s[k - 1];
    if ((c == 'E') || (c == 'e')) {
      int b_k;
      int kexp;
      boolean_T b_success;
      s1[idx - 1] = 'e';
      idx++;
      k++;
      while ((k <= 11) && (s[k - 1] == ',')) {
        k++;
      }
      if (k <= 11) {
        if (s[k - 1] == '-') {
          s1[idx - 1] = '-';
          idx++;
          k++;
        } else if (s[k - 1] == '+') {
          k++;
        }
      }
      kexp = k;
      b_k = k;
      b_success = e_copydigits(s1, idx, s, b_k, false);
      k = b_k;
      if ((!b_success) || (b_k <= kexp)) {
        success = false;
      }
    }
  }
  return success;
}

//
// Arguments    : char s1[11]
//                int &idx
//                const char s[9]
//                int &k
//                boolean_T &success
// Return Type  : boolean_T
//
static boolean_T e_copysign(char s1[11], int &idx, const char s[9], int &k,
                            boolean_T &success)
{
  boolean_T b_success;
  boolean_T exitg1;
  boolean_T foundsign;
  boolean_T isneg;
  isneg = false;
  foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (k <= 9)) {
    char c;
    c = s[k - 1];
    if (c == '-') {
      isneg = !isneg;
      foundsign = true;
      k++;
    } else if (c == ',') {
      k++;
    } else if (c == '+') {
      foundsign = true;
      k++;
    } else if (!bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)]) {
      exitg1 = true;
    } else {
      k++;
    }
  }
  b_success = (k <= 9);
  success = b_success;
  if (b_success && isneg) {
    if ((idx >= 2) && (s1[idx - 2] == '-')) {
      s1[idx - 2] = ' ';
    } else {
      s1[idx - 1] = '-';
      idx++;
    }
  }
  return foundsign;
}

//
// Arguments    : const char s[11]
//                int &k
//                double &fv
// Return Type  : boolean_T
//
static boolean_T e_readNonFinite(const char s[11], int &k, double &fv)
{
  int ksaved;
  char c[3];
  boolean_T b_finite;
  ksaved = k;
  for (int j{0}; j < 3; j++) {
    c[j] = '\x00';
    while ((k <= 11) && (s[k - 1] == ',')) {
      k++;
    }
    if (k <= 11) {
      c[j] = s[k - 1];
    }
    k++;
  }
  if (((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n')) &&
      ((c[2] == 'F') || (c[2] == 'f'))) {
    b_finite = false;
    fv = rtInf;
  } else if (((c[0] == 'N') || (c[0] == 'n')) &&
             ((c[1] == 'A') || (c[1] == 'a')) &&
             ((c[2] == 'N') || (c[2] == 'n'))) {
    b_finite = false;
    fv = rtNaN;
  } else {
    b_finite = true;
    fv = 0.0;
    k = ksaved;
  }
  return b_finite;
}

//
// Arguments    : char s1[13]
//                int &idx
//                const char s[11]
//                int &k
//                boolean_T &b_finite
//                double &nfv
//                boolean_T &foundsign
//                boolean_T &success
// Return Type  : boolean_T
//
static boolean_T e_readfloat(char s1[13], int &idx, const char s[11], int &k,
                             boolean_T &b_finite, double &nfv,
                             boolean_T &foundsign, boolean_T &success)
{
  int b_idx;
  int b_k;
  boolean_T b_success;
  boolean_T isimag;
  isimag = false;
  b_finite = true;
  nfv = 0.0;
  b_idx = idx;
  b_k = k;
  foundsign = f_copysign(s1, b_idx, s, b_k, b_success);
  idx = b_idx;
  k = b_k;
  success = b_success;
  if (b_success) {
    char c;
    boolean_T exitg1;
    if (d_isUnitImag(s, b_k)) {
      success = false;
    } else {
      double b_nfv;
      b_success = e_readNonFinite(s, b_k, b_nfv);
      b_finite = b_success;
      nfv = b_nfv;
      k = b_k;
      if (b_success) {
        b_success = e_copydigits(s1, idx, s, k, true);
        success = b_success;
        if (b_success) {
          success = e_copyexponent(s1, idx, s, k);
        }
      } else if ((b_idx >= 2) && (s1[b_idx - 2] == '-')) {
        idx = b_idx - 1;
        s1[b_idx - 2] = ' ';
        nfv = -b_nfv;
      }
      exitg1 = false;
      while ((!exitg1) && (k <= 11)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
      if ((k <= 11) && (s[k - 1] == '*')) {
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 11)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
      }
      if (k <= 11) {
        c = s[k - 1];
        if ((c == 'i') || (c == 'j')) {
          k++;
          isimag = true;
        }
      }
    }
    exitg1 = false;
    while ((!exitg1) && (k <= 11)) {
      c = s[k - 1];
      if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
          (c == '\x00') || (c == ',')) {
        k++;
      } else {
        exitg1 = true;
      }
    }
  }
  return isimag;
}

//
// Arguments    : char s1[13]
//                int &idx
//                const char s[11]
//                int &k
//                boolean_T &success
// Return Type  : boolean_T
//
static boolean_T f_copysign(char s1[13], int &idx, const char s[11], int &k,
                            boolean_T &success)
{
  boolean_T b_success;
  boolean_T exitg1;
  boolean_T foundsign;
  boolean_T isneg;
  isneg = false;
  foundsign = false;
  exitg1 = false;
  while ((!exitg1) && (k <= 11)) {
    char c;
    c = s[k - 1];
    if (c == '-') {
      isneg = !isneg;
      foundsign = true;
      k++;
    } else if (c == ',') {
      k++;
    } else if (c == '+') {
      foundsign = true;
      k++;
    } else if (!bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)]) {
      exitg1 = true;
    } else {
      k++;
    }
  }
  b_success = (k <= 11);
  success = b_success;
  if (b_success && isneg) {
    if ((idx >= 2) && (s1[idx - 2] == '-')) {
      s1[idx - 2] = ' ';
    } else {
      s1[idx - 1] = '-';
      idx++;
    }
  }
  return foundsign;
}

//
// Arguments    : const char s[5]
//                int k
// Return Type  : boolean_T
//
static boolean_T isUnitImag(const char s[5], int k)
{
  boolean_T p;
  p = false;
  if (k <= 5) {
    if (s[k - 1] == 'j') {
      p = true;
    } else if (s[k - 1] == 'i') {
      if (k >= 4) {
        p = true;
      } else {
        int b_k;
        char c[3];
        b_k = k;
        for (int j{0}; j < 3; j++) {
          c[j] = '\x00';
          while ((b_k <= 5) && (s[b_k - 1] == ',')) {
            b_k++;
          }
          if (b_k <= 5) {
            c[j] = s[b_k - 1];
          }
          b_k++;
        }
        if ((((c[0] != 'I') && (c[0] != 'i')) ||
             ((c[1] != 'N') && (c[1] != 'n')) ||
             ((c[2] != 'F') && (c[2] != 'f'))) &&
            (((c[0] != 'N') && (c[0] != 'n')) ||
             ((c[1] != 'A') && (c[1] != 'a')) ||
             ((c[2] != 'N') && (c[2] != 'n')))) {
          p = true;
        }
      }
    }
  }
  return p;
}

//
// Arguments    : const char s[5]
//                int &k
//                double &fv
// Return Type  : boolean_T
//
static boolean_T readNonFinite(const char s[5], int &k, double &fv)
{
  int ksaved;
  char c[3];
  boolean_T b_finite;
  ksaved = k;
  for (int j{0}; j < 3; j++) {
    c[j] = '\x00';
    while ((k <= 5) && (s[k - 1] == ',')) {
      k++;
    }
    if (k <= 5) {
      c[j] = s[k - 1];
    }
    k++;
  }
  if (((c[0] == 'I') || (c[0] == 'i')) && ((c[1] == 'N') || (c[1] == 'n')) &&
      ((c[2] == 'F') || (c[2] == 'f'))) {
    b_finite = false;
    fv = rtInf;
  } else if (((c[0] == 'N') || (c[0] == 'n')) &&
             ((c[1] == 'A') || (c[1] == 'a')) &&
             ((c[2] == 'N') || (c[2] == 'n'))) {
    b_finite = false;
    fv = rtNaN;
  } else {
    b_finite = true;
    fv = 0.0;
    k = ksaved;
  }
  return b_finite;
}

//
// Arguments    : char s1[7]
//                int &idx
//                const char s[5]
//                int &k
//                boolean_T &b_finite
//                double &nfv
//                boolean_T &foundsign
//                boolean_T &success
// Return Type  : boolean_T
//
static boolean_T readfloat(char s1[7], int &idx, const char s[5], int &k,
                           boolean_T &b_finite, double &nfv,
                           boolean_T &foundsign, boolean_T &success)
{
  int b_idx;
  int b_k;
  boolean_T b_success;
  boolean_T isimag;
  isimag = false;
  b_finite = true;
  nfv = 0.0;
  b_idx = idx;
  b_k = k;
  foundsign = b_copysign(s1, b_idx, s, b_k, b_success);
  idx = b_idx;
  k = b_k;
  success = b_success;
  if (b_success) {
    char c;
    boolean_T exitg1;
    if (isUnitImag(s, b_k)) {
      success = false;
    } else {
      double b_nfv;
      b_success = readNonFinite(s, b_k, b_nfv);
      b_finite = b_success;
      nfv = b_nfv;
      k = b_k;
      if (b_success) {
        b_success = copydigits(s1, idx, s, k, true);
        success = b_success;
        if (b_success) {
          success = copyexponent(s1, idx, s, k);
        }
      } else if ((b_idx >= 2) && (s1[b_idx - 2] == '-')) {
        idx = b_idx - 1;
        s1[b_idx - 2] = ' ';
        nfv = -b_nfv;
      }
      exitg1 = false;
      while ((!exitg1) && (k <= 5)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
      if ((k <= 5) && (s[k - 1] == '*')) {
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 5)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
      }
      if (k <= 5) {
        c = s[k - 1];
        if ((c == 'i') || (c == 'j')) {
          k++;
          isimag = true;
        }
      }
    }
    exitg1 = false;
    while ((!exitg1) && (k <= 5)) {
      c = s[k - 1];
      if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
          (c == '\x00') || (c == ',')) {
        k++;
      } else {
        exitg1 = true;
      }
    }
  }
  return isimag;
}

//
// Arguments    : const char s[2]
// Return Type  : creal_T
//
creal_T b_str2double(const char s[2])
{
  creal_T x;
  double scanned1;
  double scanned2;
  int b_k;
  int idx;
  int k;
  int ntoread;
  char s1[4];
  char p_tmp;
  boolean_T b_finite;
  boolean_T exitg1;
  boolean_T isimag1;
  boolean_T isneg;
  boolean_T success;
  x.re = rtNaN;
  x.im = 0.0;
  ntoread = 0;
  k = 0;
  while ((k + 1 <= 2) &&
         (bv[static_cast<int>(static_cast<unsigned char>(s[k]) & 127U)] ||
          (s[k] == '\x00'))) {
    k++;
  }
  isimag1 = false;
  b_finite = true;
  scanned1 = 0.0;
  idx = 1;
  s1[0] = '\x00';
  s1[1] = '\x00';
  s1[2] = '\x00';
  s1[3] = '\x00';
  isneg = false;
  exitg1 = false;
  while ((!exitg1) && (k + 1 <= 2)) {
    if (s[k] == '-') {
      isneg = !isneg;
      k++;
    } else if ((s[k] == ',') || (s[k] == '+') ||
               bv[static_cast<int>(static_cast<unsigned char>(s[k]) & 127U)]) {
      k++;
    } else {
      exitg1 = true;
    }
  }
  success = (k + 1 <= 2);
  if (success && isneg) {
    s1[0] = '-';
    idx = 2;
  }
  b_k = k + 1;
  if (success) {
    isneg = false;
    if (k + 1 <= 2) {
      isneg = ((s[k] == 'j') || (s[k] == 'i'));
    }
    if (isneg) {
      isimag1 = true;
      b_k = k + 2;
      while ((b_k <= 2) &&
             (bv[static_cast<int>(static_cast<unsigned char>(s[1]) & 127U)] ||
              (s[1] == '\x00') || (s[1] == ','))) {
        b_k = 3;
      }
      if ((b_k <= 2) && (s[1] == '*')) {
        b_k = 3;
        b_readfloat(s1, idx, s, b_k, b_finite, scanned1, isneg, success);
      } else {
        s1[idx - 1] = '1';
        idx++;
      }
    } else {
      b_k = k + 1;
      b_finite = b_readNonFinite(s, b_k, scanned1);
      if (b_finite) {
        success = b_copydigits(s1, idx, s, b_k, true);
        if (success) {
          success = b_copyexponent(s1, idx, s, b_k);
        }
      } else if ((idx >= 2) && (s1[0] == '-')) {
        idx = 1;
        s1[0] = ' ';
        scanned1 = -scanned1;
      }
      exitg1 = false;
      while ((!exitg1) && (b_k <= 2)) {
        p_tmp = s[b_k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(p_tmp) & 127U)] ||
            (p_tmp == '\x00') || (p_tmp == ',')) {
          b_k++;
        } else {
          exitg1 = true;
        }
      }
      if ((b_k <= 2) && (s[b_k - 1] == '*')) {
        b_k++;
        exitg1 = false;
        while ((!exitg1) && (b_k <= 2)) {
          p_tmp = s[b_k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(p_tmp) & 127U)] ||
              (p_tmp == '\x00') || (p_tmp == ',')) {
            b_k++;
          } else {
            exitg1 = true;
          }
        }
      }
      if (b_k <= 2) {
        p_tmp = s[b_k - 1];
        if ((p_tmp == 'i') || (p_tmp == 'j')) {
          b_k++;
          isimag1 = true;
        }
      }
    }
    exitg1 = false;
    while ((!exitg1) && (b_k <= 2)) {
      p_tmp = s[b_k - 1];
      if (bv[static_cast<int>(static_cast<unsigned char>(p_tmp) & 127U)] ||
          (p_tmp == '\x00') || (p_tmp == ',')) {
        b_k++;
      } else {
        exitg1 = true;
      }
    }
  }
  if (b_finite) {
    ntoread = 1;
  }
  if (success && (b_k <= 2)) {
    boolean_T b_success;
    boolean_T c_finite;
    boolean_T foundsign;
    s1[idx - 1] = ' ';
    success = false;
    c_finite = true;
    scanned2 = 0.0;
    k = idx + 1;
    foundsign = c_copysign(s1, k, s, b_k, b_success);
    idx = k;
    if (b_success) {
      isneg = false;
      if (b_k <= 2) {
        p_tmp = s[b_k - 1];
        isneg = ((p_tmp == 'j') || (p_tmp == 'i'));
      }
      if (isneg) {
        success = true;
        b_k++;
        exitg1 = false;
        while ((!exitg1) && (b_k <= 2)) {
          p_tmp = s[b_k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(p_tmp) & 127U)] ||
              (p_tmp == '\x00') || (p_tmp == ',')) {
            b_k++;
          } else {
            exitg1 = true;
          }
        }
        if ((b_k <= 2) && (s[b_k - 1] == '*')) {
          b_k++;
          b_readfloat(s1, idx, s, b_k, c_finite, scanned2, isneg, b_success);
        } else {
          s1[k - 1] = '1';
          idx = k + 1;
        }
      } else {
        c_finite = b_readNonFinite(s, b_k, scanned2);
        if (c_finite) {
          b_success = b_copydigits(s1, idx, s, b_k, true);
          if (b_success) {
            b_success = b_copyexponent(s1, idx, s, b_k);
          }
        } else if ((k >= 2) && (s1[k - 2] == '-')) {
          idx = k - 1;
          s1[k - 2] = ' ';
          scanned2 = -scanned2;
        }
        exitg1 = false;
        while ((!exitg1) && (b_k <= 2)) {
          p_tmp = s[b_k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(p_tmp) & 127U)] ||
              (p_tmp == '\x00') || (p_tmp == ',')) {
            b_k++;
          } else {
            exitg1 = true;
          }
        }
        if ((b_k <= 2) && (s[b_k - 1] == '*')) {
          b_k++;
          exitg1 = false;
          while ((!exitg1) && (b_k <= 2)) {
            p_tmp = s[b_k - 1];
            if (bv[static_cast<int>(static_cast<unsigned char>(p_tmp) &
                                    127U)] ||
                (p_tmp == '\x00') || (p_tmp == ',')) {
              b_k++;
            } else {
              exitg1 = true;
            }
          }
        }
        if (b_k <= 2) {
          p_tmp = s[b_k - 1];
          if ((p_tmp == 'i') || (p_tmp == 'j')) {
            b_k++;
            success = true;
          }
        }
      }
      exitg1 = false;
      while ((!exitg1) && (b_k <= 2)) {
        p_tmp = s[b_k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(p_tmp) & 127U)] ||
            (p_tmp == '\x00') || (p_tmp == ',')) {
          b_k++;
        } else {
          exitg1 = true;
        }
      }
    }
    if (c_finite) {
      ntoread++;
    }
    if (b_success && (b_k > 2) && (isimag1 ^ success) && foundsign) {
      success = true;
    } else {
      success = false;
    }
  } else {
    scanned2 = 0.0;
  }
  if (success) {
    s1[idx - 1] = '\x00';
    if (ntoread == 2) {
      k = std::sscanf(&s1[0], "%lf %lf", &scanned1, &scanned2);
      if (k != 2) {
        scanned1 = rtNaN;
        scanned2 = rtNaN;
      }
    } else if (ntoread == 1) {
      double b_scanned1;
      k = std::sscanf(&s1[0], "%lf", &b_scanned1);
      if (k != 1) {
        b_scanned1 = rtNaN;
      }
      if (b_finite) {
        scanned1 = b_scanned1;
      } else {
        scanned2 = b_scanned1;
      }
    }
    if (isimag1) {
      x.re = scanned2;
      x.im = scanned1;
    } else {
      x.re = scanned1;
      x.im = scanned2;
    }
  }
  return x;
}

//
// Arguments    : const char s[8]
// Return Type  : creal_T
//
creal_T c_str2double(const char s[8])
{
  creal_T x;
  double scanned1;
  double scanned2;
  int idx;
  int k;
  int nread;
  int ntoread;
  char s1[10];
  char c;
  boolean_T b_finite;
  boolean_T exitg1;
  boolean_T isimag1;
  boolean_T isneg;
  boolean_T success;
  x.re = rtNaN;
  x.im = 0.0;
  ntoread = 0;
  k = 1;
  exitg1 = false;
  while ((!exitg1) && (k <= 8)) {
    c = s[k - 1];
    if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
        (c == '\x00')) {
      k++;
    } else {
      exitg1 = true;
    }
  }
  isimag1 = false;
  b_finite = true;
  scanned1 = 0.0;
  idx = 1;
  for (int i{0}; i < 10; i++) {
    s1[i] = '\x00';
  }
  isneg = false;
  exitg1 = false;
  while ((!exitg1) && (k <= 8)) {
    c = s[k - 1];
    if (c == '-') {
      isneg = !isneg;
      k++;
    } else if ((c == ',') || (c == '+') ||
               bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)]) {
      k++;
    } else {
      exitg1 = true;
    }
  }
  success = (k <= 8);
  if (success && isneg) {
    s1[0] = '-';
    idx = 2;
  }
  if (success) {
    if (b_isUnitImag(s, k)) {
      isimag1 = true;
      k++;
      exitg1 = false;
      while ((!exitg1) && (k <= 8)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
      if ((k <= 8) && (s[k - 1] == '*')) {
        k++;
        c_readfloat(s1, idx, s, k, b_finite, scanned1, isneg, success);
      } else {
        s1[idx - 1] = '1';
        idx++;
      }
    } else {
      b_finite = c_readNonFinite(s, k, scanned1);
      if (b_finite) {
        success = c_copydigits(s1, idx, s, k, true);
        if (success) {
          success = c_copyexponent(s1, idx, s, k);
        }
      } else if ((idx >= 2) && (s1[0] == '-')) {
        idx = 1;
        s1[0] = ' ';
        scanned1 = -scanned1;
      }
      exitg1 = false;
      while ((!exitg1) && (k <= 8)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
      if ((k <= 8) && (s[k - 1] == '*')) {
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 8)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
      }
      if (k <= 8) {
        c = s[k - 1];
        if ((c == 'i') || (c == 'j')) {
          k++;
          isimag1 = true;
        }
      }
    }
    exitg1 = false;
    while ((!exitg1) && (k <= 8)) {
      c = s[k - 1];
      if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
          (c == '\x00') || (c == ',')) {
        k++;
      } else {
        exitg1 = true;
      }
    }
  }
  if (b_finite) {
    ntoread = 1;
  }
  if (success && (k <= 8)) {
    boolean_T b_success;
    boolean_T foundsign;
    s1[idx - 1] = ' ';
    isneg = false;
    success = true;
    scanned2 = 0.0;
    nread = idx + 1;
    foundsign = d_copysign(s1, nread, s, k, b_success);
    idx = nread;
    if (b_success) {
      if (b_isUnitImag(s, k)) {
        isneg = true;
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 8)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
        if ((k <= 8) && (s[k - 1] == '*')) {
          boolean_T a__3;
          k++;
          c_readfloat(s1, idx, s, k, success, scanned2, a__3, b_success);
        } else {
          s1[nread - 1] = '1';
          idx = nread + 1;
        }
      } else {
        success = c_readNonFinite(s, k, scanned2);
        if (success) {
          b_success = c_copydigits(s1, idx, s, k, true);
          if (b_success) {
            b_success = c_copyexponent(s1, idx, s, k);
          }
        } else if ((nread >= 2) && (s1[nread - 2] == '-')) {
          idx = nread - 1;
          s1[nread - 2] = ' ';
          scanned2 = -scanned2;
        }
        exitg1 = false;
        while ((!exitg1) && (k <= 8)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
        if ((k <= 8) && (s[k - 1] == '*')) {
          k++;
          exitg1 = false;
          while ((!exitg1) && (k <= 8)) {
            c = s[k - 1];
            if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
                (c == '\x00') || (c == ',')) {
              k++;
            } else {
              exitg1 = true;
            }
          }
        }
        if (k <= 8) {
          c = s[k - 1];
          if ((c == 'i') || (c == 'j')) {
            k++;
            isneg = true;
          }
        }
      }
      exitg1 = false;
      while ((!exitg1) && (k <= 8)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
    }
    if (success) {
      ntoread++;
    }
    if (b_success && (k > 8) && (isimag1 ^ isneg) && foundsign) {
      success = true;
    } else {
      success = false;
    }
  } else {
    scanned2 = 0.0;
  }
  if (success) {
    s1[idx - 1] = '\x00';
    if (ntoread == 2) {
      nread = std::sscanf(&s1[0], "%lf %lf", &scanned1, &scanned2);
      if (nread != 2) {
        scanned1 = rtNaN;
        scanned2 = rtNaN;
      }
    } else if (ntoread == 1) {
      double b_scanned1;
      nread = std::sscanf(&s1[0], "%lf", &b_scanned1);
      if (nread != 1) {
        b_scanned1 = rtNaN;
      }
      if (b_finite) {
        scanned1 = b_scanned1;
      } else {
        scanned2 = b_scanned1;
      }
    }
    if (isimag1) {
      x.re = scanned2;
      x.im = scanned1;
    } else {
      x.re = scanned1;
      x.im = scanned2;
    }
  }
  return x;
}

//
// Arguments    : const char s[9]
// Return Type  : creal_T
//
creal_T d_str2double(const char s[9])
{
  creal_T x;
  double scanned1;
  double scanned2;
  int idx;
  int k;
  int nread;
  int ntoread;
  char s1[11];
  char c;
  boolean_T b_finite;
  boolean_T exitg1;
  boolean_T isimag1;
  boolean_T isneg;
  boolean_T success;
  x.re = rtNaN;
  x.im = 0.0;
  ntoread = 0;
  k = 1;
  exitg1 = false;
  while ((!exitg1) && (k <= 9)) {
    c = s[k - 1];
    if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
        (c == '\x00')) {
      k++;
    } else {
      exitg1 = true;
    }
  }
  isimag1 = false;
  b_finite = true;
  scanned1 = 0.0;
  idx = 1;
  for (int i{0}; i < 11; i++) {
    s1[i] = '\x00';
  }
  isneg = false;
  exitg1 = false;
  while ((!exitg1) && (k <= 9)) {
    c = s[k - 1];
    if (c == '-') {
      isneg = !isneg;
      k++;
    } else if ((c == ',') || (c == '+') ||
               bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)]) {
      k++;
    } else {
      exitg1 = true;
    }
  }
  success = (k <= 9);
  if (success && isneg) {
    s1[0] = '-';
    idx = 2;
  }
  if (success) {
    if (c_isUnitImag(s, k)) {
      isimag1 = true;
      k++;
      exitg1 = false;
      while ((!exitg1) && (k <= 9)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
      if ((k <= 9) && (s[k - 1] == '*')) {
        k++;
        d_readfloat(s1, idx, s, k, b_finite, scanned1, isneg, success);
      } else {
        s1[idx - 1] = '1';
        idx++;
      }
    } else {
      b_finite = d_readNonFinite(s, k, scanned1);
      if (b_finite) {
        success = d_copydigits(s1, idx, s, k, true);
        if (success) {
          success = d_copyexponent(s1, idx, s, k);
        }
      } else if ((idx >= 2) && (s1[0] == '-')) {
        idx = 1;
        s1[0] = ' ';
        scanned1 = -scanned1;
      }
      exitg1 = false;
      while ((!exitg1) && (k <= 9)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
      if ((k <= 9) && (s[k - 1] == '*')) {
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 9)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
      }
      if (k <= 9) {
        c = s[k - 1];
        if ((c == 'i') || (c == 'j')) {
          k++;
          isimag1 = true;
        }
      }
    }
    exitg1 = false;
    while ((!exitg1) && (k <= 9)) {
      c = s[k - 1];
      if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
          (c == '\x00') || (c == ',')) {
        k++;
      } else {
        exitg1 = true;
      }
    }
  }
  if (b_finite) {
    ntoread = 1;
  }
  if (success && (k <= 9)) {
    boolean_T b_success;
    boolean_T foundsign;
    s1[idx - 1] = ' ';
    isneg = false;
    success = true;
    scanned2 = 0.0;
    nread = idx + 1;
    foundsign = e_copysign(s1, nread, s, k, b_success);
    idx = nread;
    if (b_success) {
      if (c_isUnitImag(s, k)) {
        isneg = true;
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 9)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
        if ((k <= 9) && (s[k - 1] == '*')) {
          boolean_T a__3;
          k++;
          d_readfloat(s1, idx, s, k, success, scanned2, a__3, b_success);
        } else {
          s1[nread - 1] = '1';
          idx = nread + 1;
        }
      } else {
        success = d_readNonFinite(s, k, scanned2);
        if (success) {
          b_success = d_copydigits(s1, idx, s, k, true);
          if (b_success) {
            b_success = d_copyexponent(s1, idx, s, k);
          }
        } else if ((nread >= 2) && (s1[nread - 2] == '-')) {
          idx = nread - 1;
          s1[nread - 2] = ' ';
          scanned2 = -scanned2;
        }
        exitg1 = false;
        while ((!exitg1) && (k <= 9)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
        if ((k <= 9) && (s[k - 1] == '*')) {
          k++;
          exitg1 = false;
          while ((!exitg1) && (k <= 9)) {
            c = s[k - 1];
            if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
                (c == '\x00') || (c == ',')) {
              k++;
            } else {
              exitg1 = true;
            }
          }
        }
        if (k <= 9) {
          c = s[k - 1];
          if ((c == 'i') || (c == 'j')) {
            k++;
            isneg = true;
          }
        }
      }
      exitg1 = false;
      while ((!exitg1) && (k <= 9)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
    }
    if (success) {
      ntoread++;
    }
    if (b_success && (k > 9) && (isimag1 ^ isneg) && foundsign) {
      success = true;
    } else {
      success = false;
    }
  } else {
    scanned2 = 0.0;
  }
  if (success) {
    s1[idx - 1] = '\x00';
    if (ntoread == 2) {
      nread = std::sscanf(&s1[0], "%lf %lf", &scanned1, &scanned2);
      if (nread != 2) {
        scanned1 = rtNaN;
        scanned2 = rtNaN;
      }
    } else if (ntoread == 1) {
      double b_scanned1;
      nread = std::sscanf(&s1[0], "%lf", &b_scanned1);
      if (nread != 1) {
        b_scanned1 = rtNaN;
      }
      if (b_finite) {
        scanned1 = b_scanned1;
      } else {
        scanned2 = b_scanned1;
      }
    }
    if (isimag1) {
      x.re = scanned2;
      x.im = scanned1;
    } else {
      x.re = scanned1;
      x.im = scanned2;
    }
  }
  return x;
}

//
// Arguments    : const char s[11]
// Return Type  : creal_T
//
creal_T e_str2double(const char s[11])
{
  creal_T x;
  double scanned1;
  double scanned2;
  int idx;
  int k;
  int nread;
  int ntoread;
  char s1[13];
  char c;
  boolean_T b_finite;
  boolean_T exitg1;
  boolean_T isimag1;
  boolean_T isneg;
  boolean_T success;
  x.re = rtNaN;
  x.im = 0.0;
  ntoread = 0;
  k = 1;
  exitg1 = false;
  while ((!exitg1) && (k <= 11)) {
    c = s[k - 1];
    if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
        (c == '\x00')) {
      k++;
    } else {
      exitg1 = true;
    }
  }
  isimag1 = false;
  b_finite = true;
  scanned1 = 0.0;
  idx = 1;
  for (int i{0}; i < 13; i++) {
    s1[i] = '\x00';
  }
  isneg = false;
  exitg1 = false;
  while ((!exitg1) && (k <= 11)) {
    c = s[k - 1];
    if (c == '-') {
      isneg = !isneg;
      k++;
    } else if ((c == ',') || (c == '+') ||
               bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)]) {
      k++;
    } else {
      exitg1 = true;
    }
  }
  success = (k <= 11);
  if (success && isneg) {
    s1[0] = '-';
    idx = 2;
  }
  if (success) {
    if (d_isUnitImag(s, k)) {
      isimag1 = true;
      k++;
      exitg1 = false;
      while ((!exitg1) && (k <= 11)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
      if ((k <= 11) && (s[k - 1] == '*')) {
        k++;
        e_readfloat(s1, idx, s, k, b_finite, scanned1, isneg, success);
      } else {
        s1[idx - 1] = '1';
        idx++;
      }
    } else {
      b_finite = e_readNonFinite(s, k, scanned1);
      if (b_finite) {
        success = e_copydigits(s1, idx, s, k, true);
        if (success) {
          success = e_copyexponent(s1, idx, s, k);
        }
      } else if ((idx >= 2) && (s1[0] == '-')) {
        idx = 1;
        s1[0] = ' ';
        scanned1 = -scanned1;
      }
      exitg1 = false;
      while ((!exitg1) && (k <= 11)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
      if ((k <= 11) && (s[k - 1] == '*')) {
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 11)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
      }
      if (k <= 11) {
        c = s[k - 1];
        if ((c == 'i') || (c == 'j')) {
          k++;
          isimag1 = true;
        }
      }
    }
    exitg1 = false;
    while ((!exitg1) && (k <= 11)) {
      c = s[k - 1];
      if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
          (c == '\x00') || (c == ',')) {
        k++;
      } else {
        exitg1 = true;
      }
    }
  }
  if (b_finite) {
    ntoread = 1;
  }
  if (success && (k <= 11)) {
    boolean_T b_success;
    boolean_T foundsign;
    s1[idx - 1] = ' ';
    isneg = false;
    success = true;
    scanned2 = 0.0;
    nread = idx + 1;
    foundsign = f_copysign(s1, nread, s, k, b_success);
    idx = nread;
    if (b_success) {
      if (d_isUnitImag(s, k)) {
        isneg = true;
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 11)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
        if ((k <= 11) && (s[k - 1] == '*')) {
          boolean_T a__3;
          k++;
          e_readfloat(s1, idx, s, k, success, scanned2, a__3, b_success);
        } else {
          s1[nread - 1] = '1';
          idx = nread + 1;
        }
      } else {
        success = e_readNonFinite(s, k, scanned2);
        if (success) {
          b_success = e_copydigits(s1, idx, s, k, true);
          if (b_success) {
            b_success = e_copyexponent(s1, idx, s, k);
          }
        } else if ((nread >= 2) && (s1[nread - 2] == '-')) {
          idx = nread - 1;
          s1[nread - 2] = ' ';
          scanned2 = -scanned2;
        }
        exitg1 = false;
        while ((!exitg1) && (k <= 11)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
        if ((k <= 11) && (s[k - 1] == '*')) {
          k++;
          exitg1 = false;
          while ((!exitg1) && (k <= 11)) {
            c = s[k - 1];
            if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
                (c == '\x00') || (c == ',')) {
              k++;
            } else {
              exitg1 = true;
            }
          }
        }
        if (k <= 11) {
          c = s[k - 1];
          if ((c == 'i') || (c == 'j')) {
            k++;
            isneg = true;
          }
        }
      }
      exitg1 = false;
      while ((!exitg1) && (k <= 11)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
    }
    if (success) {
      ntoread++;
    }
    if (b_success && (k > 11) && (isimag1 ^ isneg) && foundsign) {
      success = true;
    } else {
      success = false;
    }
  } else {
    scanned2 = 0.0;
  }
  if (success) {
    s1[idx - 1] = '\x00';
    if (ntoread == 2) {
      nread = std::sscanf(&s1[0], "%lf %lf", &scanned1, &scanned2);
      if (nread != 2) {
        scanned1 = rtNaN;
        scanned2 = rtNaN;
      }
    } else if (ntoread == 1) {
      double b_scanned1;
      nread = std::sscanf(&s1[0], "%lf", &b_scanned1);
      if (nread != 1) {
        b_scanned1 = rtNaN;
      }
      if (b_finite) {
        scanned1 = b_scanned1;
      } else {
        scanned2 = b_scanned1;
      }
    }
    if (isimag1) {
      x.re = scanned2;
      x.im = scanned1;
    } else {
      x.re = scanned1;
      x.im = scanned2;
    }
  }
  return x;
}

//
// Arguments    : const char s[5]
// Return Type  : creal_T
//
creal_T str2double(const char s[5])
{
  creal_T x;
  double scanned1;
  double scanned2;
  int idx;
  int k;
  int nread;
  int ntoread;
  char s1[7];
  char c;
  boolean_T b_finite;
  boolean_T exitg1;
  boolean_T isimag1;
  boolean_T isneg;
  boolean_T success;
  x.re = rtNaN;
  x.im = 0.0;
  ntoread = 0;
  k = 1;
  exitg1 = false;
  while ((!exitg1) && (k <= 5)) {
    c = s[k - 1];
    if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
        (c == '\x00')) {
      k++;
    } else {
      exitg1 = true;
    }
  }
  isimag1 = false;
  b_finite = true;
  scanned1 = 0.0;
  idx = 1;
  for (int i{0}; i < 7; i++) {
    s1[i] = '\x00';
  }
  isneg = false;
  exitg1 = false;
  while ((!exitg1) && (k <= 5)) {
    c = s[k - 1];
    if (c == '-') {
      isneg = !isneg;
      k++;
    } else if ((c == ',') || (c == '+') ||
               bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)]) {
      k++;
    } else {
      exitg1 = true;
    }
  }
  success = (k <= 5);
  if (success && isneg) {
    s1[0] = '-';
    idx = 2;
  }
  if (success) {
    if (isUnitImag(s, k)) {
      isimag1 = true;
      k++;
      exitg1 = false;
      while ((!exitg1) && (k <= 5)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
      if ((k <= 5) && (s[k - 1] == '*')) {
        k++;
        readfloat(s1, idx, s, k, b_finite, scanned1, isneg, success);
      } else {
        s1[idx - 1] = '1';
        idx++;
      }
    } else {
      b_finite = readNonFinite(s, k, scanned1);
      if (b_finite) {
        success = copydigits(s1, idx, s, k, true);
        if (success) {
          success = copyexponent(s1, idx, s, k);
        }
      } else if ((idx >= 2) && (s1[0] == '-')) {
        idx = 1;
        s1[0] = ' ';
        scanned1 = -scanned1;
      }
      exitg1 = false;
      while ((!exitg1) && (k <= 5)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
      if ((k <= 5) && (s[k - 1] == '*')) {
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 5)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
      }
      if (k <= 5) {
        c = s[k - 1];
        if ((c == 'i') || (c == 'j')) {
          k++;
          isimag1 = true;
        }
      }
    }
    exitg1 = false;
    while ((!exitg1) && (k <= 5)) {
      c = s[k - 1];
      if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
          (c == '\x00') || (c == ',')) {
        k++;
      } else {
        exitg1 = true;
      }
    }
  }
  if (b_finite) {
    ntoread = 1;
  }
  if (success && (k <= 5)) {
    boolean_T b_success;
    boolean_T foundsign;
    s1[idx - 1] = ' ';
    isneg = false;
    success = true;
    scanned2 = 0.0;
    nread = idx + 1;
    foundsign = b_copysign(s1, nread, s, k, b_success);
    idx = nread;
    if (b_success) {
      if (isUnitImag(s, k)) {
        isneg = true;
        k++;
        exitg1 = false;
        while ((!exitg1) && (k <= 5)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
        if ((k <= 5) && (s[k - 1] == '*')) {
          boolean_T a__3;
          k++;
          readfloat(s1, idx, s, k, success, scanned2, a__3, b_success);
        } else {
          s1[nread - 1] = '1';
          idx = nread + 1;
        }
      } else {
        success = readNonFinite(s, k, scanned2);
        if (success) {
          b_success = copydigits(s1, idx, s, k, true);
          if (b_success) {
            b_success = copyexponent(s1, idx, s, k);
          }
        } else if ((nread >= 2) && (s1[nread - 2] == '-')) {
          idx = nread - 1;
          s1[nread - 2] = ' ';
          scanned2 = -scanned2;
        }
        exitg1 = false;
        while ((!exitg1) && (k <= 5)) {
          c = s[k - 1];
          if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
              (c == '\x00') || (c == ',')) {
            k++;
          } else {
            exitg1 = true;
          }
        }
        if ((k <= 5) && (s[k - 1] == '*')) {
          k++;
          exitg1 = false;
          while ((!exitg1) && (k <= 5)) {
            c = s[k - 1];
            if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
                (c == '\x00') || (c == ',')) {
              k++;
            } else {
              exitg1 = true;
            }
          }
        }
        if (k <= 5) {
          c = s[k - 1];
          if ((c == 'i') || (c == 'j')) {
            k++;
            isneg = true;
          }
        }
      }
      exitg1 = false;
      while ((!exitg1) && (k <= 5)) {
        c = s[k - 1];
        if (bv[static_cast<int>(static_cast<unsigned char>(c) & 127U)] ||
            (c == '\x00') || (c == ',')) {
          k++;
        } else {
          exitg1 = true;
        }
      }
    }
    if (success) {
      ntoread++;
    }
    if (b_success && (k > 5) && (isimag1 ^ isneg) && foundsign) {
      success = true;
    } else {
      success = false;
    }
  } else {
    scanned2 = 0.0;
  }
  if (success) {
    s1[idx - 1] = '\x00';
    if (ntoread == 2) {
      nread = std::sscanf(&s1[0], "%lf %lf", &scanned1, &scanned2);
      if (nread != 2) {
        scanned1 = rtNaN;
        scanned2 = rtNaN;
      }
    } else if (ntoread == 1) {
      double b_scanned1;
      nread = std::sscanf(&s1[0], "%lf", &b_scanned1);
      if (nread != 1) {
        b_scanned1 = rtNaN;
      }
      if (b_finite) {
        scanned1 = b_scanned1;
      } else {
        scanned2 = b_scanned1;
      }
    }
    if (isimag1) {
      x.re = scanned2;
      x.im = scanned1;
    } else {
      x.re = scanned1;
      x.im = scanned2;
    }
  }
  return x;
}

} // namespace internal
} // namespace coder
} // namespace adcs

//
// File trailer for str2double.cpp
//
// [EOF]
//
