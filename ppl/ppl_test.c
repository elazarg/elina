/*
 * ppl_test.c
 *
 * Unit testing. Compares NewPolka & PPL implementations.
 *
 * APRON Library /  PPL library wrapper
 *
 * Copyright (C) Antoine Mine' 2006
 *
 */

/* This file is part of the APRON Library, released under LGPL license.  
   Please read the COPYING file packaged in the distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "ap_global0.h"
#include "pk.h"
#include "apron_ppl.h"

ap_manager_t* ppl;
ap_manager_t* pk;


/* utilities */
/* ********* */

void random_interval(ap_interval_t* i)
{
  int n1 = lrand48()%20-10;
  int n2 = n1 + lrand48()%20;
  int d  = lrand48()%3+1;
  ap_interval_set_frac(i,n1,d,n2,d);
}

/* random affine expression */
ap_linexpr0_t* random_linexpr(int dim)
{
  ap_linexpr0_t* l = ap_linexpr0_alloc(AP_LINEXPR_DENSE,dim);
  int i;
  for (i=0;i<dim;i++)
    ap_coeff_set_scalar_frac(l->p.coeff+i,lrand48()%20-10,lrand48()%4+1);
  ap_coeff_set_scalar_frac(&l->cst,lrand48()%20-10,lrand48()%4+1);
  return l;
}

/* random affine expression with scalar constant coefficient */
ap_linexpr0_t* random_linexpr_inter(int dim)
{
  ap_linexpr0_t* l = random_linexpr(dim);
  int n1 = lrand48()%20-10;
  int n2 = n1 + lrand48()%20;
  int d  = lrand48()%4+1;
  ap_linexpr0_set_cst_interval_frac(l,n1,d,n2,d);
  return l;
}

/* random generator of specified type */
ap_generator0_t random_generator(int dim, ap_gentyp_t g)
{
  ap_linexpr0_t* l = ap_linexpr0_alloc(AP_LINEXPR_DENSE,dim);
  int i;
  for (i=0;i<dim;i++)
    ap_coeff_set_scalar_frac(l->p.coeff+i,lrand48()%20-2,lrand48()%4+1);
  ap_coeff_set_scalar_int(&l->cst,1);
  return ap_generator0_make(g,l);
}

/* random polyhedron */
ap_abstract0_t* random_poly(ap_manager_t* man,int dim)
{
  int i;
  ap_abstract0_t* p;
  ap_interval_t** t = ap_interval_array_alloc(dim);
  ap_generator0_array_t ar = ap_generator0_array_make(dim);
  for (i=0;i<dim;i++)
    ap_interval_set_int(t[i],0,0);
  for (i=0;i<dim;i++)
    ar.p[i] = random_generator(dim,(lrand48()%100>=90)?AP_GEN_RAY:AP_GEN_VERTEX);
  //ap_generator0_array_fprint(stderr,&ar,NULL);
  p = ap_abstract0_of_box(man,0,dim,(const ap_interval_t**)t);
  ap_abstract0_add_ray_array(man,true,p,&ar);
  ap_generator0_array_clear(&ar);
  ap_interval_array_free(t,dim);
  return p;
}

/* convert to specified manager */
ap_abstract0_t* convert(ap_manager_t* man, ap_abstract0_t* a)
{
  ap_lincons0_array_t l;
  ap_dimension_t d = ap_abstract0_dimension(a->man,a);
  if (a->man==man) return a;
  l = ap_abstract0_to_lincons_array(a->man,a);
  a = ap_abstract0_of_lincons_array(man,0,d.realdim,&l);
  ap_lincons0_array_clear(&l);
  return a;
}

/* compare */
tbool_t is_eq(ap_abstract0_t* a1,ap_abstract0_t* a2)
{
  ap_abstract0_t* aa2 = convert(a1->man,a2);
  tbool_t r = ap_abstract0_is_eq(a1->man,a1,aa2);
  if (a2!=aa2) ap_abstract0_free(aa2->man,aa2);
  return r;
}

/* print */
void print_poly(const char* msg, ap_abstract0_t* p)
{
  fprintf(stderr,"%s (%s) = ",msg,p->man->library);
  ap_abstract0_fprint(stderr,p->man,p,NULL);
  ap_abstract0_fdump(stderr,p->man,p);
  fprintf(stderr,"\n");
}


/* loop */
/* **** */

#define N 60

char b1_[N+4]= " [";
char b2_[N+4];
int i_;
int error_ = 0;

#define LOOP								\
  {									\
    memset(b1_+2,' ',N); b1_[N+2]=']'; b1_[N+3]=0;			\
    memset(b2_,8,N+3); b2_[N+3]=0;					\
    for (i_=0;i_<N;i_++) {						\
      printf("%s%s",b1_,b2_);						\
      fflush(stdout);
      
#define RESULT(c) b1_[i_+2]=c

#define ERROR(msg)					\
  do {							\
    fprintf(stderr,"\n%s\n",msg);			\
    RESULT('!');					\
    error_++;						\
  } while (0)

#define ENDLOOP	} } printf("%s%s\n",b1_,b2_)




/* tests */
/* ***** */

void test_conv()
{
  int dim = 5;
  printf("\nconversions\n");
  LOOP {
    ap_abstract0_t* pk0 = random_poly(pk,dim);
    ap_abstract0_t* ppl0 = convert(ppl,pk0);
    ap_abstract0_t* pk1 = convert(pk,ppl0);
    ap_abstract0_t* ppl1 = convert(ppl,pk1);
    RESULT('*');
    if (is_eq(pk0,pk1)!=tbool_true || is_eq(ppl0,ppl1)!=tbool_true) {
      ERROR("different results");
      print_poly("pk0",pk0); print_poly("ppl0",ppl0);
      print_poly("pk1",pk1); print_poly("ppl1",ppl1);
    }
    ap_abstract0_free(pk,pk0); ap_abstract0_free(pk,pk1);
    ap_abstract0_free(ppl,ppl0); ap_abstract0_free(ppl,ppl1);
  } ENDLOOP;
}


void test_join()
{
  int dim = 4;
  printf("\nbinary join\n");
  LOOP {
    ap_abstract0_t* pk0 = random_poly(pk,dim);
    ap_abstract0_t* pk1 = random_poly(pk,dim);
    ap_abstract0_t* pkr = ap_abstract0_join(pk,false,pk0,pk1);
    ap_abstract0_t* ppl0 = convert(ppl,pk0);
    ap_abstract0_t* ppl1 = convert(ppl,pk1);
    ap_abstract0_t* pplr = ap_abstract0_join(ppl,false,ppl0,ppl1);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      print_poly("pk0",pk0); print_poly("pk1",pk1);
      print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pk0); ap_abstract0_free(pk,pk1); ap_abstract0_free(pk,pkr);
    ap_abstract0_free(ppl,ppl0); ap_abstract0_free(ppl,ppl1); ap_abstract0_free(ppl,pplr);
  } ENDLOOP;
}

#define NB 3

void test_join_array()
{
  int i,dim = 3;
  printf("\narray join\n");
  LOOP {
    ap_abstract0_t* pka[NB], *pkr;
    ap_abstract0_t* ppla[NB], *pplr;
    for (i=0;i<NB;i++) {
      pka[i] = random_poly(pk,dim);
      ppla[i] = convert(ppl,pka[i]);
    }
    pkr = ap_abstract0_join_array(pk,(const ap_abstract0_t*const*)pka,NB);
    pplr = ap_abstract0_join_array(ppl,(const ap_abstract0_t*const*)ppla,NB);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    for (i=0;i<NB;i++) { ap_abstract0_free(pk,pka[i]); ap_abstract0_free(ppl,ppla[i]); }
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
  } ENDLOOP;
}

void test_meet()
{
  int dim = 4;
  printf("\nbinary meet\n");
  LOOP {
    ap_abstract0_t* pk0 = random_poly(pk,dim);
    ap_abstract0_t* pk1 = random_poly(pk,dim);
    ap_abstract0_t* pkr = ap_abstract0_meet(pk,false,pk0,pk1);
    ap_abstract0_t* ppl0 = convert(ppl,pk0);
    ap_abstract0_t* ppl1 = convert(ppl,pk1);
    ap_abstract0_t* pplr = ap_abstract0_meet(ppl,false,ppl0,ppl1);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      print_poly("pk0",pk0); print_poly("pk1",pk1); 
      print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pk0); ap_abstract0_free(pk,pk1); ap_abstract0_free(pk,pkr);
    ap_abstract0_free(ppl,ppl0); ap_abstract0_free(ppl,ppl1); ap_abstract0_free(ppl,pplr);
  } ENDLOOP;
}

void test_meet_array()
{
  int i,dim = 3;
  printf("\narray meet\n");
  LOOP {
    ap_abstract0_t* pka[NB], *pkr;
    ap_abstract0_t* ppla[NB], *pplr;
    for (i=0;i<NB;i++) {
      pka[i] = random_poly(pk,dim);
      ppla[i] = convert(ppl,pka[i]);
    }
    pkr = ap_abstract0_meet_array(pk,(const ap_abstract0_t*const*)pka,NB);
    pplr = ap_abstract0_meet_array(ppl,(const ap_abstract0_t*const*)ppla,NB);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    for (i=0;i<NB;i++) { ap_abstract0_free(pk,pka[i]); ap_abstract0_free(ppl,ppla[i]); }
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
  } ENDLOOP;
}


void test_dimadd(void)
{
  printf("\nadd dimensions\n");
  LOOP {
    size_t i, dim = 8;
    ap_dimchange_t* a = ap_dimchange_alloc(0,3);
    ap_abstract0_t* pka,*pkr, *ppla,*pplr;
    int proj = lrand48()%2;
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    for (i=0;i<a->realdim;i++) {
      a->dim[i] = lrand48()%3;
      if (i) a->dim[i] += a->dim[i-1];
      assert(a->dim[i]<dim);
    }
    pkr = ap_abstract0_add_dimensions(pk,false,pka,a,proj);
    pplr = ap_abstract0_add_dimensions(ppl,false,ppla,a,proj);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      ap_dimchange_fprint(stderr,a);
      print_poly("pka",pka); print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
    ap_dimchange_free(a);
  } ENDLOOP;
}

void test_dimrem(void)
{
  printf("\nremove dimensions\n");
  LOOP {
    size_t i, dim = 8;
    ap_dimchange_t* a = ap_dimchange_alloc(0,2);
    ap_abstract0_t* pka,*pkr, *ppla,*pplr;
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    for (i=0;i<a->realdim;i++) {
      a->dim[i] = lrand48()%3 + 1;
      if (i) a->dim[i] += a->dim[i-1];
      assert(a->dim[i]<dim);
    }
    pkr = ap_abstract0_remove_dimensions(pk,false,pka,a);
    pplr = ap_abstract0_remove_dimensions(ppl,false,ppla,a);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      ap_dimchange_fprint(stderr,a);
      print_poly("pka",pka); print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
    ap_dimchange_free(a);
  } ENDLOOP;
}

void test_forget(void)
{
  printf("\nforget\n");
  LOOP {
    size_t i, dim = 8;
    ap_dimchange_t* a = ap_dimchange_alloc(0,2);
    ap_abstract0_t* pka,*pkr, *ppla,*pplr;
    int proj = lrand48()%2;
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    for (i=0;i<a->realdim;i++) {
      a->dim[i] = lrand48()%3 + 1;
      if (i) a->dim[i] += a->dim[i-1];
      assert(a->dim[i]<dim);
    }
    pkr = ap_abstract0_forget_array(pk,false,pka,a->dim,a->realdim,proj);
    pplr = ap_abstract0_forget_array(ppl,false,ppla,a->dim,a->realdim,proj);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      ap_dimchange_fprint(stderr,a);
      print_poly("pka",pka); print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
    ap_dimchange_free(a);
  } ENDLOOP;
}

void test_permute(void)
{
  printf("\npermute dimensions\n");
  LOOP {
    size_t i, dim = 6;
    ap_dimperm_t* p = ap_dimperm_alloc(dim);
    ap_abstract0_t* pka,*pkr, *ppla,*pplr;
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    /* random permutation */
    ap_dimperm_set_id(p);
    for (i=0;i<p->size-1;i++) {
      int j = i + (lrand48() % (p->size-i));
      int a = p->dim[j]; p->dim[j] = p->dim[i]; p->dim[i] = a;
    }
    pkr = ap_abstract0_permute_dimensions(pk,false,pka,p);
    pplr = ap_abstract0_permute_dimensions(ppl,false,ppla,p);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      ap_dimperm_fprint(stderr,p);
      print_poly("pka",pka); print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
    ap_dimperm_free(p);
  } ENDLOOP;
}

void test_expand(void)
{
  printf("\nexpand dimensions\n");
  LOOP {
    size_t i, dim = 6;
    ap_dim_t d = lrand48() % dim;
    size_t n = (lrand48() % 2) + 1;
    ap_abstract0_t* pka,*pkr, *ppla,*pplr;
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    pkr = ap_abstract0_expand(pk,false,pka,d,n);
    pplr = ap_abstract0_expand(ppl,false,ppla,d,n);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      fprintf(stderr,"dim %i expanded %i times\n",(int)d,(int)n);
      print_poly("pka",pka); print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
  } ENDLOOP;
}

void test_fold(void)
{
  printf("\nfold dimensions\n");
  LOOP {
    size_t i, dim = 6;
    ap_dim_t dd[3];
    ap_abstract0_t* pka,*pkr, *ppla,*pplr;
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    dd[0] = lrand48() % (dim-3);
    dd[1] = dd[0] + 1 + (lrand48() % (dim-2-dd[0]));
    dd[2] = dd[1] + 1 + (lrand48() % (dim-1-dd[1]));
    pkr = ap_abstract0_fold(pk,false,pka,dd,3);
    pplr = ap_abstract0_fold(ppl,false,ppla,dd,3);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      fprintf(stderr,"fold %i,%i,%i\n",(int)dd[0],(int)dd[1],(int)dd[2]);
      print_poly("pka",pka); print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
  } ENDLOOP;
}


void test_add_lincons(void)
{
  printf("\nadd lincons\n");
  LOOP {
    size_t i, dim = 6, nb = 4;
    ap_abstract0_t* pka,*pkr, *ppla,*pplr;
    ap_lincons0_array_t ar = ap_lincons0_array_make(nb);
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    for (i=0;i<nb;i++) {
      ar.p[i] = ap_lincons0_make((lrand48()%100>=90)?AP_CONS_EQ:
				 (lrand48()%100>=90)?AP_CONS_SUP:AP_CONS_SUPEQ,
				 random_linexpr(dim));
    }
    pkr = ap_abstract0_meet_lincons_array(pk,false,pka,&ar);
    pplr = ap_abstract0_meet_lincons_array(ppl,false,ppla,&ar);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      ap_lincons0_array_fprint(stderr,&ar,NULL);
      print_poly("pka",pka); print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
    ap_lincons0_array_clear(&ar);
  } ENDLOOP;
}

void test_add_ray(void)
{
  printf("\nadd rays\n");
  LOOP {
    size_t i, dim = 4, nb = 4;
    ap_abstract0_t* pka,*pkr, *ppla,*pplr;
    ap_generator0_array_t ar = ap_generator0_array_make(nb);
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    for (i=0;i<nb;i++)
      ar.p[i] = random_generator(dim,(lrand48()%100>=80)?AP_GEN_LINE:AP_GEN_RAY);
    pkr = ap_abstract0_add_ray_array(pk,false,pka,&ar);
    pplr = ap_abstract0_add_ray_array(ppl,false,ppla,&ar);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      ap_generator0_array_fprint(stderr,&ar,NULL);
      print_poly("pka",pka); print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
    ap_generator0_array_clear(&ar);
  } ENDLOOP;
}

void test_box(void)
{
  printf("\nbox conversion\n");
  LOOP {
    size_t i, dim = 6;
    ap_abstract0_t* pka,*pkr, *ppla,*pplr;
    ap_interval_t** pki,**ppli;
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    pki = ap_abstract0_to_box(pk,pka);
    ppli = ap_abstract0_to_box(ppl,ppla);
    pkr = ap_abstract0_of_box(pk,0,dim,(const ap_interval_t*const*)pki);
    pplr = ap_abstract0_of_box(ppl,0,dim,(const ap_interval_t*const*)ppli);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      for (i=0;i<dim;i++) {
	fprintf(stderr,"pki[%i]=",i); ap_interval_fprint(stderr,pki[i]);
	fprintf(stderr," ppli[%i]=",i); ap_interval_fprint(stderr,ppli[i]);
	fprintf(stderr,"\n");
      }
      print_poly("pka",pka); print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
    ap_interval_array_free(pki,dim); ap_interval_array_free(ppli,dim);
  } ENDLOOP;

}

void test_vbound(void)
{
  printf("\nvariable bound\n");
  LOOP {
    size_t i, dim = 6;
    ap_abstract0_t* pka,*ppla;
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    RESULT('*');
    for (i=0;i<dim;i++) {
      tbool_t pkd,ppld;
      ap_interval_t* pki,*ppli;
      pkd = ap_abstract0_is_dimension_unconstrained(pk,pka,i);
      ppld = ap_abstract0_is_dimension_unconstrained(ppl,ppla,i);
      pki = ap_abstract0_bound_dimension(pk,pka,i);
      ppli = ap_abstract0_bound_dimension(ppl,ppla,i);
      if (pkd!=ppld || ap_interval_cmp(pki,ppli)) {
	ERROR("different results");
	print_poly("pka",pka);
	fprintf(stderr,"pki[%i]=",i); ap_interval_fprint(stderr,pki);
	fprintf(stderr," ppli[%i]=",i); ap_interval_fprint(stderr,ppli);
	fprintf(stderr," pkd=%i ppld=%i\n",pkd,ppld);
      }
      ap_interval_free(pki);
      ap_interval_free(ppli);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
  } ENDLOOP;
}

void test_lbound(void)
{
  printf("\nlinear expression bound\n");
  LOOP {
    size_t dim = 6;
    ap_abstract0_t* pka,*ppla;
    ap_interval_t* pki,*ppli;
    ap_linexpr0_t* l = random_linexpr(dim);
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    pki = ap_abstract0_bound_linexpr(pk,pka,l);
    ppli = ap_abstract0_bound_linexpr(ppl,ppla,l);
    RESULT('*');
    if (ap_interval_cmp(pki,ppli)) {
      ERROR("different results");
      print_poly("pka",pka);
      ap_linexpr0_fprint(stderr,l,NULL);
      fprintf(stderr,"\npki="); ap_interval_fprint(stderr,pki);
      fprintf(stderr,"\nppli="); ap_interval_fprint(stderr,ppli);
      fprintf(stderr,"\n");
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_interval_free(pki); ap_interval_free(ppli);
    ap_linexpr0_free(l);
  } ENDLOOP;
}

void test_csat(void)
{
  printf("\nconstraint saturation\n");
  LOOP {
    size_t dim = 6;
    ap_abstract0_t* pka,*ppla;
    ap_lincons0_t l = ap_lincons0_make((lrand48()%100>=90)?AP_CONS_EQ:
				       (lrand48()%100>=90)?AP_CONS_SUP:AP_CONS_SUPEQ,
				       random_linexpr(dim));
    tbool_t pks,ppls;
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    pks = ap_abstract0_sat_lincons(pk,pka,&l);
    ppls = ap_abstract0_sat_lincons(ppl,ppla,&l);
    RESULT('*');
    if (pks!=ppls) {
      ERROR("different results");
      print_poly("pka",pka);
      ap_lincons0_fprint(stderr,&l,NULL);
      fprintf(stderr,"\npks=%i ppls=%i\n",pks,ppls);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_lincons0_clear(&l);
  } ENDLOOP;
}

void test_isat(void)
{
  printf("\ninterval saturation\n");
  LOOP {
    size_t dim = 6;
    size_t p = lrand48() % dim;
    ap_abstract0_t* pka,*ppla;
    ap_interval_t* i = ap_interval_alloc();
    tbool_t pks,ppls;
    random_interval(i);
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    pks = ap_abstract0_sat_interval(pk,pka,p,i);
    ppls = ap_abstract0_sat_interval(ppl,ppla,p,i);
    RESULT('*');
    if (pks!=ppls) {
      ERROR("different results");
      print_poly("pka",pka);
      ap_interval_fprint(stderr,i);
      fprintf(stderr,"\nvar=%i\npks=%i ppls=%i\n",p,pks,ppls);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_interval_free(i);
  } ENDLOOP;
}

void test_assign(void)
{
  printf("\nassign\n");
  LOOP {
    size_t i, dim = 7;
    size_t p = lrand48() % dim;
    ap_abstract0_t* pka,*pkr, *ppla,*pplr;
    ap_linexpr0_t* l = random_linexpr(dim);
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    pkr = ap_abstract0_assign_linexpr(pk,false,pka,p,l,NULL);
    pplr = ap_abstract0_assign_linexpr(ppl,false,ppla,p,l,NULL);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      fprintf(stderr,"x%i <- ",p); ap_linexpr0_fprint(stderr,l,NULL); fprintf(stderr,"\n");
      print_poly("pka",pka); print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
    ap_linexpr0_free(l);
  } ENDLOOP;
}

void test_par_assign(void)
{
  printf("\nparallel assign\n");
  LOOP {
    size_t i, dim = 7;
    size_t p = lrand48() % dim;
    ap_abstract0_t* pka,*pkr, *ppla,*pplr;
    ap_dim_t d[NB];
    ap_linexpr0_t *l[NB];
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    for (i=0;i<NB;i++) {
      l[i]  = random_linexpr(dim);
      if (!i) d[i] = lrand48()%(dim-NB);
      else d[i] = d[i-1] + 1 + (lrand48()%(dim-NB+i-d[i-1]));
    }
    pkr = ap_abstract0_assign_linexpr_array(pk,false,pka,d,(const ap_linexpr0_t**)l,NB,NULL);
    pplr = ap_abstract0_assign_linexpr_array(ppl,false,ppla,d,(const ap_linexpr0_t**)l,NB,NULL);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results"); 
      for (i=0;i<NB;i++) {
	fprintf(stderr,"x%i <- ",d[i]); 
	ap_linexpr0_fprint(stderr,l[i],NULL); 
	fprintf(stderr,"\n");
      }
      print_poly("pka",pka); print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
    for (i=0;i<NB;i++) ap_linexpr0_free(l[i]);
  } ENDLOOP;
}

void test_subst(void)
{
  printf("\nsubst\n");
  LOOP {
    size_t i, dim = 7;
    size_t p = lrand48() % dim;
    ap_abstract0_t* pka,*pkr, *ppla,*pplr;
    ap_linexpr0_t* l = random_linexpr(dim);
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    pkr = ap_abstract0_substitute_linexpr(pk,false,pka,p,l,NULL);
    pplr = ap_abstract0_substitute_linexpr(ppl,false,ppla,p,l,NULL);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      fprintf(stderr,"x%i -> ",p); ap_linexpr0_fprint(stderr,l,NULL); fprintf(stderr,"\n");
      print_poly("pka",pka); print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
    ap_linexpr0_free(l);
  } ENDLOOP;
}

void test_par_subst(void)
{
  printf("\nparallel subst\n");
  LOOP {
    size_t i, dim = 7;
    size_t p = lrand48() % dim;
    ap_abstract0_t* pka,*pkr, *ppla,*pplr;
    ap_dim_t d[NB];
    ap_linexpr0_t *l[NB];
    pka = random_poly(pk,dim);
    ppla = convert(ppl,pka);
    for (i=0;i<NB;i++) {
      l[i]  = random_linexpr(dim);
      if (!i) d[i] = lrand48()%(dim-NB);
      else d[i] = d[i-1] + 1 + (lrand48()%(dim-NB+i-d[i-1]));
    }
    pkr = ap_abstract0_substitute_linexpr_array(pk,false,pka,d,(const ap_linexpr0_t**)l,NB,NULL);
    pplr = ap_abstract0_substitute_linexpr_array(ppl,false,ppla,d,(const ap_linexpr0_t**)l,NB,NULL);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results"); 
      for (i=0;i<NB;i++) {
	fprintf(stderr,"x%i -> ",d[i]); 
	ap_linexpr0_fprint(stderr,l[i],NULL); 
	fprintf(stderr,"\n");
      }
      print_poly("pka",pka); print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pka); ap_abstract0_free(ppl,ppla);
    ap_abstract0_free(pk,pkr); ap_abstract0_free(ppl,pplr);
    for (i=0;i<NB;i++) ap_linexpr0_free(l[i]);
  } ENDLOOP;
}


void test_widen()
{
  int dim = 5;
  printf("\nwidening\n");
  LOOP {
    ap_abstract0_t* pk0, *pk1, *pkr;
    ap_abstract0_t* ppl0, *ppl1, *pplr;
    pk0 = random_poly(pk,dim);
    pk1 = random_poly(pk,dim);
    pk1 = ap_abstract0_join(pk,true,pk1,pk0);
    pkr = ap_abstract0_widening(pk,pk0,pk1);
    ppl0 = convert(ppl,pk0);
    ppl1 = convert(ppl,pk1);
    pplr = ap_abstract0_widening(ppl,ppl0,ppl1);
    RESULT('*');
    if (is_eq(pkr,pplr)!=tbool_true) {
      ERROR("different results");
      print_poly("pk0",pk0); print_poly("pk1",pk1);
      print_poly("pkr",pkr); print_poly("pplr",pplr);
    }
    ap_abstract0_free(pk,pk0); ap_abstract0_free(pk,pk1); ap_abstract0_free(pk,pkr);
    ap_abstract0_free(ppl,ppl0); ap_abstract0_free(ppl,ppl1); ap_abstract0_free(ppl,pplr);
  } ENDLOOP;
}


/* main */
/* **** */

int main(void)
{
  int i, strict;
  srand48(time(NULL));

  for (strict=0;strict<2;strict++) {
    /* init */
    pk = pk_manager_alloc(strict);
    ppl = ap_ppl_poly_manager_alloc(strict);
    pk->option.scalar_discr = ppl->option.scalar_discr = AP_SCALAR_MPQ;
    for (i=0;i<AP_EXC_SIZE;i++){
      pk->option.abort_if_exception[i] = true;
      ppl->option.abort_if_exception[i] = true;
    }
    
    printf("\n\ncomparing libraries:\n- %s (%s)\n- %s (%s)\nwith strict=%i\n\n",
	   pk->library,pk->version,ppl->library,ppl->version,strict);
    
    /* run tests */
    test_conv();
    test_join();
    test_meet();
    test_join_array();
    test_meet_array();
    test_dimadd();
    test_dimrem();
    test_forget();
    test_permute();
    test_expand();
    if (0) test_fold(); // bug in NewPolka ?
    test_add_lincons();
    if (!strict) test_add_ray(); // buf in strict mode ?
    test_box();
    test_vbound();
    test_lbound();
    test_csat();
    test_isat();
    test_assign();
    test_par_assign();
    test_subst();
    test_par_subst();
    if (!strict) test_widen(); // behave differently in strict mode ?
    
    /* clean-up */
    ap_manager_free(pk);
    ap_manager_free(ppl);
  }

  if (error_) printf("\n%i error(s)!\n",error_);
  else printf("\nall tests passed\n");
  return 0;
}
