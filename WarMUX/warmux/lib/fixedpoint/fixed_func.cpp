/*
Copyright (c) 2010, Warmux Team

Large parts of the code are from the fixed point library of Markus Trenkwalder
Copyright (c) 2007, Markus Trenkwalder

Portions taken from the Vicent 3D rendering library
Copyright (c) 2004, David Blythe, Hans Martin Will

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the library's copyright owner nor the names of its
  contributors may be used to endorse or promote products derived from this
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include "fixed_func.h"

namespace fp {

static const int32_t FIX16_2PI     = float2fix<16>(2*M_PI);
static const int32_t FIX16_HALF_PI = float2fix<16>(0.5*M_PI);
static const int32_t FIX16_R2PI    = float2fix<16>(1/(2*M_PI));

static const uint16_t sin_tab[] = {
#include "fixsintab.h"
};

static const int32_t asin_tab[] = {
#include "fixasintab.h"
};

static const uint16_t atan_tab[] = {
#include "fixatantab.h"
};


fint_t fixcos16(fint_t a)
{
  fint_t v;
  /* reduce to [0,1) */
  while (a < 0) a += FIX16_2PI;
  a = fixmul<16>(a, FIX16_R2PI);
  a += 0x4000;

  /* now in the range [0, 0xffff], reduce to [0, 0xfff] */
  a >>= 4;

  v = (a & 0x400) ? sin_tab[0x3ff - (a & 0x3ff)] : sin_tab[a & 0x3ff];
  v = fixmul<16>(v, 1 << 16);
  return (a & 0x800) ? -v : v;
}

fint_t fixsin16(fint_t a)
{
  fint_t v;

  /* reduce to [0,1) */
  while (a < 0) a += FIX16_2PI;
  a = fixmul<16>(a, FIX16_R2PI);

  /* now in the range [0, 0xffff], reduce to [0, 0xfff] */
  a >>= 4;

  v = (a & 0x400) ? sin_tab[0x3ff - (a & 0x3ff)] : sin_tab[a & 0x3ff];
  v = fixmul<16>(v, 1 << 16);
  return (a & 0x800) ? -v : v;
}

fint_t fixacos16(fint_t a)
{
  return FIX16_HALF_PI - fixasin16(a);
}

fint_t fixasin16(fint_t a)
{
  if (a > 0) {
    if (a >= 0x10000)
      a = 0x10000;
    return asin_tab[a];;
  } else {
    if (a <= -0x10000)
      a = -0x10000;
    return -asin_tab[-a];
  }
}

fint_t fixatan16(fint_t a)
{
  if (a > 0) {
    if (a <= 0x10000) {
      return atan_tab[a];
    } else {
      return FIX16_HALF_PI - atan_tab[fixinv<16>(a)];;
    }
  } else {
    if (a >= -0x10000) {
      return - atan_tab[-a];
    } else {
      return -FIX16_HALF_PI + atan_tab[-fixinv<16>(a)];
    }
  }
}

fint_t fixrsqrt16(fint_t a)
{
  fint_t x;

  static const uint16_t rsq_tab[] = { /* domain 0.5 .. 1.0-1/16 */
    0xb504, 0xaaaa, 0xa1e8, 0x9a5f, 0x93cd, 0x8e00, 0x88d6, 0x8432,
  };

  fint_t i, exp;
  if (a == 0) return 0x7fffffff;
  if (a == (1<<16)) return a;

  exp = detail::CountLeadingZeros(a);
  x = rsq_tab[(a>>(28-exp))&0x7]<<1;

  exp -= 16;
  if (exp <= 0)
    x >>= -exp>>1;
  else
    x <<= (exp>>1)+(exp&1);
  if (exp&1) x = fixmul<16>(x, rsq_tab[0]);


  /* newton-raphson */
  /* x = x/2*(3-(a*x)*x) */
   i = 0;
  do {
    x = fixmul<16>((x>>1),((1<<16)*3 - fixmul<16>(fixmul<16>(a,x),x)));
  } while(++i < 3);

  return x;
}

static inline fint_t fast_div16(fint_t a, fint_t b)
{
  if ((b >> 24) && (b >> 24) + 1) {
    return fixmul<16>(a >> 8, fixinv<16>(b >> 8));
  } else {
    return fixmul<16>(a, fixinv<16>(b));
  }
}

fint_t fixsqrt16(fint_t a)
{
  if (a < 1<<7) {
    return 0;
  }

#if 0
  static fint_t max = 0;
  if (max < a) {
    printf("max: %"PRIi64"\n", a);
    max = a;
  }
#endif

  fint_t s = (a + (1<<16)) >> 1;
  /* 14 iterations to find exact value for max 948015267840 */
  for (int i = 0; i < 14; i++) {
    s = (s + (a<<16) / s) >> 1;
  }
  return s;
}

fint_t fixsqrt16_approx(fint_t a)
{
  if (a < 1<<7) {
    return 0;
  }

#if 0
  static fint_t max = 0;
  if (max < a) {
    printf("max: %"PRIi64"\n", a);
    max = a;
  }
#endif

  fint_t s = (a + (1<<16)) >> 1;
  /* 6 iterations to find exact value for max 34668544 */
  for (int i = 0; i < 6; i++) {
    s = (s + (a<<16) / s) >> 1;
  }
  return s;
}

} // end namespace fp
