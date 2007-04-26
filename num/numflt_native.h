/* ********************************************************************** */
/* numflt_native.h */
/* ********************************************************************** */

#ifndef _NUMFLT_NATIVE_H_
#define _NUMFLT_NATIVE_H_

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include "gmp.h"
#include "mpfr.h"
#include "ap_scalar.h"
#include "num_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(NUMFLT_DOUBLE)
typedef double numflt_native;
#define NUMFLT_ZERO 0.0
#define NUMFLT_ONE 1.0
#define NUMFLT_MANT_DIG DBL_MANT_DIG

#elif defined(NUMFLT_LONGDOUBLE)
typedef long double numflt_native;
#define NUMFLT_ZERO 0.0L
#define NUMFLT_ONE 1.0L
#define NUMFLT_MANT_DIG LDBL_MANT_DIG

#else
#error "HERE"
#endif

typedef numflt_native numflt_t[1];

#define NUMFLT_MAX NUMFLT_ONE/NUMFLT_ZERO

#define NUMFLT_NATIVE

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */
static inline void numflt_set(numflt_t a, numflt_t b)
{ *a = *b; }
static inline void numflt_set_array(numflt_t* a, numflt_t* b, size_t size)
{ memcpy(a,b,size*sizeof(numflt_t)); }
static inline void numflt_set_int(numflt_t a, long int i)
{ *a = (numflt_native)i; }
 
/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void numflt_init(numflt_t a)
{ *a = NUMFLT_ZERO; }
static inline void numflt_init_array(numflt_t* a, size_t size)
{
  size_t i; 
  for (i=0; i<size; i++) *(a[i]) = NUMFLT_ZERO; 
}
static inline void numflt_init_set(numflt_t a, numflt_t b)
{ numflt_set(a,b); }
static inline void numflt_init_set_int(numflt_t a, long int i)
{ numflt_set_int(a,i); }

static inline void numflt_clear(numflt_t a)
{}
static inline void numflt_clear_array(numflt_t* a, size_t size)
{}

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void numflt_neg(numflt_t a, numflt_t b)
{ *a = -(*b); }
static inline void numflt_add(numflt_t a, numflt_t b, numflt_t c)
{ *a = *b + *c; }
static inline void numflt_add_uint(numflt_t a, numflt_t b, unsigned long int c)
{ *a = *b + (numflt_native)c; }
static inline void numflt_sub(numflt_t a, numflt_t b, numflt_t c)
{ *a = *b - *c; }
static inline void numflt_sub_uint(numflt_t a, numflt_t b, unsigned long int c)
{ *a = *b - (numflt_native)c; }
static inline void numflt_mul(numflt_t a, numflt_t b, numflt_t c)
{ *a = *b * *c; }
static inline void numflt_div(numflt_t a, numflt_t b, numflt_t c)
{ *a = *b / *c; }

#if defined(NUMFLT_DOUBLE)
static inline void numflt_abs(numflt_t a, numflt_t b)
{ *a = fabs(*b); }
static inline void numflt_mul_2(numflt_t a, numflt_t b)
{ *a = ldexp(*b,1); }
static inline void numflt_div_2(numflt_t a, numflt_t b)
{ *a = ldexp(*b,-1); }
static inline void numflt_min(numflt_t a, numflt_t b, numflt_t c)
{ *a = fmin(*b,*c); }
static inline void numflt_max(numflt_t a, numflt_t b, numflt_t c)
{ *a = fmax(*b,*c); }
static inline void numflt_floor(numflt_t a, numflt_t b)
{ *a = floor(*b); }
static inline void numflt_ceil(numflt_t a, numflt_t b)
{ *a = ceil(*b); }
#else
static inline void numflt_abs(numflt_t a, numflt_t b)
{ *a = fabsl(*b); }
static inline void numflt_mul_2(numflt_t a, numflt_t b)
{ *a = ldexpl(*b,1); }
static inline void numflt_div_2(numflt_t a, numflt_t b)
{ *a = ldexpl(*b,-1); }
static inline void numflt_min(numflt_t a, numflt_t b, numflt_t c)
{ *a = fminl(*b,*c); }
static inline void numflt_max(numflt_t a, numflt_t b, numflt_t c)
{ *a = fmaxl(*b,*c); }
static inline void numflt_floor(numflt_t a, numflt_t b)
{ *a = floorl(*b); }
static inline void numflt_ceil(numflt_t a, numflt_t b)
{ *a = ceill(*b); }
#endif
/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int numflt_sgn(numflt_t a)
{ return (*a==NUMFLT_ZERO ? 0 : (*a>NUMFLT_ZERO ? 1 : -1)); }
static inline int numflt_cmp(numflt_t a, numflt_t b)
{ return (*a==*b ? 0 : (*a>*b ? 1 : -1)); }
static inline int numflt_cmp_int(numflt_t a, long int b)
{ 
  numflt_native bb = (numflt_native)b;
  return (*a==bb ? 0 : (*a>bb ? 1 : -1)); 
}
static inline bool numflt_equal(numflt_t a, numflt_t b)
{ return *a==*b; }

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

#if defined(NUMFLT_DOUBLE)
static inline void numflt_print(numflt_t a)
{ printf("%.20g",*a+NUMFLT_ZERO); }
static inline void numflt_fprint(FILE* stream, numflt_t a)
{ fprintf(stream,"%.20g",*a+NUMFLT_ZERO); }
static inline int numflt_snprint(char* s, size_t size, numflt_t a)
{ return snprintf(s,size,"%.20g",*a+NUMFLT_ZERO); }
#else
static inline void numflt_print(numflt_t a)
{ printf("%.20Lg",*a+NUMFLT_ZERO); }
static inline void numflt_fprint(FILE* stream, numflt_t a)
{ fprintf(stream,"%.20Lg",*a+NUMFLT_ZERO); }
static inline int numflt_snprint(char* s, size_t size, numflt_t a)
{ return snprintf(s,size,"%.20Lg",*a+NUMFLT_ZERO); }
#endif

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

/* int2 -> numflt */
static inline void numflt_set_int2(numflt_t a, long int i, unsigned long int j)
{ *a = (numflt_native)i/(-((numflt_native)(-j))); }

/* mpz -> numflt */
/* mpfr is supposed to have exactly the IEEE754 double precision of NUMFLT_MANT_DIG bits */
static inline bool numflt_set_mpz_tmp(numflt_t a, mpz_t b, mpfr_t mpfr)
{ 
  int res = mpfr_set_z(mpfr,b,GMP_RNDU);
#if defined(NUMFLT_DOUBLE)
  *a = mpfr_get_d(mpfr,GMP_RNDU);/* Normally, exact conversion here (unless overfloww) */
#else
  *a = mpfr_get_ld(mpfr,GMP_RNDU);/* Normally, exact conversion here (unless overfloww) */
#endif
  return (res==0);
}
static inline bool numflt_set_mpz(numflt_t a, mpz_t b)
{
  mpfr_t mpfr;
  
  mpfr_init2(mpfr,NUMFLT_MANT_DIG);
  bool res = numflt_set_mpz_tmp(a,b,mpfr);
  mpfr_clear(mpfr);
  return res;
}
/* mpq -> numflt */
/* mpfr is supposed to have exactly the IEEE754 double precision of NUMFLT_MANT_DIG bits */
static inline bool numflt_set_mpq_tmp(numflt_t a, mpq_t b, mpfr_t mpfr)
{
  int res = mpfr_set_q(mpfr,b,GMP_RNDU);
#if defined(NUMFLT_DOUBLE)
  *a = mpfr_get_d(mpfr,GMP_RNDU);/* Normally, exact conversion here (unless overfloww) */
#else
  *a = mpfr_get_ld(mpfr,GMP_RNDU);/* Normally, exact conversion here (unless overfloww) */
#endif
  return (res==0);
}  
static inline bool numflt_set_mpq(numflt_t a, mpq_t b)
{
  mpfr_t mpfr;
  
  mpfr_init2(mpfr,NUMFLT_MANT_DIG);
  bool res = numflt_set_mpq_tmp(a,b,mpfr);
  mpfr_clear(mpfr);
  return res;
}
/* double -> numflt */
static inline bool numflt_set_double(numflt_t a, double k)
{ *a = (numflt_native)k; return true; }

/* numflt -> int */
static inline bool int_set_numflt(long int* a, numflt_t b)
{
  numflt_native c;
  numflt_ceil(&c,b);
  *a = (long int)c;
  return (*b==c);
}
/* numflt -> mpz */
static inline bool mpz_set_numflt(mpz_t a, numflt_t b)
{ 
  double c = ceil(*b);
  mpz_set_d(a,c);
  return (*b==(numflt_native)c);
}
/* numflt -> mpq */
static inline bool mpq_set_numflt(mpq_t a, numflt_t b)
#if defined(NUMFLT_DOUBLE)
{ mpq_set_d(a,*b); return true; }
#else
{
  double c = (double)(*b);
  mpq_set_d(a,c);
  return (*b==(numflt_native)c);
}
#endif
/* numflt -> double */
static inline bool double_set_numflt(double* a, numflt_t b)
#if defined(NUMFLT_DOUBLE)
{ *a = *b; return true; }
#else
{ *a = *b; return ((numflt_native)(*a)==*b); }
#endif


static inline bool mpz_fits_numflt(mpz_t a)
{
  double k = mpz_get_d(a);
  return (fabs(k)+1.0) != (double)1.0/(double)0.0;
}
static inline bool mpq_fits_numflt(mpq_t a)
{
  double k = mpq_get_d(a);
  return (fabs(k)+1.0) != (double)1.0/(double)0.0;
}
static inline bool double_fits_numflt(double a)
{ return true; }
static inline bool numflt_fits_int(numflt_t a)
{ 
  numflt_native d;
  numflt_ceil(&d,a);
  return d >= (numflt_native)(-LONG_MAX) && d<= (numflt_native)LONG_MAX;
}
static inline bool numflt_fits_double(numflt_t a)
#if defined(NUMFLT_DOUBLE)
{ return true; }
#else
{ 
  double k = (double)*a;
  return fabs(k) != (double)1.0/(double)0.0;
}
#endif

/* ====================================================================== */
/* Only for floating point */
/* ====================================================================== */

static inline bool numflt_infty(numflt_t a)
#if defined(NUMFLT_DOUBLE)
{ return fabs(*a) == NUMFLT_MAX; }
#else
{ return fabsl(*a) == NUMFLT_MAX; }
#endif
static inline void numflt_set_infty(numflt_t a)
{ *a = NUMFLT_MAX; }

/* ====================================================================== */
/* Serialization */
/* ====================================================================== */

static inline size_t numflt_serialize(void* dst, numflt_t src)
{
  num_store_words8(dst,src,sizeof(numflt_t));
  return sizeof(numflt_t);
}

static inline size_t numflt_deserialize(numflt_t dst, const void* src)
{
  num_store_words8(dst,src,sizeof(numflt_t));
  return sizeof(numflt_t);
}

static inline size_t numflt_serialized_size(numflt_t a)
{ return sizeof(numflt_t); }

#ifdef __cplusplus
}
#endif

#endif
