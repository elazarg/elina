/* ********************************************************************** */
/* pk_project.h: projections  */
/* ********************************************************************** */

#ifndef _PK_PROJECT_H_
#define _PK_PROJECT_H_

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_bit.h"
#include "pk_satmat.h"
#include "pk_matrix.h"

void _poly_projectforget_array(bool project,
			       bool lazy,
			       ap_manager_t* man,	
			       poly_t* po, const poly_t* pa, 
			       const ap_dim_t* tdim, size_t size);

poly_t* poly_forget_array(ap_manager_t* man, 
			  bool destructive, poly_t* pa, 
			  const ap_dim_t* tdim, size_t size,
			  bool project);

#endif
