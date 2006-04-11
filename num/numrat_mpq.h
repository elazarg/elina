/* ********************************************************************** */
/* rational_mpq.h */
/* ********************************************************************** */

#ifndef _NUMRAT_MPQ_H_
#define _NUMRAT_MPQ_H_

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <gmp.h>

#include "num_config.h"

#ifdef NUMINT_MPZ
#include "numint.h"
#else
#error "HERE"
#endif

typedef mpq_t numrat_t;


/* ====================================================================== */
/* Rational operations */
/* ====================================================================== */

static inline void numrat_canonicalize(numrat_t a)
{ mpq_canonicalize(a); }

#define numrat_numref(a) mpq_numref(a)
#define numrat_denref(a) mpq_denref(a)

static inline void numrat_set_numint2(numrat_t a, const numint_t b, const numint_t c)
{
  numint_set(numrat_numref(a),b);
  numint_set(numrat_denref(a),c);
  numrat_canonicalize(a);
}

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */
static inline void numrat_set(numrat_t a, const numrat_t b)
{ mpq_set(a,b); }
static inline void numrat_set_array(numrat_t* a, const numrat_t* b, size_t size)
{
  int i;
  for (i=0; i<size; i++) mpq_set(a[i],b[i]);
}
static inline void numrat_set_int(numrat_t a, long int i)
{ mpq_set_si(a,i,1); }

/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void numrat_init(numrat_t a)
{ mpq_init(a); }
static inline void numrat_init_array(numrat_t* a, size_t size)
{
  int i;
  for (i=0; i<size; i++) mpq_init(a[i]);
}
static inline void numrat_init_set(numrat_t a, const numrat_t b)
{ mpq_init(a); mpq_set(a,b); }
static inline void numrat_init_set_int(numrat_t a, long int i)
{ mpq_init(a); mpq_set_si(a,i,1); }

static inline void numrat_clear(numrat_t a)
{ mpq_clear(a); }
static inline void numrat_clear_array(numrat_t* a, size_t size)
{
  int i;
  for (i=0; i<size; i++) mpq_clear(a[i]);
}

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void numrat_neg(numrat_t a, const numrat_t b)
{ mpq_neg(a,b); }
static inline void numrat_abs(numrat_t a, const numrat_t b)
{ mpq_abs(a,b); }
static inline void numrat_add(numrat_t a, const numrat_t b, const numrat_t c)
{ mpq_add(a,b,c); }
static inline void numrat_add_uint(numrat_t a, const numrat_t b, unsigned long int c)
{
  numint_add_uint(numrat_numref(a),numrat_numref(b),c);
  numint_set(numrat_denref(a),numrat_denref(b));
  numrat_canonicalize(a);
}
static inline void numrat_sub(numrat_t a, const numrat_t b, const numrat_t c)
{ mpq_sub(a,b,c); }
static inline void numrat_sub_uint(numrat_t a, const numrat_t b, unsigned long int c)
{
  numint_sub_uint(numrat_numref(a),numrat_numref(b),c);
  numint_set(numrat_denref(a),numrat_denref(b));
  numrat_canonicalize(a);
}
static inline void numrat_mul(numrat_t a, const numrat_t b, const numrat_t c)
{ mpq_mul(a,b,c); }
static inline void numrat_mul_2(numrat_t a, const numrat_t b)
{ mpq_mul_2exp(a,b,1); }
static inline void numrat_div(numrat_t a, const numrat_t b, const numrat_t c)
{ mpq_div(a,b,c); }
static inline void numrat_div_2(numrat_t a, const numrat_t b)
{ mpq_div_2exp(a,b,1); }
static inline void numrat_min(numrat_t a, const numrat_t b, const numrat_t c)
{ mpq_set(a, mpq_cmp(b,c)<=0 ? b : c); }
static inline void numrat_max(numrat_t a, const numrat_t b, const numrat_t c)
{ mpq_set(a, mpq_cmp(b,c)>=0 ? b : c); }
static inline void numrat_floor(numrat_t a, const numrat_t b)
{
  numint_fdiv_q(numrat_numref(a),numrat_numref(b),numrat_denref(b));
  numint_set_int(numrat_denref(a),1);
}
static inline void numrat_ceil(numrat_t a, const numrat_t b)
{
  numint_cdiv_q(numrat_numref(a),numrat_numref(b),numrat_denref(b));
  numint_set_int(numrat_denref(a),1);
}

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int numrat_sgn(const numrat_t a)
{ return mpq_sgn(a); }
static inline int numrat_cmp(const numrat_t a, const numrat_t b)
{ return mpq_cmp(a,b); }
static inline int numrat_cmp_int(const numrat_t a, long int b)
{ return mpq_cmp_si(a,b,1); }
static inline int numrat_equal(const numrat_t a, const numrat_t b)
{ return mpq_equal(a,b); }

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void numrat_print(const numrat_t a)
{ mpq_out_str(stdout,10,a); }
static inline void numrat_fprint(FILE* stream, const numrat_t a)
{ mpq_out_str(stream,10,a); }
static inline int numrat_snprint(char* s, size_t size, const numrat_t a)
{
  int res;
  if (mpz_sizeinbase(mpq_numref(a),10) +
      mpz_sizeinbase(mpq_denref(a),10) +
      3 > size )
    res = snprintf(s,size, mpq_sgn(a)>0 ? "+BIG" : "-BIG");
  else {
    mpq_get_str(s,10,a);
    res = strlen(s);
  }
  return res;
}

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

/* int2 -> numrat */
static inline void numrat_set_int2(numrat_t a, long int i, unsigned long int j)
{ mpq_set_si(a,i,j); }

/* mpz -> numrat */
static inline bool mpz_fits_numrat(const mpz_t a)
{ return true; }
static inline void numrat_set_mpz(numrat_t a, const mpz_t b)
{ 
  mpz_set(mpq_numref(a),b); 
  mpz_set_ui(mpq_denref(a),1);
}

/* mpq -> numrat */
static inline bool mpq_fits_numrat(const mpq_t a)
{ return true; }
static inline void numrat_set_mpq(numrat_t a, const mpq_t b)
{ mpq_set(a,b); }

/* double -> numrat */
static inline bool double_fits_numrat(double k)
{ return true; }
static inline void numrat_set_double(numrat_t a, double k)
{ mpq_set_d(a,k); }

/* numrat -> int */
static inline bool numrat_fits_int(const numrat_t a)
{
  double d = ceil(mpq_get_d(a));
  return d<=LONG_MAX && d>=-LONG_MAX;
}
static inline long int numrat_get_int(const numrat_t a)
{ return (long int)ceil(mpq_get_d(a)); } /* Bad... */

/* numrat -> mpz */
static inline void mpz_set_numrat(mpz_t a, const numrat_t b)
{
  numint_t q;
  numint_init(q);
  numint_cdiv_q(q,numrat_numref(b),numrat_denref(b));
  mpz_set_numint(a,q);
}
/* numrat -> mpq */
static inline void mpq_set_numrat(mpq_t a, const numrat_t b)
{
  mpz_set_numint(mpq_numref(a), numrat_numref(b));
  mpz_set_numint(mpq_denref(a), numrat_denref(b));
}
/* numrat -> double */
static inline bool numrat_fits_double(const numrat_t a)
{ 
  double k = mpq_get_d(a);
  return fabs(k) != (double)1.0/(double)0.0;
}
static inline double numrat_get_double(const numrat_t a)
{
  return mpq_get_d(a);
}

#endif
