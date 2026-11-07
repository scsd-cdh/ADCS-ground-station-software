//
// Sponsored License - for use in support of a program or activity
// sponsored by MathWorks.  Not for government, commercial or other
// non-sponsored organizational use.
// File: AdcsCore.cpp
//
// MATLAB Coder version            : 25.2
// C/C++ source code generated on  : 26-Sep-2026 11:30:14
//

// Include Files
#include "AdcsCore.h"
#include "atan2.h"
#include "cat.h"
#include "cos.h"
#include "rem.h"
#include "rt_nonfinite.h"
#include "sin.h"
#include "str2double.h"
#include "tleToSatrec_data.h"
#include "tleToSatrec_types.h"
#include "coder_array.h"
#include "omp.h"
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <emmintrin.h>

// Function Declarations
namespace adcs {
static double rt_powd_snf(double u0, double u1);

} // namespace adcs

// Function Definitions
namespace adcs {
//
// Arguments    : double u0
//                double u1
// Return Type  : double
//
static double rt_powd_snf(double u0, double u1)
{
  double y;
  if (std::isnan(u0) || std::isnan(u1)) {
    y = rtNaN;
  } else {
    double d;
    y = std::abs(u0);
    d = std::abs(u1);
    if (std::isinf(u1)) {
      if (y == 1.0) {
        y = 1.0;
      } else if (y > 1.0) {
        if (u1 > 0.0) {
          y = rtInf;
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = rtInf;
      }
    } else if (d == 0.0) {
      y = 1.0;
    } else if (d == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = std::sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > std::floor(u1))) {
      y = rtNaN;
    } else {
      y = std::pow(u0, u1);
    }
  }
  return y;
}

//
// Arguments    : void
// Return Type  : void
//
AdcsCore::AdcsCore()
{
  omp_init_nest_lock(&tleToSatrec_nestLockGlobal);
}

//
// Arguments    : void
// Return Type  : void
//
AdcsCore::~AdcsCore()
{
  omp_destroy_nest_lock(&tleToSatrec_nestLockGlobal);
}

//
// SGP4INIT  Initialise near-Earth SGP4 (period < 225 min) from mean elements.
//    Inputs (TLE mean elements): ecco [-], inclo/nodeo/argpo/mo [rad],
//    noKozai [rad/min] (Kozai mean motion as in the TLE), bstar [1/Earth
//    radii]. satrec.error: 0 ok, 1 bad elements, 7 deep-space orbit (not
//    implemented). Port of Vallado's reference implementation (opsmode 'i'),
//    validated against the published SGP4-VER test vectors (see tests/test_sgp4.cpp).
//
// Arguments    : double ecco
//                double inclo
//                double nodeo
//                double argpo
//                double mo
//                double noKozai
//                double bstar
//                struct0_T *satrec
// Return Type  : void
//
void AdcsCore::sgp4init(double ecco, double inclo, double nodeo, double argpo,
                        double mo, double noKozai, double bstar,
                        struct0_T *satrec)
{
  satrec->error = 0.0;
  satrec->ecco = ecco;
  satrec->inclo = inclo;
  satrec->nodeo = nodeo;
  satrec->argpo = argpo;
  satrec->mo = mo;
  satrec->no = 0.0;
  satrec->bstar = bstar;
  satrec->con41 = 0.0;
  satrec->isimp = 0.0;
  satrec->eta = 0.0;
  satrec->cc1 = 0.0;
  satrec->cc4 = 0.0;
  satrec->cc5 = 0.0;
  satrec->x1mth2 = 0.0;
  satrec->mdot = 0.0;
  satrec->argpdot = 0.0;
  satrec->nodedot = 0.0;
  satrec->omgcof = 0.0;
  satrec->xmcof = 0.0;
  satrec->nodecf = 0.0;
  satrec->t2cof = 0.0;
  satrec->xlcof = 0.0;
  satrec->aycof = 0.0;
  satrec->delmo = 0.0;
  satrec->sinmao = 0.0;
  satrec->x7thm1 = 0.0;
  satrec->d2 = 0.0;
  satrec->d3 = 0.0;
  satrec->d4 = 0.0;
  satrec->t3cof = 0.0;
  satrec->t4cof = 0.0;
  satrec->t5cof = 0.0;
  if ((noKozai <= 0.0) || (ecco < 0.0) || (ecco >= 1.0)) {
    satrec->error = 1.0;
  } else {
    double ak;
    double ao;
    double con41;
    double con42;
    double cosio;
    double cosio2;
    double d1;
    double del1;
    double eccsq;
    double no;
    double rteosq;
    double sinio;
    //  ---- initl: un-Kozai the mean motion, auxiliary quantities ----
    eccsq = ecco * ecco;
    rteosq = std::sqrt(std::fmax(1.0 - eccsq, 0.0));
    //  ecco < 1 is guaranteed above; keeps codegen real-valued
    cosio = std::cos(inclo);
    cosio2 = cosio * cosio;
    ak = rt_powd_snf(0.074366916133173422 / noKozai, 0.66666666666666663);
    d1 = 0.00081196200000000006 * (3.0 * cosio2 - 1.0) /
         (rteosq * (1.0 - eccsq));
    del1 = d1 / (ak * ak);
    ak *= (1.0 - del1 * del1) -
          del1 * (134.0 * del1 * del1 / 81.0 + 0.33333333333333331);
    no = noKozai / (d1 / (ak * ak) + 1.0);
    satrec->no = no;
    ao = rt_powd_snf(0.074366916133173422 / no, 0.66666666666666663);
    sinio = std::sin(inclo);
    del1 = ao * (1.0 - eccsq);
    con42 = 1.0 - 5.0 * cosio2;
    con41 = (-con42 - cosio2) - cosio2;
    ak = ao * (1.0 - ecco);
    satrec->con41 = con41;
    if (6.2831853071795862 / no >= 225.0) {
      satrec->error = 7.0;
      //  deep-space (SDP4) not implemented
    } else {
      double cc1;
      double cc3;
      double coef1;
      double cosio4;
      double eeta;
      double eta;
      double etasq;
      double psisq;
      double sfour;
      double temp1;
      double temp2;
      double temp2_tmp;
      double temp3;
      double tsi;
      double xhdot1;
      double xlcof;
      double xmcof;
      //  ---- near-Earth initialisation ----
      satrec->isimp = (ak < 1.0344928415594841);
      sfour = 1.0122292801892716;
      d1 = 1.8802791590152709E-9;
      ak = (ak - 1.0) * 6378.135;
      if (ak < 156.0) {
        sfour = ak - 78.0;
        if (ak < 98.0) {
          sfour = 20.0;
        }
        d1 = rt_powd_snf((120.0 - sfour) / 6378.135, 4.0);
        sfour = sfour / 6378.135 + 1.0;
      }
      ak = 1.0 / (del1 * del1);
      tsi = 1.0 / (ao - sfour);
      eta = ao * ecco * tsi;
      etasq = eta * eta;
      eeta = ecco * eta;
      psisq = std::abs(1.0 - etasq);
      del1 = d1 * rt_powd_snf(tsi, 4.0);
      coef1 = del1 / rt_powd_snf(psisq, 3.5);
      cc1 = bstar * (coef1 * no *
                     (ao * ((1.5 * etasq + 1.0) + eeta * (etasq + 4.0)) +
                      0.00040598100000000003 * tsi / psisq * con41 *
                          (3.0 * etasq * (etasq + 8.0) + 8.0)));
      cc3 = 0.0;
      if (ecco > 0.0001) {
        cc3 = -2.0 * del1 * tsi * -0.0023450697200115278 * no * sinio / ecco;
      }
      cosio4 = cosio2 * cosio2;
      temp1 = 0.0016239240000000001 * ak * no;
      temp2_tmp = 0.5 * temp1;
      temp2 = temp2_tmp * 0.001082616 * ak;
      temp3 = 7.762359375E-7 * ak * ak * no;
      xhdot1 = -temp1 * cosio;
      xmcof = 0.0;
      if (ecco > 0.0001) {
        xmcof = -0.66666666666666663 * del1 * bstar / eeta;
      }
      if (cosio + 1.0 > 1.5E-12) {
        xlcof =
            0.000586267430002882 * sinio * (5.0 * cosio + 3.0) / (cosio + 1.0);
      } else {
        xlcof = 0.000586267430002882 * sinio * (5.0 * cosio + 3.0) / 1.5E-12;
      }
      if (satrec->isimp != 1.0) {
        double d3;
        del1 = cc1 * cc1;
        d1 = 4.0 * ao * tsi * del1;
        ak = d1 * tsi * cc1 / 3.0;
        d3 = (17.0 * ao + sfour) * ak;
        ak = 0.5 * ak * ao * tsi * (221.0 * ao + 31.0 * sfour) * cc1;
        satrec->t3cof = d1 + 2.0 * del1;
        satrec->t4cof = 0.25 * (3.0 * d3 + cc1 * (12.0 * d1 + 10.0 * del1));
        satrec->t5cof = 0.2 * (((3.0 * ak + 12.0 * cc1 * d3) + 6.0 * d1 * d1) +
                               15.0 * del1 * (2.0 * d1 + del1));
        satrec->d2 = d1;
        satrec->d3 = d3;
        satrec->d4 = ak;
      }
      satrec->eta = eta;
      satrec->cc1 = cc1;
      ak = 2.0 * etasq;
      satrec->cc4 =
          2.0 * no * coef1 * ao * (1.0 - eccsq) *
          ((eta * (0.5 * etasq + 2.0) + ecco * (ak + 0.5)) -
           0.001082616 * tsi / (ao * psisq) *
               (-3.0 * con41 *
                    ((1.0 - 2.0 * eeta) + etasq * (1.5 - 0.5 * eeta)) +
                0.75 * (1.0 - cosio2) * (ak - eeta * (etasq + 1.0)) *
                    std::cos(2.0 * argpo)));
      satrec->cc5 = 2.0 * coef1 * ao * (1.0 - eccsq) *
                    ((2.75 * (etasq + eeta) + 1.0) + eeta * etasq);
      satrec->x1mth2 = 1.0 - cosio2;
      ak = 0.0625 * temp2;
      satrec->mdot = (no + temp2_tmp * rteosq * con41) +
                     ak * rteosq * ((13.0 - 78.0 * cosio2) + 137.0 * cosio4);
      satrec->argpdot = (-0.5 * temp1 * con42 +
                         ak * ((7.0 - 114.0 * cosio2) + 395.0 * cosio4)) +
                        temp3 * ((3.0 - 36.0 * cosio2) + 49.0 * cosio4);
      ak = 7.0 * cosio2;
      satrec->nodedot = xhdot1 + (0.5 * temp2 * (4.0 - 19.0 * cosio2) +
                                  2.0 * temp3 * (3.0 - ak)) *
                                     cosio;
      satrec->omgcof = bstar * cc3 * std::cos(argpo);
      satrec->xmcof = xmcof;
      satrec->nodecf = 3.5 * (1.0 - eccsq) * xhdot1 * cc1;
      satrec->t2cof = 1.5 * cc1;
      satrec->xlcof = xlcof;
      satrec->aycof = 0.0011725348600057639 * sinio;
      satrec->delmo = rt_powd_snf(eta * std::cos(mo) + 1.0, 3.0);
      satrec->sinmao = std::sin(mo);
      satrec->x7thm1 = ak - 1.0;
    }
  }
}

//
// SGP4PROP  Propagate to tsince [minutes from epoch].
//    r [km], v [km/s] in TEME (1x3 each). err: 0 ok, 1 eccentricity out of
//    range, 2 mean motion <= 0, 4 negative semi-latus rectum, 6 decayed.
//
// Arguments    : const struct0_T *satrec
//                double tsince
//                double r[3]
//                double v[3]
//                double *err
// Return Type  : void
//
void AdcsCore::sgp4prop(const struct0_T *satrec, double tsince, double r[3],
                        double v[3], double *err)
{
  r[0] = 0.0;
  v[0] = 0.0;
  r[1] = 0.0;
  v[1] = 0.0;
  r[2] = 0.0;
  v[2] = 0.0;
  *err = 0.0;
  if (satrec->error != 0.0) {
    *err = satrec->error;
  } else {
    double argpdf;
    double nodem;
    double t2;
    double t3;
    double t4;
    double temp;
    double tempa;
    double tempe;
    double templ;
    double xmdf;
    //  ---- secular gravity and atmospheric drag ----
    xmdf = satrec->mo + satrec->mdot * tsince;
    argpdf = satrec->argpo + satrec->argpdot * tsince;
    t2 = tsince * tsince;
    nodem = (satrec->nodeo + satrec->nodedot * tsince) + satrec->nodecf * t2;
    tempa = 1.0 - satrec->cc1 * tsince;
    tempe = satrec->bstar * satrec->cc4 * tsince;
    templ = satrec->t2cof * t2;
    if (satrec->isimp != 1.0) {
      temp = satrec->omgcof * tsince +
             satrec->xmcof *
                 (rt_powd_snf(satrec->eta * std::cos(xmdf) + 1.0, 3.0) -
                  satrec->delmo);
      xmdf += temp;
      argpdf -= temp;
      t3 = t2 * tsince;
      t4 = t3 * tsince;
      tempa = ((tempa - satrec->d2 * t2) - satrec->d3 * t3) - satrec->d4 * t4;
      tempe += satrec->bstar * satrec->cc5 * (std::sin(xmdf) - satrec->sinmao);
      templ = (templ + satrec->t3cof * t3) +
              t4 * (satrec->t4cof + tsince * satrec->t5cof);
    }
    if (satrec->no <= 0.0) {
      *err = 2.0;
    } else {
      double am;
      am = rt_powd_snf(0.074366916133173422 / satrec->no, 0.66666666666666663) *
           tempa * tempa;
      t4 = satrec->ecco - tempe;
      if ((t4 >= 1.0) || (t4 < -0.001)) {
        *err = 1.0;
      } else {
        double axnl;
        double esine;
        double pl;
        int ktr;
        if (t4 < 1.0E-6) {
          t4 = 1.0E-6;
        }
        xmdf += satrec->no * templ;
        t3 = (xmdf + argpdf) + nodem;
        nodem = coder::b_rem(nodem);
        argpdf = coder::b_rem(argpdf);
        t3 = coder::b_rem(t3);
        //  ---- long-period periodics ----
        axnl = t4 * std::cos(argpdf);
        temp = 1.0 / (am * (1.0 - t4 * t4));
        xmdf = t4 * std::sin(argpdf) + temp * satrec->aycof;
        //  ---- Kepler's equation ----
        t3 = coder::b_rem(
            (((coder::b_rem((t3 - argpdf) - nodem) + argpdf) + nodem) +
             temp * satrec->xlcof * axnl) -
            nodem);
        t4 = t3;
        t2 = 9999.9;
        ktr = 1;
        tempa = 0.0;
        tempe = 1.0;
        while ((std::abs(t2) >= 1.0E-12) && (ktr <= 10)) {
          tempa = std::sin(t4);
          tempe = std::cos(t4);
          t2 = (((t3 - xmdf * tempe) + axnl * tempa) - t4) /
               ((1.0 - tempe * axnl) - tempa * xmdf);
          if (std::abs(t2) >= 0.95) {
            if (t2 > 0.0) {
              t2 = 0.95;
            } else {
              t2 = -0.95;
            }
          }
          t4 += t2;
          ktr++;
        }
        //  ---- short-period preliminary quantities ----
        esine = axnl * tempa - xmdf * tempe;
        t3 = axnl * axnl + xmdf * xmdf;
        pl = am * (1.0 - t3);
        if (pl < 0.0) {
          *err = 4.0;
        } else {
          double mrt;
          double rl;
          rl = am * (1.0 - (axnl * tempe + xmdf * tempa));
          templ = std::sqrt(1.0 - t3);
          temp = esine / (templ + 1.0);
          t3 = am / rl;
          tempa = t3 * ((tempa - xmdf) - axnl * temp);
          t4 = t3 * ((tempe - axnl) + xmdf * temp);
          xmdf = (t4 + t4) * tempa;
          argpdf = 1.0 - 2.0 * tempa * tempa;
          temp = 1.0 / pl;
          tempe = 0.000541308 * temp;
          t3 = tempe * temp;
          //  ---- short-period periodics ----
          t2 = 1.5 * t3;
          mrt = rl * (1.0 - t2 * templ * satrec->con41) +
                0.5 * tempe * satrec->x1mth2 * argpdf;
          t4 = coder::b_atan2(tempa, t4) - 0.25 * t3 * satrec->x7thm1 * xmdf;
          t3 = t2 * std::cos(satrec->inclo);
          t2 = nodem + t3 * xmdf;
          tempa = satrec->inclo + t3 * std::sin(satrec->inclo) * argpdf;
          t3 = 0.074366916133173422 / rt_powd_snf(am, 1.5) * tempe;
          am = std::sqrt(am) * esine / rl -
               t3 * satrec->x1mth2 * xmdf / 0.074366916133173422;
          temp = std::sqrt(pl) / rl +
                 t3 * (satrec->x1mth2 * argpdf + 1.5 * satrec->con41) /
                     0.074366916133173422;
          //  ---- orientation vectors ----
          esine = std::sin(t4);
          argpdf = std::cos(t4);
          axnl = std::sin(t2);
          xmdf = std::cos(t2);
          templ = std::sin(tempa);
          t3 = std::cos(tempa);
          tempe = -axnl * t3;
          t3 *= xmdf;
          t4 = tempe * esine + xmdf * argpdf;
          t2 = t3 * esine + axnl * argpdf;
          tempa = templ * esine;
          if (mrt < 1.0) {
            *err = 6.0;
          } else {
            r[0] = mrt * t4 * 6378.135;
            r[1] = mrt * t2 * 6378.135;
            r[2] = mrt * tempa * 6378.135;
            v[0] = (am * t4 + temp * (tempe * argpdf - xmdf * esine)) *
                   7.905370510517634;
            v[1] = (am * t2 + temp * (t3 * argpdf - axnl * esine)) *
                   7.905370510517634;
            v[2] = (am * tempa + temp * (templ * argpdf)) * 7.905370510517634;
          }
        }
      }
    }
  }
}

//
// TEMETOECEF  TEME -> ECEF (rotation by GMST; polar motion ignored, UT1 ~ UTC).
//    rTeme, vTeme: Nx3 (any length unit, velocity per second). jdUt1: Nx1.
//    Neglecting UT1-UTC (<0.9 s) and polar motion costs at most a few hundred
//    metres, well inside SGP4's own ~1 km accuracy.
//
// Arguments    : const ::coder::array<double, 2U> &rTeme
//                const ::coder::array<double, 2U> &vTeme
//                const ::coder::array<double, 1U> &jdUt1
//                ::coder::array<double, 2U> &rEcef
//                ::coder::array<double, 2U> &vEcef
// Return Type  : void
//
void AdcsCore::temeToEcef(const ::coder::array<double, 2U> &rTeme,
                          const ::coder::array<double, 2U> &vTeme,
                          const ::coder::array<double, 1U> &jdUt1,
                          ::coder::array<double, 2U> &rEcef,
                          ::coder::array<double, 2U> &vEcef)
{
  __m128d r1;
  __m128d r2;
  ::coder::array<double, 2U> vp;
  ::coder::array<double, 1U> c;
  ::coder::array<double, 1U> r;
  ::coder::array<double, 1U> tut1;
  double b_varargin_1;
  double d_varargin_1;
  int loop_ub;
  int scalarLB;
  int vectorUB;
  //  IAU-82 Greenwich mean sidereal time [rad].
  loop_ub = jdUt1.size(0);
  tut1.set_size(jdUt1.size(0));
  scalarLB = (jdUt1.size(0) / 2) << 1;
  vectorUB = scalarLB - 2;
  for (int i{0}; i <= vectorUB; i += 2) {
    _mm_storeu_pd(&tut1[i], _mm_div_pd(_mm_sub_pd(_mm_loadu_pd(&jdUt1[i]),
                                                  _mm_set1_pd(2.451545E+6)),
                                       _mm_set1_pd(36525.0)));
  }
  for (int i{scalarLB}; i < loop_ub; i++) {
    tut1[i] = (jdUt1[i] - 2.451545E+6) / 36525.0;
  }
  loop_ub = tut1.size(0);
  if (static_cast<int>(tut1.size(0) < 1600)) {
    for (int i1{0}; i1 < loop_ub; i1++) {
      double c_varargin_1;
      double varargin_1;
      varargin_1 = tut1[i1];
      c_varargin_1 = tut1[i1];
      varargin_1 =
          0.017453292519943295 * ((((-6.2E-6 * rt_powd_snf(varargin_1, 3.0) +
                                     0.093104 * (c_varargin_1 * c_varargin_1)) +
                                    3.1644001848128662E+9 * tut1[i1]) +
                                   67310.54841) /
                                  240.0);
      if (std::isnan(varargin_1) || std::isinf(varargin_1)) {
        varargin_1 = rtNaN;
      } else {
        c_varargin_1 = std::abs(varargin_1 / 6.2831853071795862);
        if (!(std::abs(c_varargin_1 - std::floor(c_varargin_1 + 0.5)) >
              DBL_EPSILON * c_varargin_1)) {
          varargin_1 *= 0.0;
        } else {
          varargin_1 = std::fmod(varargin_1, 6.2831853071795862);
        }
      }
      tut1[i1] = varargin_1;
    }
  } else {
#pragma omp parallel for num_threads(omp_get_max_threads()) private(           \
        b_varargin_1, d_varargin_1)

    for (int i1 = 0; i1 < loop_ub; i1++) {
      b_varargin_1 = tut1[i1];
      d_varargin_1 = tut1[i1];
      b_varargin_1 =
          0.017453292519943295 * ((((-6.2E-6 * rt_powd_snf(b_varargin_1, 3.0) +
                                     0.093104 * (d_varargin_1 * d_varargin_1)) +
                                    3.1644001848128662E+9 * tut1[i1]) +
                                   67310.54841) /
                                  240.0);
      if (std::isnan(b_varargin_1) || std::isinf(b_varargin_1)) {
        b_varargin_1 = rtNaN;
      } else {
        d_varargin_1 = std::abs(b_varargin_1 / 6.2831853071795862);
        if (!(std::abs(d_varargin_1 - std::floor(d_varargin_1 + 0.5)) >
              DBL_EPSILON * d_varargin_1)) {
          b_varargin_1 *= 0.0;
        } else {
          b_varargin_1 = std::fmod(b_varargin_1, 6.2831853071795862);
        }
      }
      tut1[i1] = b_varargin_1;
    }
  }
  c.set_size(loop_ub);
  if (loop_ub - 1 >= 0) {
    std::copy(&tut1[0], &tut1[loop_ub], &c[0]);
  }
  coder::b_cos(c);
  coder::b_sin(tut1);
  loop_ub = tut1.size(0);
  r.set_size(tut1.size(0));
  scalarLB = (tut1.size(0) / 2) << 1;
  vectorUB = scalarLB - 2;
  for (int i{0}; i <= vectorUB; i += 2) {
    r1 = _mm_loadu_pd(&tut1[i]);
    _mm_storeu_pd(&r[i], _mm_mul_pd(r1, _mm_set1_pd(-1.0)));
  }
  for (int i{scalarLB}; i < loop_ub; i++) {
    r[i] = -tut1[i];
  }
  if (c.size(0) == 1) {
    loop_ub = rTeme.size(0);
  } else {
    loop_ub = c.size(0);
  }
  if (tut1.size(0) == 1) {
    scalarLB = rTeme.size(0);
  } else {
    scalarLB = tut1.size(0);
  }
  if (r.size(0) == 1) {
    vectorUB = rTeme.size(0);
  } else {
    vectorUB = r.size(0);
  }
  if ((c.size(0) == rTeme.size(0)) && (tut1.size(0) == rTeme.size(0)) &&
      (loop_ub == scalarLB) && (r.size(0) == rTeme.size(0)) &&
      (c.size(0) == rTeme.size(0)) && (vectorUB == loop_ub)) {
    loop_ub = c.size(0);
    rEcef.set_size(c.size(0), 3);
    scalarLB = (c.size(0) / 2) << 1;
    vectorUB = scalarLB - 2;
    for (int i{0}; i <= vectorUB; i += 2) {
      r1 = _mm_loadu_pd(&c[i]);
      r2 = _mm_loadu_pd(&tut1[i]);
      _mm_storeu_pd(
          &rEcef[i],
          _mm_add_pd(_mm_mul_pd(r1, _mm_loadu_pd(&rTeme[i])),
                     _mm_mul_pd(r2, _mm_loadu_pd(&rTeme[i + rTeme.size(0)]))));
    }
    for (int i{scalarLB}; i < loop_ub; i++) {
      rEcef[i] = c[i] * rTeme[i] + tut1[i] * rTeme[i + rTeme.size(0)];
    }
    loop_ub = r.size(0);
    scalarLB = (r.size(0) / 2) << 1;
    vectorUB = scalarLB - 2;
    for (int i{0}; i <= vectorUB; i += 2) {
      r1 = _mm_loadu_pd(&r[i]);
      r2 = _mm_loadu_pd(&c[i]);
      _mm_storeu_pd(
          &rEcef[i + rEcef.size(0)],
          _mm_add_pd(_mm_mul_pd(r1, _mm_loadu_pd(&rTeme[i])),
                     _mm_mul_pd(r2, _mm_loadu_pd(&rTeme[i + rTeme.size(0)]))));
    }
    for (int i{scalarLB}; i < loop_ub; i++) {
      rEcef[i + rEcef.size(0)] =
          r[i] * rTeme[i] + c[i] * rTeme[i + rTeme.size(0)];
    }
    loop_ub = rTeme.size(0);
    for (int i{0}; i < loop_ub; i++) {
      rEcef[i + rEcef.size(0) * 2] = rTeme[i + rTeme.size(0) * 2];
    }
  } else {
    binary_expand_op_1(rEcef, c, rTeme, tut1, r);
  }
  if (c.size(0) == 1) {
    loop_ub = vTeme.size(0);
  } else {
    loop_ub = c.size(0);
  }
  if (tut1.size(0) == 1) {
    scalarLB = vTeme.size(0);
  } else {
    scalarLB = tut1.size(0);
  }
  if (r.size(0) == 1) {
    vectorUB = vTeme.size(0);
  } else {
    vectorUB = r.size(0);
  }
  if ((c.size(0) == vTeme.size(0)) && (tut1.size(0) == vTeme.size(0)) &&
      (loop_ub == scalarLB) && (r.size(0) == vTeme.size(0)) &&
      (c.size(0) == vTeme.size(0)) && (vectorUB == loop_ub)) {
    loop_ub = c.size(0);
    vp.set_size(c.size(0), 3);
    scalarLB = (c.size(0) / 2) << 1;
    vectorUB = scalarLB - 2;
    for (int i{0}; i <= vectorUB; i += 2) {
      r1 = _mm_loadu_pd(&c[i]);
      r2 = _mm_loadu_pd(&tut1[i]);
      _mm_storeu_pd(
          &vp[i],
          _mm_add_pd(_mm_mul_pd(r1, _mm_loadu_pd(&vTeme[i])),
                     _mm_mul_pd(r2, _mm_loadu_pd(&vTeme[i + vTeme.size(0)]))));
    }
    for (int i{scalarLB}; i < loop_ub; i++) {
      vp[i] = c[i] * vTeme[i] + tut1[i] * vTeme[i + vTeme.size(0)];
    }
    loop_ub = r.size(0);
    scalarLB = (r.size(0) / 2) << 1;
    vectorUB = scalarLB - 2;
    for (int i{0}; i <= vectorUB; i += 2) {
      r1 = _mm_loadu_pd(&r[i]);
      r2 = _mm_loadu_pd(&c[i]);
      _mm_storeu_pd(
          &vp[i + vp.size(0)],
          _mm_add_pd(_mm_mul_pd(r1, _mm_loadu_pd(&vTeme[i])),
                     _mm_mul_pd(r2, _mm_loadu_pd(&vTeme[i + vTeme.size(0)]))));
    }
    for (int i{scalarLB}; i < loop_ub; i++) {
      vp[i + vp.size(0)] = r[i] * vTeme[i] + c[i] * vTeme[i + vTeme.size(0)];
    }
    loop_ub = vTeme.size(0);
    for (int i{0}; i < loop_ub; i++) {
      vp[i + vp.size(0) * 2] = vTeme[i + vTeme.size(0) * 2];
    }
  } else {
    binary_expand_op_1(vp, c, vTeme, tut1, r);
  }
  //  Earth rotation rate [rad/s]
  if (vp.size(0) == rEcef.size(0)) {
    loop_ub = vp.size(0);
    vEcef.set_size(vp.size(0), 3);
    scalarLB = (vp.size(0) / 2) << 1;
    vectorUB = scalarLB - 2;
    for (int i{0}; i <= vectorUB; i += 2) {
      __m128d r3;
      r1 = _mm_loadu_pd(&rEcef[i + rEcef.size(0)]);
      r2 = _mm_loadu_pd(&vp[i]);
      r3 = _mm_set1_pd(7.29211514670698E-5);
      _mm_storeu_pd(&vEcef[i], _mm_add_pd(r2, _mm_mul_pd(r3, r1)));
      r1 = _mm_loadu_pd(&rEcef[i]);
      r2 = _mm_loadu_pd(&vp[i + vp.size(0)]);
      _mm_storeu_pd(&vEcef[i + vEcef.size(0)],
                    _mm_sub_pd(r2, _mm_mul_pd(r3, r1)));
      r1 = _mm_loadu_pd(&vp[i + vp.size(0) * 2]);
      _mm_storeu_pd(&vEcef[i + vEcef.size(0) * 2], r1);
    }
    for (int i{scalarLB}; i < loop_ub; i++) {
      vEcef[i] = vp[i] + 7.29211514670698E-5 * rEcef[i + rEcef.size(0)];
      vEcef[i + vEcef.size(0)] =
          vp[i + vp.size(0)] - 7.29211514670698E-5 * rEcef[i];
      vEcef[i + vEcef.size(0) * 2] = vp[i + vp.size(0) * 2];
    }
  } else {
    binary_expand_op(vEcef, vp, rEcef);
  }
}

//
// TLETOSATREC  Build an SGP4 record from the two 69-character TLE lines.
//    Lines are assumed already validated (see TLEData.parse).
//
// Arguments    : const char line1[69]
//                const char line2[69]
//                struct0_T *satrec
// Return Type  : void
//
void AdcsCore::tleToSatrec(const char line1[69], const char line2[69],
                           struct0_T *satrec)
{
  creal_T b_x;
  creal_T c_x;
  creal_T d_x;
  creal_T x;
  int sgn;
  char cv[9];
  sgn = 1;
  if (line1[53] == '-') {
    sgn = -1;
  }
  //  str2double is complex-valued under codegen; TLE fields are always real.
  x = coder::internal::str2double(&line1[54]);
  //  str2double is complex-valued under codegen; TLE fields are always real.
  b_x = coder::internal::b_str2double(&line1[59]);
  //  str2double is complex-valued under codegen; TLE fields are always real.
  c_x = coder::internal::c_str2double(&line2[8]);
  //  str2double is complex-valued under codegen; TLE fields are always real.
  d_x = coder::internal::c_str2double(&line2[17]);
  //  str2double is complex-valued under codegen; TLE fields are always real.
  cv[0] = '0';
  cv[1] = '.';
  for (int i{0}; i < 7; i++) {
    cv[i + 2] = line2[i + 26];
  }
  creal_T e_x;
  creal_T f_x;
  creal_T g_x;
  creal_T h_x;
  e_x = coder::internal::d_str2double(cv);
  //  str2double is complex-valued under codegen; TLE fields are always real.
  f_x = coder::internal::c_str2double(&line2[34]);
  //  str2double is complex-valued under codegen; TLE fields are always real.
  g_x = coder::internal::c_str2double(&line2[43]);
  //  str2double is complex-valued under codegen; TLE fields are always real.
  h_x = coder::internal::e_str2double(&line2[52]);
  //  rev/day -> rad/min
  sgp4init(e_x.re, c_x.re * 0.017453292519943295, d_x.re * 0.017453292519943295,
           f_x.re * 0.017453292519943295, g_x.re * 0.017453292519943295,
           h_x.re * 2.0 * 3.1415926535897931 / 1440.0,
           static_cast<double>(sgn) * x.re * 1.0E-5 * rt_powd_snf(10.0, b_x.re),
           satrec);
}

} // namespace adcs

//
// File trailer for AdcsCore.cpp
//
// [EOF]
//
