/* ********************************************************************** */
/* itv.h: (unidimensional) intervals */
/* ********************************************************************** */

#ifndef _ITV_H_
#define _ITV_H_

#include <stdio.h>
#include "num.h"
#include "bound.h"
#include "itv_config.h"
#include "ap_global0.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Be cautious: interval [a,b] is represented by [-a,b].  This is because
   bound quantities are always rounded toward +infty */

struct itv_t {
  bound_t inf; /* negation of the inf bound */
  bound_t sup; /* sup bound */
};
typedef struct itv_t itv_t[1];
typedef struct itv_t* itv_ptr;


/* Workspace to avoid temporary allocation and deallocation when num_t and
   bound_t are multiprecision numbers */
typedef struct itv_internal_t {
  num_t canonicalize_num;
  bound_t muldiv_bound;
  bound_t mul_bound;
  itv_t mul_itv;
  itv_t mul_itv2;
  ap_scalar_t* set_interval_scalar;
  itv_t eval_itv;
  itv_t eval_itv2;
} itv_internal_t;


itv_internal_t* itv_internal_alloc();
  /* Allocate and initialize internal workspace */
void itv_internal_free(itv_internal_t* intern);
  /* Clear and free internal workspace */

void itv_internal_init(itv_internal_t* intern);
void itv_internal_clear(itv_internal_t* intern);
  /* GMP Semantics */

/* ********************************************************************** */
/* itv */
/* ********************************************************************** */

/* Initialization and clearing */
static inline void itv_init(itv_t a);
static inline void itv_init_array(itv_t* a, size_t size);
static inline void itv_init_set(itv_t a, const itv_t b);
static inline void itv_clear(itv_t a);
static inline void itv_clear_array(itv_t* a, size_t size);
static inline void itv_array_free(itv_t* a, size_t size);

/* Assignement */
static inline void itv_set(itv_t a, const itv_t b);
static inline void itv_set_bottom(itv_t a);
static inline void itv_set_top(itv_t a);
static inline void itv_swap(itv_t a, itv_t b);

/* Normalization and tests */
bool itv_canonicalize(itv_internal_t* intern,
		      itv_t a, bool integer);
  /* Canonicalize an interval:
     - if integer is true, narrows bound to integers
     - return true if the interval is bottom
     - return false otherwise
  */
static inline bool itv_is_top(const itv_t a);
static inline bool itv_is_bottom(itv_internal_t* intern, itv_t a);
  /* Return true iff the interval is resp. [-oo,+oo] or empty */
static inline bool itv_is_leq(const itv_t a, const itv_t b);
  /* Inclusion test */
static inline bool itv_is_eq(const itv_t a, const itv_t b);
  /* Equality test */

/* Lattice operations */
static inline bool itv_meet(itv_internal_t* intern,
			    itv_t a, const itv_t b, const itv_t c);
  /* Assign a with the intersection of b and c */
static inline void itv_join(itv_t a, const itv_t b, const itv_t c);
  /* Assign a with the union of b and c */
static inline void itv_widening(itv_t a, const itv_t b, const itv_t c);
  /* Assign a with the standard interval widening of b by c */

/* Arithmetic operations */
static inline 
void itv_add(itv_t a, const itv_t b, const itv_t c);
void itv_sub(itv_t a, const itv_t b, const itv_t c);
void itv_neg(itv_t a, const itv_t b);
void itv_mul(itv_internal_t* intern,
	     itv_t a, const itv_t b, const itv_t c);
static inline
void itv_add_num(itv_t a, const itv_t b, const num_t c);
void itv_mul_bound(itv_internal_t* intern,
		   itv_t a, const itv_t b, const bound_t c);
void itv_div_bound(itv_internal_t* intern,
		   itv_t a, const itv_t b, const bound_t c);
  

/* Printing */
int itv_snprint(char* s, size_t size, const itv_t a);
void itv_fprint(FILE* stream, const itv_t a);

/* Conversions (not fully implemented) */
/* round indicates if rounding mode is ceil (+1) or floor (-1) */

int num_set_ap_scalar(num_t a, const ap_scalar_t* b, int round);
  /* Convert a non infinity ap_scalar_t into a num_t */
int bound_set_ap_scalar(num_t a, const ap_scalar_t* b, int round);
  /* Convert a ap_scalar_t into a bound_t */
int ap_scalar_set_num(ap_scalar_t* a, const num_t b, int round);
  /* Convert a num_t into a ap_scalar_t */
int ap_scalar_set_bound(ap_scalar_t* a, const bound_t b, int round);
  /* Convert a bound_t into a ap_scalar_t */

void itv_set_ap_interval(itv_internal_t* intern,
			 itv_t a, const ap_interval_t* b);
  /* Convert a ap_interval_t into a itv_t */
void ap_interval_set_itv(ap_interval_t* a, const itv_t b);
  /* Convert a itv_t into a ap_interval_t */

bool itv_set_ap_coeff(itv_internal_t* intern,
		      itv_t a, const ap_coeff_t* b);
  /* Convert a ap_coeff_t into a itv_t.

     Return true if the returned interval is a point */

/* ********************************************************************** */
/* Definition of inline functions */
/* ********************************************************************** */

static inline void itv_init(itv_t a)
{
  bound_init(a->inf);
  bound_init(a->sup);
}
static inline void itv_init_array(itv_t* a, size_t size)
{
  int i;
  for (i=0; i<size; i++) itv_init(a[i]);
}    
static inline void itv_init_set(itv_t a, const itv_t b)
{
  bound_init_set(a->inf,b->inf);
  bound_init_set(a->sup,b->sup);
}
static inline void itv_clear(itv_t a)
{
  bound_clear(a->inf);
  bound_clear(a->sup);
}
static inline void itv_clear_array(itv_t* a, size_t size)
{
#if !defined(NUM_NATIVE)
  int i;
  for (i=0; i<size; i++) itv_clear(a[i]);
#endif
}    
static inline void itv_array_free(itv_t* a, size_t size)
{
  itv_clear_array(a,size);
  free(a);
}

static inline void itv_set(itv_t a, const itv_t b)
{
  bound_set(a->inf,b->inf);
  bound_set(a->sup,b->sup);
}
static inline void itv_set_bottom(itv_t a)
{
  bound_set_int(a->inf,-1);
  bound_set_int(a->sup,-1);
}
static inline void itv_set_top(itv_t a)
{
  bound_set_infty(a->inf);
  bound_set_infty(a->sup);
}
static inline void itv_swap(itv_t a, itv_t b)
{ itv_t t; *t=*a;*a=*b;*b=*t; }

static inline bool itv_is_top(const itv_t a)
{ 
  return bound_infty(a->inf) && bound_infty(a->sup); 
}
static inline bool itv_is_bottom(itv_internal_t* intern, itv_t a)
{
  return itv_canonicalize(intern, a, false);
}
static inline bool itv_is_leq(const itv_t a, const itv_t b)
{
  return bound_cmp(a->sup,b->sup)<=0 && bound_cmp(a->inf,b->inf)<=0;
}
static inline bool itv_is_eq(const itv_t a, const itv_t b)
{
  return bound_equal(a->sup,b->sup) && bound_equal(a->inf,b->inf);
}

static inline bool itv_meet(itv_internal_t* intern,
			    itv_t a, const itv_t b, const itv_t c)
{
  bound_min(a->sup,b->sup,c->sup);
  bound_min(a->inf,b->inf,c->inf);
  return itv_canonicalize(intern,a,false);
}
static inline void itv_join(itv_t a, const itv_t b, const itv_t c)
{
  bound_max(a->sup,b->sup,c->sup);
  bound_max(a->inf,b->inf,c->inf);
}
static inline void bound_widening(bound_t a, const bound_t b, const bound_t c)
{
  if (bound_infty(c) ||
      bound_cmp(b,c)<0){
    bound_set_infty(a);
  } else {
    bound_set(a,b);
  }
}
static inline void itv_widening(itv_t a, 
				const itv_t b, const itv_t c)
{
  bound_widening(a->sup,b->sup,c->sup);
  bound_widening(a->inf,b->inf,c->inf);
}
static inline void itv_add(itv_t a, const itv_t b, const itv_t c)
{
  bound_add(a->sup,b->sup,c->sup);
  bound_add(a->inf,b->inf,c->inf);
}
static inline void itv_add_num(itv_t a, const itv_t b, const num_t c)
{
  bound_add_num(a->sup,b->sup,c);
  bound_sub_num(a->inf,b->inf,c);
}

#ifdef __cplusplus
}
#endif

#endif
