#pragma once
#include "complex.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
     \brief Subroutine BHMIE is the Bohren-Huffman Mie scattering subroutine
     to calculate scattering and absorption by a homogenous isotropic
     sphere.

  Given:
     X = 2*pi*a/lambda
     REFREL = (complex refr. index of sphere)/(real index of medium)
     NANG = number of angles between 0 and 90 degrees
            (will calculate 2*NANG-1 directions from 0 to 180 deg.)
  Returns:
     S1(1 .. 2*NANG-1) =  (incid. E perp. to scatt. plane,
                                 scatt. E perp. to scatt. plane)
     S2(1 .. 2*NANG-1) =  (incid. E parr. to scatt. plane,
                                 scatt. E parr. to scatt. plane)
     QEXT = C_ext/pi*a**2 = efficiency factor for extinction
     QSCA = C_sca/pi*a**2 = efficiency factor for scattering
     QBACK = 4*pi*(dC_sca/domega)/pi*a**2
           = backscattering efficiency
     GSCA = <cos(theta)> for scattering
 
  Original program taken from Bohren and Huffman (1983), Appendix A
  Modified by B.T.Draine, Princeton Univ. Obs., 90/10/26
  in order to compute <cos(theta)>
  This code was translatted to C by P. J. Flatau Feb 1998. The C
  version uses "Numerical Recipes" public domain code for complex
  arithmetics "complex.c" and "nrutil.c" (http://www.nr.com).

**/
void  bhmie(float x,fcomplex cxref, unsigned long nang,fcomplex cxs1[],
 fcomplex cxs2[],float  *qext, float *qsca, float *qback, float *gsca);

#ifdef __cplusplus
}
#endif
