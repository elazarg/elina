/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _AP_LINEARIZE_H_
#define _AP_LINEARIZE_H_

#include "ap_manager.h"
#include "ap_expr0.h"
#include "ap_abstract0.h"

#ifdef __cplusplus
extern "C" {
#endif


  /* evaluate to interval */
ap_interval_t* ap_linexpr0_eval(ap_manager_t* man,
				ap_abstract0_t* abs,
				ap_linexpr0_t* expr, 
				ap_scalar_discr_t discr,
				bool* pexact);


/* These functions are dedicated to implementors of domains. They offer generic
   default implementations for some of the operations required by the APRON
   API, when there is no more specific and efficient implementation for the
   domain being implemented.

   To use them, the function allocating manager, which is specific to the domain,
   should put the corresponding pointers in the virtual table to them.

   They manipulated "unboxed" abstract values, which are native to the
   underlying library: they are not yet boxed with the manager in the type
   ap_abstract0_t.
*/

/* The following functions use the given abstract value for transforming
   interval linear expressions (resp. constraints, arrays of expressions,
   arrays od constraints) in quasilinear corresponding objects.

   They use to_box and dimension (and is_bottom if NDEBUG is undefined) generic
   functions.

   - discr allows to choose the type of scalars used for computations and for
     the result.

   - pexact is a pointer to a Boolean, which is set to true if all the
     conversions and computations were exact.

   Calling ap_linearize_linexpr0_array is more efficient than calling N times
   ap_linearize_linexpr0 because the conversion of abstrat value to bounding
   boxes is done only once, as well as other internal allocations.
*/

ap_linexpr0_t* 
ap_quasilinearize_linexpr0(ap_manager_t* man,
			   void* abs,
			   ap_linexpr0_t* linexpr0,
			   ap_scalar_discr_t discr,
			   bool* pexact);

ap_lincons0_t 
ap_quasilinearize_lincons0(ap_manager_t* man,
			   void* abs,
			   ap_lincons0_t* lincons0,
			   ap_scalar_discr_t discr,
			   bool* pexact);

ap_linexpr0_t** 
ap_quasilinearize_linexpr0_array(ap_manager_t* man,
				 void* abs,
				 ap_linexpr0_t** texpr, size_t size,
				 ap_scalar_discr_t discr,
				 bool* pexact);

ap_lincons0_array_t 
ap_quasilinearize_lincons0_array(ap_manager_t* man,
				 void* abs,
				 ap_lincons0_array_t* array,
				 ap_scalar_discr_t discr,
				 bool* pexact,
				 bool convert,
				 bool linearize);
  
  
/* For internal use by ap_linearize_aux */
void ap_linearize_quasilincons0(ap_lincons0_array_t* array, size_t* pindex, 
				ap_lincons0_t* cons);



#ifdef __cplusplus
}
#endif

#endif
