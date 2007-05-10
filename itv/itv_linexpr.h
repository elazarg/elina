/* ********************************************************************** */
/* itv_linexpr.c: */
/* ********************************************************************** */

#ifndef _ITV_LINEXPR_H_
#define _ITV_LINEXPR_H_

#include "itv.h"
#include "ap_expr0.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Interval linear term */
typedef struct itv_linterm_t {
  itv_t itv;
  bool equality;
    /* indicate that the interval is reduced to a single value */
  ap_dim_t dim;
} itv_linterm_t;

/* Interval linear expression */
typedef struct itv_linexpr_t {
  itv_linterm_t* linterm;
  size_t size;
  itv_t cst;
  bool equality;
    /* indicate that the interval is reduced to a single value */
} itv_linexpr_t;

/* Interval linear constraint */
typedef struct itv_lincons_t {
  itv_linexpr_t linexpr;
  ap_constyp_t constyp;
  num_t num;
} itv_lincons_t;

/* Iterator (Macro): use:
   itv_linexpr_ForeachLinterm(itv_linexpr_t* e, size_t i, ap_dim_t d, itv_ptr pitv, bool* pequality){
     ..
   }
   where
   - e is the inspected expression,
   - i is the internal iterator (of type size_t or int)
   - dim is the dimension of one linear term
   - pitv is a pointer to the corresponding coefficient
   - pequality indicates if the interval is actually a point

*/
#define itv_linexpr_ForeachLinterm(_p_e, _p_i, _p_d, _p_itv, _p_equality) \
  for ((_p_i)=0;							\
       (_p_i)<(_p_e)->size ?						\
	 (((_p_d) = (_p_e)->linterm[i].dim),				\
	  ((_p_itv) = (_p_e)->linterm[i].itv),				\
	  ((_p_equality) = &((_p_e)->linterm[i].equality)),		\
	  ((_p_d)!=AP_DIM_MAX)) :					\
	 false;								\
       (_p_i)++)

/* ********************************************************************** */
/* I. Constructor and Destructor */
/* ********************************************************************** */

static inline void itv_linexpr_init(itv_linexpr_t* expr, size_t size);
static inline void itv_linexpr_reinit(itv_linexpr_t* expr, size_t size);
static inline void itv_linexpr_clear(itv_linexpr_t* expr);

static inline void itv_lincons_init(itv_lincons_t* cons);
static inline void itv_lincons_clear(itv_lincons_t* cons);

/* ********************************************************************** */
/* II. Conversions from and to APRON datatypes */
/* ********************************************************************** */

static inline bool itv_linexpr_set_ap_linexpr0(itv_internal_t* intern,
					       itv_linexpr_t* expr, 
					       itv_t* p,
					       ap_linexpr0_t* linexpr0);
  /* Convert an ap_linexpr0_t into an itv_linexpr_t
     Return true if the conversion is exact
     If p==NULL, no linearization,
     If p!=NULL, transformation into a quasilinear expression
  */
static inline bool itv_lincons_set_ap_lincons0(itv_internal_t* intern,
					       itv_lincons_t* lincons,
					       itv_t* p,
					       ap_lincons0_t* cons);
  /* Convert an ap_lincons0_t into an itv_lincons_t
     Return true if the conversion is exact
     If p==NULL, no linearization,
     If p!=NULL, transformation into a quasilinear expression
  */

static inline bool itv_ap_linexpr0_set_ap_linexpr0(itv_internal_t* intern,
						   ap_linexpr0_t* rlinexpr0, 
						   itv_t* p,
						   ap_linexpr0_t* linexpr0);
  /* Linearize an ap_linexpr0 using the array of itv_t,
     by converting them first to itv types, and then back to ap_linexpr0.
  */
static inline bool itv_ap_lincons0_set_ap_lincons0(itv_internal_t* intern,
						   ap_lincons0_t* rlincons0, 
						   itv_t* p,
						   ap_lincons0_t* lincons0);
  /* Linearize an ap_lincons0 using the array of itv_t,
     by converting them first to itv types, and then back to ap_lincons0.
  */

/* ********************************************************************** */
/* III. Evaluation of expressions  */
/* ********************************************************************** */

static inline void itv_eval_itv_linexpr(itv_internal_t* intern,
					itv_t itv,
					itv_t* p,
					itv_linexpr_t* expr);
  /* Evaluate the interval linear expression, using the array p[]
     associating intervals to dimensions */

static inline bool itv_eval_ap_linexpr0(itv_internal_t* intern,
					itv_t itv,
					itv_t* p,
					ap_linexpr0_t* expr);
  /* Evaluate the interval linear expression, using the array p[]
     associating intervals to dimensions.

     Return true if all conversions were exact */

/* ********************************************************************** */
/* IV. Arithmetic */
/* ********************************************************************** */

static inline void itv_linexpr_neg(itv_linexpr_t* expr);
  /* Negate in-place an expression */
static inline void itv_linexpr_scale(itv_internal_t* intern,
				     itv_linexpr_t* expr, itv_t coeff);
  /* Scale in-place an expression by an interval */

static inline itv_linexpr_t itv_linexpr_add(itv_internal_t* intern,
					    itv_linexpr_t* exprA,
					    itv_linexpr_t* exprB);
static inline itv_linexpr_t itv_linexpr_sub(itv_internal_t* intern,
					    itv_linexpr_t* exprA,
					    itv_linexpr_t* exprB);
  /* Resp. add and substract two linear epxression.
     (Substraction temporarily negates exprB,a nd then restores it */

/* ********************************************************************** */
/* Definition of inline functions */
/* ********************************************************************** */

void ITVFUN(linexpr_init)(itv_linexpr_t* expr, size_t size);
static inline void itv_linexpr_init(itv_linexpr_t* expr, size_t size)
{ ITVFUN(linexpr_init)(expr,size); }

void ITVFUN(linexpr_reinit)(itv_linexpr_t* expr, size_t size);
static inline void itv_linexpr_reinit(itv_linexpr_t* expr, size_t size)
{ ITVFUN(linexpr_reinit)(expr,size); }

void ITVFUN(linexpr_clear)(itv_linexpr_t* expr);
static inline void itv_linexpr_clear(itv_linexpr_t* expr)
{ ITVFUN(linexpr_clear)(expr); }

static inline
void itv_lincons_init(itv_lincons_t* cons)
{
  itv_linexpr_init(&cons->linexpr,0);
  num_init(cons->num);
}
static inline
void itv_lincons_clear(itv_lincons_t* cons)
{
  itv_linexpr_clear(&cons->linexpr);
  num_clear(cons->num);
}

bool ITVFUN(linexpr_set_ap_linexpr0)(itv_internal_t* intern,
				     itv_linexpr_t* expr, 
				     itv_t* p,
				     ap_linexpr0_t* linexpr0);
static inline bool itv_linexpr_set_ap_linexpr0(itv_internal_t* intern,
					       itv_linexpr_t* expr, 
					       itv_t* p,
					       ap_linexpr0_t* linexpr0)
{ return ITVFUN(linexpr_set_ap_linexpr0)(intern,expr,p,linexpr0); }

bool ITVFUN(lincons_set_ap_lincons0)(itv_internal_t* intern,
				     itv_lincons_t* cons, 
				     itv_t* p,
				     ap_lincons0_t* lincons0);
static inline bool itv_lincons_set_ap_lincons0(itv_internal_t* intern,
					       itv_lincons_t* cons, 
					       itv_t* p,
					       ap_lincons0_t* lincons0)
{ return ITVFUN(lincons_set_ap_lincons0)(intern,cons,p,lincons0); }

bool ITVFUN(ap_linexpr0_set_ap_linexpr0)(itv_internal_t* intern,
					 ap_linexpr0_t* rlinexpr0, 
					 itv_t* p,
					 ap_linexpr0_t* linexpr0);
static inline bool itv_ap_linexpr0_set_ap_linexpr0(itv_internal_t* intern,
						   ap_linexpr0_t* rlinexpr0, 
						   itv_t* p,
						   ap_linexpr0_t* linexpr0)
{ return ITVFUN(ap_linexpr0_set_ap_linexpr0)(intern,rlinexpr0,p,linexpr0); }

bool ITVFUN(ap_lincons0_set_ap_lincons0)(itv_internal_t* intern,
					 ap_lincons0_t* rlincons0, 
					 itv_t* p,
					 ap_lincons0_t* lincons0);
static inline bool itv_ap_lincons0_set_ap_lincons0(itv_internal_t* intern,
						   ap_lincons0_t* rlincons0, 
						   itv_t* p,
						   ap_lincons0_t* lincons0)
{ return ITVFUN(ap_lincons0_set_ap_lincons0)(intern,rlincons0,p,lincons0); }

void ITVFUN(eval_itv_linexpr)(itv_internal_t* intern,
			      itv_t itv,
			      itv_t* p,
			      itv_linexpr_t* expr);
static inline void itv_eval_itv_linexpr(itv_internal_t* intern,
					itv_t itv,
					itv_t* p,
					itv_linexpr_t* expr)
{ ITVFUN(eval_itv_linexpr)(intern,itv,p,expr); }

bool ITVFUN(eval_ap_linexpr0)(itv_internal_t* intern,
			      itv_t itv,
			      itv_t* p,
			      ap_linexpr0_t* expr);
static inline bool itv_eval_ap_linexpr0(itv_internal_t* intern,
					itv_t itv,
					itv_t* p,
					ap_linexpr0_t* expr)
{ return ITVFUN(eval_ap_linexpr0)(intern,itv,p,expr); }

void ITVFUN(linexpr_neg)(itv_linexpr_t* expr);
static inline void itv_linexpr_neg(itv_linexpr_t* expr)
{ ITVFUN(linexpr_neg)(expr); }

void ITVFUN(linexpr_scale)(itv_internal_t* intern,
			   itv_linexpr_t* expr, itv_t coeff);
static inline void itv_linexpr_scale(itv_internal_t* intern,
				     itv_linexpr_t* expr, itv_t coeff)
{ ITVFUN(linexpr_scale)(intern,expr,coeff); }

void ITVFUN(linexpr_div)(itv_internal_t* intern,
			   itv_linexpr_t* expr, itv_t coeff);
static inline void itv_linexpr_div(itv_internal_t* intern,
				     itv_linexpr_t* expr, itv_t coeff)
{ ITVFUN(linexpr_div)(intern,expr,coeff); }

itv_linexpr_t ITVFUN(linexpr_add)(itv_internal_t* intern,
				  itv_linexpr_t* exprA,
				  itv_linexpr_t* exprB);
static inline itv_linexpr_t itv_linexpr_add(itv_internal_t* intern,
					    itv_linexpr_t* exprA,
					    itv_linexpr_t* exprB)
{ return ITVFUN(linexpr_add)(intern,exprA,exprB); }

itv_linexpr_t ITVFUN(linexpr_sub)(itv_internal_t* intern,
				  itv_linexpr_t* exprA,
				  itv_linexpr_t* exprB);
static inline itv_linexpr_t itv_linexpr_sub(itv_internal_t* intern,
					    itv_linexpr_t* exprA,
					    itv_linexpr_t* exprB)
{ return ITVFUN(linexpr_sub)(intern,exprA,exprB); }

#ifdef __cplusplus
}
#endif

#endif
