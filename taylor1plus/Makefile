
include ../Makefile.config

ifneq ($(CSDP_PATH),)
CSDP_CFLAGS = -ansi -Wall -DNOSHORTS -DUSEGETTIME -I$(CSDP_PATH)/include -D_USE_SDP
CSDP_LIBS = -L/usr/lib/sse2 -L$(CSDP_PATH)/lib -lsdp -llapack -lf77blas -lcblas -latlas -lgfortran -lm
endif

PREFIX = $(APRON_PREFIX)

SRCDIR = $(shell pwd)

ifneq ($(OCAMLFIND),)
MLGMPIDL_INCLUDE = $(shell $(OCAMLFIND) query gmp)
MLGMPIDL_LIB = $(MLGMPIDL_INCLUDE)
else
MLGMPIDL_INCLUDE = $(MLGMPIDL_PREFIX)/include
MLGMPIDL_LIB = $(MLGMPIDL_PREFIX)/lib
endif

#---------------------------------------
# Programs
#---------------------------------------

#---------------------------------------
# Flags
#---------------------------------------

CFLAGS := $(CSDP_CFLAGS) $(CFLAGS)
CFLAGS_DEBUG := $(CSDP_CFLAGS) $(CFLAGS_DEBUG) -D_GET_CPU_TIME -D_T1P_DEBUG #-D_T1P_DEBUG_FUN #-pg -D_CALL_DEBUG

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = \
-I$(MLGMPIDL_INCLUDE) \
-I../num \
-I../itv \
-I../apron \
-I../mlapronidl \
-I$(GMP_PREFIX)/include -I$(MPFR_PREFIX)/include \
-I$(CAMLIDL_PREFIX)/lib/ocaml -I$(CAML_PREFIX)/lib/ocaml

# Caml
OCAMLINC = -I $(MLGMPIDL_LIB) -I ../mlapronidl

#---------------------------------------
# Files
#---------------------------------------

CCMODULES = t1p_internal t1p_representation t1p_constructor t1p_meetjoin t1p_assign t1p_resize t1p_otherops t1p_fun t1p_itv_utils
CCSRC = t1p_config.h t1p.h $(CCMODULES:%=%.h) $(CCMODULES:%=%.c)

CCINC_TO_INSTALL = t1p.h
CCBIN_TO_INSTALL =
CCLIB_TO_INSTALL = libt1pMPQ.a libt1pMPQ_debug.a 
ifneq ($(HAS_SHARED),)
CCLIB_TO_INSTALL := $(CCLIB_TO_INSTALL) \
libt1pMPQ.so libt1pMPQ_debug.so 
endif
CAML_TO_INSTALL = \
t1p.idl t1p.ml t1p.mli t1p.cmi \
t1pMPQ.cma \
libt1pMPQ_caml.a \
libt1pMPQ_caml_debug.a 
ifneq ($(HAS_OCAMLOPT),)
CAML_TO_INSTALL += \
t1pMPQ.cmxa t1pMPQ.a 
endif
ifneq ($(HAS_SHARED),)
CAML_TO_INSTALL += dllt1pMPQ_caml.so dllt1pMPQ_caml_debug.so 
endif

LIBS = -L$(APRON_PREFIX) -lapron -L$(GMP_PREFIX)/lib -lgmpxx -lgmp -L$(MPFR_PREFIX)/lib -lmpfr -lstdc++ -lm
LIBS_DEBUG = -L$(APRON_PREFIX) -lapron_debug -L$(GMP_PREFIX)/lib -lgmpxx -lgmp -L$(MPFR_PREFIX)/lib -lmpfr -lstdc++ -lm

#---------------------------------------
# Rules
#---------------------------------------

# Possible goals:
# depend doc install
# and the following one

all: allMPQ #allD allMPFR 
# testD testMPQ testMPFR testD_debug testMPQ_debug testMPFR_debug test_joinD_debug test_joinMPQ_debug test_joinMPFR_debug 


allMPQ: libt1pMPQ.a libt1pMPQ_debug.a
allD: libt1pD.a libt1pD_debug.a
allMPFR: libt1pMPFR.a libt1pMPFR_debug.a
ifneq ($(HAS_SHARED),)
allMPQ: libt1pMPQ.so libt1pMPQ_debug.so
allD: libt1pD.so libt1pD_debug.so
allMPFR: libt1pMPFR.so libt1pMPFR_debug.so
endif

exemple1%_debug: exemple1%_debug.o
	$(CC) $(CFLAGS_DEBUG) -DNUM_DOUBLE -o $@ -L. $< -lt1p$*_debug -lbox$*_debug -loct$*_debug -lapron_debug $(LIBS_DEBUG)

#test%: t1p_test_eval_texp%.o
#	$(CC) $(CFLAGS) -DNUM_DOUBLE -o $@ $(LIBS) -L. $< -lt1p$* -lbox$* -lpolkaMPQ_debug -lapron t1p_internal$*.o t1p_representation$*.o

#test%_debug: t1p_test_eval_texp%_debug.o
#	$(CC) $(CFLAGS_DEBUG) -DNUM_DOUBLE -o $@ -L. $< -lt1p$*_debug -lbox$*_debug -lpolkaMPQ_debug t1p_internal$*_debug.o $(LIBS_DEBUG)


#test_join%: t1p_test_join%.o
#	$(CC) $(CFLAGS_DEBUG) -DNUM_DOUBLE -o $@ $(LIBS) -L. $< -lt1p$* -lbox$* -lpolkaMPQ_debug -lapron_debug t1p_internal$*.o t1p_representation$*.o 

#test_join%_debug: t1p_test_join%_debug.o
#	$(CC) $(CFLAGS_DEBUG) -DNUM_DOUBLE -o $@ -L. $< -lt1p$*_debug -lbox$*_debug -lpolkaMPQ_debug t1p_internal$*_debug.o -lapron_debug $(LIBS_DEBUG)


ml: t1p.mli t1p.ml t1p.cmi mlMPQ mlD mlMPFR

mlMPQ: t1pMPQ.cma libt1pMPQ_caml.a libt1pMPQ_caml_debug.a 
mlD: t1pD.cma libt1pD_caml.a libt1pD_caml_debug.a
mlMPFR: t1pMPFR.cma libt1pMPFR_caml.a libt1pMPFR_caml_debug.a
ifneq ($(HAS_OCAMLOPT),)
mlMPQ: t1pMPQ.cmxa t1pMPQ.a
mlD: t1pD.cmxa t1pD.a
mlMPFR: t1pMPFR.cmxa t1pMPFR.a
endif
ifneq ($(HAS_SHARED),)
mlMPQ: dllt1pMPQ_caml.so dllt1pMPQ_caml_debug.so
mlD: dllt1pD_caml.so  dllt1pD_caml_debug.so 
mlMPFR: dllt1pMPFR_caml.so dllt1pMPFR_caml_debug.so
endif

mlexample%.byte: mlexample.ml t1p%.cma
	$(OCAMLC) $(OCAMLFLAGS) -I $(MLGMPIDL_LIB) -I $(APRON_PREFIX)/lib -o $@ bigarray.cma gmp.cma apron.cma t1p$*.cma $<

mlexample%.opt: mlexample.ml t1p%.cmxa
	$(OCAMLOPT) $(OCAMLOPTFLAGS) -I $(MLGMPIDL_LIB) -I $(APRON_PREFIX)/lib -o $@ bigarray.cmxa gmp.cmxa apron.cmxa t1p$*.cmxa $<

clean:
	/bin/rm -f *.[ao] *.so *.cm[ioax] *.cmxa *.byte *.opt *.annot
	/bin/rm -f *.?.tex *.log *.aux *.bbl *.blg *.toc *.dvi *.ps *.pstex*
	/bin/rm -fr t1ppolkarung t1ppolkatopg tmp
	/bin/rm -fr test*

distclean: clean
	/bin/rm -fr manager.idl
	/bin/rm -f t1p.ml t1p.mli t1p_caml.c
	/bin/rm -f Makefile.depend

install: $(CCINC_TO_INSTALL) $(CCLIB_TO_INSTALL)
	$(INSTALLd) $(PREFIX)/include $(PREFIX)/lib
	$(INSTALL) $(CCINC_TO_INSTALL) $(PREFIX)/include
	for i in $(CCLIB_TO_INSTALL); do \
		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/lib; fi; \
	done
#	for i in $(CCBIN_TO_INSTALL); do \
#		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/bin; fi; \
#	done
	for i in $(CAML_TO_INSTALL); do \
		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/lib; fi; \
	done

uninstall:
	for i in $(CCINC_TO_INSTALL); do /bin/rm -f $(PREFIX)/include/$$i; done
	/bin/rm -f $(PREFIX)/lib/libt1p*.* $(PREFIX)/lib/liboct*_debug.*
	/bin/rm -f $(PREFIX)/lib/dllt1p*.so $(PREFIX)/lib/dlloct*_debug.so
	/bin/rm -f $(PREFIX)/lib/t1p.mli $(PREFIX)/lib/t1p.ml $(PREFIX)/lib/t1p.cm[ix] $(PREFIX)/lib/t1p.idl $(PREFIX)/lib/t1p*.cma $(PREFIX)/lib/t1p*.cmxa $(PREFIX)/lib/t1p*.a


dist: $(CCSRC) Makefile sedscript_caml t1p.texi t1p.idl t1p.ml t1p.mli t1p_caml.c COPYING README
	(cd ..; tar zcvf t1p.tgz $(^:%=t1p/%))

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .tex .c .h .a .o

.PRECIOUS: libt1p%.a libt1p%_debug.a libt1p%.so libt1p%_debug.so
.PRECIOUS: libt1p%_caml.a libt1p%_caml_debug.a dllt1p%_caml.so
.PRECIOUS: %MPQ.o %D.o %MPFR.o
.PRECIOUS: %MPQ_debug.o %D_debug.o %MPFR_debug.o
.PRECIOUS: %.cmo %.cmx

#-----------------------------------
# C part
#-----------------------------------

libt1p%.a: $(subst .c,%.o,$(CCMODULES:%=%.c))
	$(AR) rcs $@ $^
	$(RANLIB) $@
libt1p%_debug.a: $(subst .c,%_debug.o,$(CCMODULES:%=%.c))
	$(AR) rcs $@ $^
	$(RANLIB) $@
libt1p%.so:  $(subst .c,%.o,$(CCMODULES:%=%.c))
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LIBS) -L../apron -L../box -lbox$* -L../newpolka -lpolka$*
libt1p%_debug.so: $(subst .c,%_debug.o,$(CCMODULES:%=%.c))
	$(CC) $(CFLAGS_DEBUG) -shared -o $@ $^ $(LIBS_DEBUG) -L../apron

%MPQ.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<
%MPQ_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<
%Rll.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_LONGLONGRAT -c -o $@ $<
%Rll_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_LONGLONGRAT -c -o $@ $<
%D.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_DOUBLE -c -o $@ $<
%D_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_DOUBLE -c -o $@ $<
%MPFR.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_MPFR -c -o $@ $<
%MPFR_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_MPFR -c -o $@ $<

#---------------------------------------
# OCaml binding
#---------------------------------------

libt1p%_caml.a: t1p_caml.o libt1p%.a
	$(AR) rcs $@ $<
	$(RANLIB) $@
libt1p%_caml_debug.a: t1p_caml_debug.o libt1p%_debug.a
	$(AR) rcs $@ $<
	$(RANLIB) $@

dllt1p%_caml.so: t1p_caml.o libt1p%.so
	$(CC) $(CFLAGS) -shared -o $@ t1p_caml.o  -L. -lt1p$* $(LIBS) -L../apron
dllt1p%_caml_debug.so: t1p_caml_debug.o libt1p%_debug.so
	$(CC) $(CFLAGS) -shared -o $@ t1p_caml_debug.o -L. -lt1p$*_debug $(LIBS_DEBUG) -L../apron

t1p_caml.o: t1p_caml.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<
t1p_caml_debug.o: t1p_caml.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<

#---------------------------------------
# OCaml rules
#---------------------------------------

# t1p%.cma: t1p.cmo libt1p%_caml.a libt1p%.a
# 	$(OCAMLMKLIB) -ocamlc "$(OCAMLC)" -verbose -o t1p$* -oc t1p$*_caml t1p.cmo $(LIBS) -L. -lt1p$*

# t1p%.cmxa t1p%.a: t1p.cmx libt1p%_caml.a libt1p%.a
# 	$(OCAMLMKLIB) -ocamlopt "$(OCAMLOPT)" -verbose -o t1p$* -oc t1p$*_caml t1p.cmx $(LIBS) -L. -lt1p$* -L$(APRON_PREFIX)/lib

# DEBUG version

t1p%.cma: t1p.cmo libt1p%_caml.a libt1p%.a
	$(OCAMLMKLIB) -ocamlc "$(OCAMLC)" -verbose -o t1p$* -oc t1p$*_caml t1p.cmo -L. -lt1p$* $(LIBS) 

t1p%.cmxa t1p%.a: t1p.cmx libt1p%_caml.a libt1p%.a
	$(OCAMLMKLIB) -ocamlopt "$(OCAMLOPT)" -verbose -o t1p$* -oc t1p$*_caml t1p.cmx -L$(APRON_PREFIX)/lib -L. -lt1p$* $(LIBS) 

#---------------------------------------
# IDL rules
#---------------------------------------

manager.idl: ../mlapronidl/manager.idl
	ln -s -f $< $@

# generates t1p.ml, t1p.mli, t1p_caml.c from t1p.idl
rebuild: manager.idl t1p.idl
	mkdir -p tmp
	cp t1p.idl tmp/t1p.idl
	$(CAMLIDL) -no-include -nocpp tmp/t1p.idl
	cp tmp/t1p_stubs.c t1p_caml.c
	$(SED) -f sedscript_caml tmp/t1p.ml >t1p.ml
	$(SED) -f sedscript_caml tmp/t1p.mli >t1p.mli

#---------------------------------------
# ML generic rules
#---------------------------------------

%.cmi: %.mli
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmo: %.ml %.cmi
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmx: %.ml %.cmi
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -c $<

#-----------------------------------
# DEPENDENCIES
#-----------------------------------
