/*
 * oct_nary.c
 *
 * N-ary octagon functions: meet, join and widening
 *
 * APRON Library / Octagonal Domain
 *
 * Copyright (C) Antoine Mine' 2006
 *
 */

#include "oct.h"
#include "oct_internal.h"

/* ============================================================ */
/* Meet and Join */
/* ============================================================ */

oct_t* oct_meet(ap_manager_t* man, bool destructive, oct_t* a1, const oct_t* a2)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_MEET,0);
  bound_t* m;
  arg_assert(a1->dim==a2->dim && a1->intdim==a2->intdim,return NULL;);
  if ((!a1->closed && !a1->m) || (!a2->closed && !a2->m))
    /* one argument is empty */
    return oct_set_mat(pr,a1,NULL,NULL,destructive);
  else {
    bound_t* m1 = a1->closed ? a1->closed : a1->m;
    bound_t* m2 = a2->closed ? a2->closed : a2->m;
    size_t i;
    m = destructive ? m1 : hmat_alloc(pr,a1->dim);
    for (i=0;i<matsize(a1->dim);i++)
      bound_min(m[i],m1[i],m2[i]);
    /* optimal, but not closed */
    return oct_set_mat(pr,a1,m,NULL,destructive);
  }
}

oct_t* oct_join(ap_manager_t* man, bool destructive, oct_t* a1, const oct_t* a2)
{
 oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_JOIN,0);
 arg_assert(a1->dim==a2->dim && a1->intdim==a2->intdim,return NULL;);
 if (pr->funopt->algorithm>=0) {
   oct_cache_closure(pr,a1);
   oct_cache_closure(pr,a2);
 }
 if (!a1->closed && !a1->m) {
   if (!a2->closed && !a2->m)
     /* both empty */
     return oct_set_mat(pr,a1,NULL,NULL,destructive);
   else
     /* a1 empty, a2 not empty */
     return oct_set_mat(pr,a1,hmat_copy(pr,a2->m,a2->dim),
			hmat_copy(pr,a2->closed,a2->dim),destructive);
 }
 else if (!a2->closed && !a2->m)
   /* a1 not empty, a2 empty */
   return oct_set_mat(pr,a1,a1->m,a1->closed,destructive);
 else {
   /* not empty */
   bound_t* m1 = a1->closed ? a1->closed : a1->m;
   bound_t* m2 = a2->closed ? a2->closed : a2->m;
   bound_t* m = destructive ? m1 : hmat_alloc(pr,a1->dim);
   size_t i;
   man->result.flag_exact = tbool_top;
   for (i=0;i<matsize(a1->dim);i++)
     bound_max(m[i],m1[i],m2[i]);
   if (a1->closed && a2->closed) {
     /* result is closed and optimal on Q */
     if (num_incomplete || a1->intdim) flag_incomplete;
     return oct_set_mat(pr,a1,NULL,m,destructive);
   }
   else {
     /* not optimal, not closed */
     flag_algo;
     return oct_set_mat(pr,a1,m,NULL,destructive); 
   }
 }
}

oct_t* oct_meet_array(ap_manager_t* man, const oct_t** tab, size_t size)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_MEET_ARRAY,0);
  oct_t* r;
  size_t i,k;
  arg_assert(size>0,return NULL;);
  r = oct_alloc_internal(pr,tab[0]->dim,tab[0]->intdim);
  /* check whether there is an empty element */
  for (k=0;k<size;k++)
    if (!tab[k]->m && !tab[k]->closed) return r;
  /* all elements are non-empty */
  r->m = hmat_copy(pr,tab[0]->closed ? tab[0]->closed : tab[0]->m,r->dim);
  for (k=1;k<size;k++) {
    bound_t* x = tab[k]->closed ? tab[k]->closed : tab[k]->m;
    arg_assert(tab[k]->dim==r->dim && tab[k]->intdim==r->intdim,
	       oct_free_internal(pr,r);return NULL;);
    for (i=0;i<matsize(r->dim);i++)
      bound_min(r->m[i],r->m[i],x[i]);
  }
  return r;
}

oct_t* oct_join_array(ap_manager_t* man, const oct_t** tab, size_t size)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_JOIN_ARRAY,0);
  int algo = pr->funopt->algorithm;
  bool closed = true;
  oct_t* r;
  bound_t* m = NULL;
  size_t i,k;
  arg_assert(size>0,return NULL;);
  r = oct_alloc_internal(pr,tab[0]->dim,tab[0]->intdim);
  for (k=0;k<size;k++) {
    arg_assert(tab[k]->dim==r->dim && tab[k]->intdim==r->intdim,
	       oct_free_internal(pr,r);return NULL;);
    if (algo>=0) oct_cache_closure(pr,tab[k]);
    /* skip definitively empty */
    if (!tab[k]->m && !tab[k]->closed) continue;
    if (!m)
      /* first non-empty */
      m = hmat_copy(pr,tab[k]->closed ? tab[k]->closed : tab[k]->m,r->dim);
    else {
      /* not first non-empty */
      bound_t* x = tab[k]->closed ? tab[k]->closed : tab[k]->m;
      for (i=0;i<matsize(r->dim);i++)
	bound_max(m[i],m[i],x[i]);
    }
    if (!tab[k]->closed) closed = false;
  }

  if (!m) {
    /* empty result */
  }
  else if (closed) { 
    /* closed, optimal result, in Q */
    man->result.flag_exact = tbool_top;
    r->closed = m; 
    if (num_incomplete || r->intdim) flag_incomplete;
  }
  else {
    /* non closed, non optimal result */
    r->m = m; 
    flag_algo; 
  }
  return r;
}



/* ============================================================ */
/* Widening, Narrowing */
/* ============================================================ */

oct_t* oct_widening(ap_manager_t* man, const oct_t* a1, const oct_t* a2)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_WIDENING,0);
  int algo = pr->funopt->algorithm;
  oct_t* r;
  arg_assert(a1->dim==a2->dim && a1->intdim==a2->intdim,return NULL;);
  if (algo>=0) oct_cache_closure(pr,a2);
  if (!a1->closed && !a1->m)
    /* a1 definitively closed */
    r = oct_copy_internal(pr,a2);
  else if (!a2->closed && !a2->m)
   /* a2 definitively closed */
    r = oct_copy_internal(pr,a1);
  else {
    /* work on the origial left matrix, not the closed cache! */
    bound_t* m1 = a1->m ? a1->m : a1->closed;
    bound_t* m2 = a2->closed ? a2->closed : a2->m;
    size_t i;
    r = oct_alloc_internal(pr,a1->dim,a1->intdim);
    r->m = hmat_alloc(pr,r->dim);
    if (algo==oct_pre_widening || algo==-oct_pre_widening) {
      /* degenerate hull: NOT A PROPER WIDENING, use with care */
      for (i=0;i<matsize(r->dim);i++)
	bound_max(r->m[i],m1[i],m2[i]);
    }
    else {
      /* standard widening */
      for (i=0;i<matsize(r->dim);i++)
	if (bound_cmp(m1[i],m2[i])>=0) bound_set(r->m[i],m1[i]);
	else bound_set_infty(r->m[i]);
    }
  }
  return r;
}

oct_t* oct_widening_threshold(ap_manager_t* man,
			      const oct_t* a1, const oct_t* a2,
			      const ap_scalar_t** array,
			      size_t nb)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_WIDENING,nb+1);
  int algo = pr->funopt->algorithm;
  oct_t* r;
  arg_assert(a1->dim==a2->dim && a1->intdim==a2->intdim,return NULL;);
  if (algo>=0) oct_cache_closure(pr,a2);
  if (!a1->closed && !a1->m)
    /* a1 definitively closed */
    r = oct_copy_internal(pr,a2);
  else if (!a2->closed && !a2->m)
   /* a2 definitively closed */
    r = oct_copy_internal(pr,a1);
  else {
    /* work on the origial left matrix, not the closed cache! */
    bound_t* m1 = a1->m ? a1->m : a1->closed;
    bound_t* m2 = a2->closed ? a2->closed : a2->m;
    size_t i;
    r = oct_alloc_internal(pr,a1->dim,a1->intdim);
    r->m = hmat_alloc(pr,r->dim);
    /* convert array to bounds */
    for (i=0;i<nb;i++)
      bound_of_scalar(pr,pr->tmp[i],array[i],false,false);
    bound_set_infty(pr->tmp[nb]);
    /* point-wise loop */
    for (i=0;i<matsize(r->dim);i++) 
      if (bound_cmp(m1[i],m2[i])>=0) bound_set(r->m[i],m1[i]);
      else {
	/* dichotomy on array */
	size_t low=0, hi=nb;
	while (low<hi) {
	  size_t med = (low+hi)/2;
	  if (bound_cmp(pr->tmp[med],m2[i])<0) low=med+1;
	  else hi=med;
	}
	bound_set(r->m[i],pr->tmp[low]);
      }
  }
  return r;
}


/* TODO: get some AP_FUNID_NARROWING */
oct_t* oct_narrowing(ap_manager_t* man, const oct_t* a1, const oct_t* a2)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_WIDENING,0);
  oct_t* r;
  arg_assert(a1->dim==a2->dim && a1->intdim==a2->intdim,return NULL;);
  if (pr->funopt->algorithm>=0) {
    oct_cache_closure(pr,a1);
    oct_cache_closure(pr,a2);
  }
  r = oct_alloc_internal(pr,a1->dim,a1->intdim);
  if ((!a1->closed && !a1->m) || (!a2->closed && !a2->m)) {
    /* a1 or a2 definitively closed */
  }
  else {
    bound_t* m1 = a1->closed ? a1->closed : a1->m;
    bound_t* m2 = a2->closed ? a2->closed : a2->m;
    size_t i;
    r->m = hmat_alloc(pr,r->dim);
    for (i=0;i<matsize(r->dim);i++)
      bound_set(r->m[i], bound_infty(m1[i]) ? m2[i] : m1[i]);
  }
  return r;
}

