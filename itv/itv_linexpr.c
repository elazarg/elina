/* ********************************************************************** */
/* itv_linexpr.c: */
/* ********************************************************************** */

#include "itv_linexpr.h"

void itv_linexpr_init(itv_linexpr_t* expr, size_t size)
{
  expr->linterm = NULL;
  expr->size = 0;
  itv_init(expr->cst);
  expr->equality = true;
  itv_linexpr_reinit(expr,size);
}
void itv_linexpr_reinit(itv_linexpr_t* expr, size_t size)
{
  size_t i;
  expr->linterm = realloc(expr->linterm,size*sizeof(itv_linterm_t));

  for (i=expr->size;i<size;i++){
    itv_init(expr->linterm[i].itv);
    expr->linterm[i].equality = true;
  }
  for  (i=size; i<expr->size; i++){
    itv_clear(expr->linterm[i].itv);
  }
  expr->size = size;
  return;
}
void itv_linexpr_clear(itv_linexpr_t* expr)
{
  size_t i;
  if (expr->linterm){
    for (i=0;i<expr->size;i++){
      itv_clear(expr->linterm[i].itv);
    }
    free(expr->linterm);
    expr->linterm = NULL;
    expr->size = 0;
  }
  itv_clear(expr->cst);
}
void itv_linexpr_set_ap_linexpr0(itv_internal_t* intern,
				 itv_linexpr_t* expr, const ap_linexpr0_t* linexpr0)
{
  size_t i,k,size;
  ap_dim_t dim;
  ap_coeff_t* coeff;

  size=0;
  ap_linexpr0_ForeachLinterm(linexpr0,i,dim,coeff){
    size++;
  }
  itv_linexpr_reinit(expr,size);
  expr->equality = itv_set_ap_coeff(intern, expr->cst, &linexpr0->cst);
  k = 0;
  ap_linexpr0_ForeachLinterm(linexpr0,i,dim,coeff){
    expr->linterm[k].dim = dim;
    expr->linterm[k].equality = itv_set_ap_coeff(intern,
						 expr->linterm[k].itv,
						 coeff);
    k++;
  }
}
void itv_lincons_set_ap_lincons0(itv_internal_t* intern,
				 itv_lincons_t* cons, const ap_lincons0_t* lincons0)
{
  itv_linexpr_set_ap_linexpr0(intern, &cons->linexpr,lincons0->linexpr0);
  cons->constyp = lincons0->constyp;
}

/* Evaluate an interval linear expression */
void itv_eval_itv_linexpr(itv_internal_t* intern,
			  itv_t itv,
			  const itv_t* p,
			  const itv_linexpr_t* expr)
{
  int i;
  ap_dim_t dim;
  itv_ptr pitv;
  bool* peq;
  assert(p);

  itv_set(itv, expr->cst);
  itv_linexpr_ForeachLinterm(expr,i,dim,pitv,peq){
    if (*peq){
      if (num_sgn(pitv->sup)!=0){
	itv_mul_bound(intern,
		      intern->eval_itv,
		      p[dim],
		      pitv->sup);
	itv_add(itv, itv, intern->eval_itv);
      }
    }
    else {
      itv_mul(intern,
	      intern->eval_itv,
	      p[dim],
	      pitv);
      itv_add(itv, itv, intern->eval_itv);
    }
    if (itv_is_top(itv))
      break;
  }
}

/* Evaluate an interval linear expression */
void itv_eval_ap_linexpr0(itv_internal_t* intern,
			  itv_t itv,
			  const itv_t* p,
			  const ap_linexpr0_t* expr)
{
  int i;
  ap_dim_t dim;
  ap_coeff_t* pcoeff;
  assert(p);

  itv_set_ap_coeff(intern,itv, &expr->cst);
  ap_linexpr0_ForeachLinterm(expr,i,dim,pcoeff){
    bool eq = itv_set_ap_coeff(intern,intern->eval_itv2,pcoeff);
    if (eq){
      if (num_sgn(intern->eval_itv2->sup)!=0){
	itv_mul_bound(intern,
		      intern->eval_itv,
		      p[dim],
		      intern->eval_itv2->sup);
	itv_add(itv, itv, intern->eval_itv);
      }
    }
    else {
      itv_mul(intern,
	      intern->eval_itv,
	      p[dim],
	      intern->eval_itv2);
      itv_add(itv, itv, intern->eval_itv);
    }
    if (itv_is_top(itv))
      break;
  }
}

