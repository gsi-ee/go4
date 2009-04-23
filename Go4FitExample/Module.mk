FITEX_NAME    = Go4FitExample
MODULE_NAME   = $(FITEX_NAME)

ifdef GO4PACKAGE
FITEX_DIR     = $(GO4SYS)/$(FITEX_NAME)
LIBGO4FIT     = $(GO4PATH) -lGo4Fit
FITEX_EXEDEP  = $(GO4FIT_LIB)
else
FITEX_DIR     = .
LIBGO4FIT     = $(GO4PATH) -lGo4Fit
FITEX_EXEDEP  =
endif

ifdef GO4_WIN32
FITEX_USEDLIBS = $(ROOTLIBS) libGo4Fit.lib
else
FITEX_USEDLIBS = $(ROOTLIBS) -lMinuit $(if $(USESPECTRUM),-lSpectrum,) $(LIBGO4FIT)
endif

# ------------------Example1------------------------------------------
 
FITEX1_O      = $(FITEX_DIR)/Example1.$(ObjSuf)
FITEX1_S      = $(FITEX_DIR)/Example1.$(SrcSuf)
FITEX1_E      = $(FITEX_DIR)/Example1$(ExeSuf)

$(FITEX1_E):      $(FITEX_EXEDEP) $(FITEX1_O)
	$(LD) $(LDFLAGS) $(FITEX1_O) $(FITEX_USEDLIBS) $(OutPutOpt) $(FITEX1_E)
#	$(LD) $(LDFLAGS) $(EXAMPMESH_EXEO) $(LIBS_FULLSET) $(EXAMPMESH_LIB) $(OutPutOpt) $(EXAMPMESH_EXE)

# ------------------Example2------------------------------------------

ifndef GO4_WIN32

FITEX2_LIBNAME = Example2Func
FITEX2_O      = $(FITEX_DIR)/Example2.$(ObjSuf)
FITEX2_S      = $(FITEX_DIR)/Example2.$(SrcSuf)
FITEX2_FS     = $(FITEX_DIR)/$(FITEX2_LIBNAME).$(SrcSuf)
FITEX2_FO     = $(FITEX_DIR)/$(FITEX2_LIBNAME).$(ObjSuf)
FITEX2_LIB    = $(FITEX_DIR)/$(FITEX2_LIBNAME).$(DllSuf)
FITEX2_E      = $(FITEX_DIR)/Example2$(ExeSuf)

$(FITEX2_LIB):  $(FITEX2_FO) 
#	$(LD) $(SOFLAGS)$(FITEX2_LIB) $(LDFLAGS) $(FITEX2_FO) $(OutPutOpt) $(FITEX2_LIB)
	@$(MakeLibrary) $(FITEX2_LIBNAME) "$(FITEX2_FO)" $(FITEX_DIR)

$(FITEX2_E):    $(FITEX_EXEDEP) $(FITEX2_O) $(FITEX2_LIB)
	$(LD) $(LDFLAGS) $(FITEX2_O) $(FITEX_USEDLIBS) $(OutPutOpt) $(FITEX2_E)

endif

# -----------------Example3-----------------------------------------------

ifneq ($(shell which f77 2>/dev/null),)
ifneq ($(wildcard /usr/lib/libg2c.*)$(wildcard /usr/local/lib/libg2c.*),)

FITEX3_FUNCS   =  $(FITEX_DIR)/Example3Func.f
FITEX3_FUNCO   =  $(FITEX_DIR)/Example3Func.$(ObjSuf)

FITEX3_O      = $(FITEX_DIR)/Example3.$(ObjSuf)
FITEX3_S      = $(FITEX_DIR)/Example3.$(SrcSuf)
FITEX3_E      = $(FITEX_DIR)/Example3$(ExeSuf)

$(FITEX3_FUNCO):  $(FITEX3_FUNCS)
	f77 -c $(FITEX3_FUNCS) -o $(FITEX3_FUNCO)

$(FITEX3_E):   $(FITEX_EXEDEP) $(FITEX3_O) $(FITEX3_FUNCO)
	$(LD) $(LDFLAGS) $(FITEX3_O) $(FITEX3_FUNCO) $(FITEX_USEDLIBS) -lg2c $(OutPutOpt) $(FITEX3_E)

endif
endif

# ------------------Example4---------------------------------------------

FITEX4_LIBNAME =  libExample4
FITEX4_OBJ     =  $(FITEX_DIR)/TModelTemplate.$(ObjSuf)
FITEX4_DICTO   =  $(FITEX_DIR)/Example4Dict.$(ObjSuf)
FITEX4_SRC     =  $(FITEX_DIR)/TModelTemplate.$(SrcSuf)
FITEX4_DICTS   =  $(FITEX_DIR)/Example4Dict.$(SrcSuf)
FITEX4_HED     =  $(FITEX_DIR)/TModelTemplate.$(HedSuf)
FITEX4_LINKDEF =  $(FITEX_DIR)/Example4LinkDef.h
FITEX4_LIB     =  $(FITEX_DIR)/$(FITEX4_LIBNAME).$(DllSuf)

FITEX4_O      = $(FITEX_DIR)/Example4.$(ObjSuf)
FITEX4_S      = $(FITEX_DIR)/Example4.$(SrcSuf)
FITEX4_E      = $(FITEX_DIR)/Example4$(ExeSuf)

$(FITEX4_DICTS): $(FITEX4_HED)
	@rootcint -f $(FITEX4_DICTS) -c $(FITEX4_HED) $(FITEX4_LINKDEF)
    
$(FITEX4_LIB):  $(FITEX4_OBJ) $(FITEX4_DICTO)
	@$(MakeLibrary) $(FITEX4_LIBNAME) "$(FITEX4_OBJ) $(FITEX4_DICTO)" $(FITEX_DIR) $(FITEX4_LINKDEF) "$(GO4FIT_LIB) $(ROOTSYS)/lib/libMinuit.$(DllSuf)"

ifdef GO4_WIN32
FITEX4_LIBSSET = $(FITEX4_LIB)
else
FITEX4_LIBSSET = -L$(FITEX_DIR) -lExample4
endif


$(FITEX4_E):     $(FITEX_EXEDEP) $(FITEX4_O) $(FITEX4_LIB)
	$(LD) $(LDFLAGS) $(FITEX4_O) $(FITEX_USEDLIBS) $(FITEX4_LIBSSET) $(OutPutOpt) $(FITEX4_E)

# ------------------Example5------------------------------------------

FITEX5_O      = $(FITEX_DIR)/Example5.$(ObjSuf)
FITEX5_S      = $(FITEX_DIR)/Example5.$(SrcSuf)
FITEX5_E      = $(FITEX_DIR)/Example5$(ExeSuf)

$(FITEX5_E):     $(FITEX_EXEDEP) $(FITEX5_O)
	$(LD) $(LDFLAGS) $(FITEX5_O) $(FITEX_USEDLIBS) $(OutPutOpt) $(FITEX5_E)

# ------------------Example6------------------------------------------

FITEX6_O      = $(FITEX_DIR)/Example6.$(ObjSuf)
FITEX6_S      = $(FITEX_DIR)/Example6.$(SrcSuf)
FITEX6_E      = $(FITEX_DIR)/Example6$(ExeSuf)

$(FITEX6_E):     $(FITEX_EXEDEP) $(FITEX6_O)
	$(LD) $(LDFLAGS) $(FITEX6_O) $(FITEX_USEDLIBS) $(OutPutOpt) $(FITEX6_E)

# ------------------Example7------------------------------------------

FITEX7_O      = $(FITEX_DIR)/Example7.$(ObjSuf)
FITEX7_S      = $(FITEX_DIR)/Example7.$(SrcSuf)
FITEX7_E      = $(FITEX_DIR)/Example7$(ExeSuf)

$(FITEX7_E):     $(FITEX_EXEDEP) $(FITEX7_O)
	$(LD) $(LDFLAGS) $(FITEX7_O) $(FITEX_USEDLIBS) $(OutPutOpt) $(FITEX7_E)

# ------------------Example8------------------------------------------

FITEX8_O      = $(FITEX_DIR)/Example8.$(ObjSuf)
FITEX8_S      = $(FITEX_DIR)/Example8.$(SrcSuf)
FITEX8_E      = $(FITEX_DIR)/Example8$(ExeSuf)

$(FITEX8_E):      $(FITEX_EXEDEP) $(FITEX8_O)
	$(LD) $(LDFLAGS) $(FITEX8_O) $(FITEX_USEDLIBS) $(OutPutOpt) $(FITEX8_E)

# ------------------Example9------------------------------------------

FITEX9_O      = $(FITEX_DIR)/Example9.$(ObjSuf)
FITEX9_S      = $(FITEX_DIR)/Example9.$(SrcSuf)
FITEX9_E      = $(FITEX_DIR)/Example9$(ExeSuf)

$(FITEX9_E):      $(FITEX_EXEDEP) $(FITEX9_O)
	$(LD) $(LDFLAGS) $(FITEX9_O) $(FITEX_USEDLIBS) $(OutPutOpt) $(FITEX9_E)

# ------------------Example10------------------------------------------

FITEX10_O      = $(FITEX_DIR)/Example10.$(ObjSuf)
FITEX10_S      = $(FITEX_DIR)/Example10.$(SrcSuf)
FITEX10_E      = $(FITEX_DIR)/Example10$(ExeSuf)

$(FITEX10_E):      $(FITEX_EXEDEP) $(FITEX10_O)
	$(LD) $(LDFLAGS) $(FITEX10_O) $(FITEX_USEDLIBS) $(OutPutOpt) $(FITEX10_E)

# ------------------Example11------------------------------------------

FITEX11_O      = $(FITEX_DIR)/Example11.$(ObjSuf)
FITEX11_S      = $(FITEX_DIR)/Example11.$(SrcSuf)
FITEX11_E      = $(FITEX_DIR)/Example11$(ExeSuf)

$(FITEX11_E):      $(FITEX_EXEDEP) $(FITEX11_O)
	$(LD) $(LDFLAGS) $(FITEX11_O) $(FITEX_USEDLIBS) $(OutPutOpt) $(FITEX11_E)

# ------------------Example12------------------------------------------

FITEX12_O      = $(FITEX_DIR)/Example12.$(ObjSuf)
FITEX12_S      = $(FITEX_DIR)/Example12.$(SrcSuf)
FITEX12_E      = $(FITEX_DIR)/Example12$(ExeSuf)

$(FITEX12_E):      $(FITEX_EXEDEP) $(FITEX12_O)
	$(LD) $(LDFLAGS) $(FITEX12_O) $(FITEX_USEDLIBS) $(OutPutOpt) $(FITEX12_E)

# ------------------Example13------------------------------------------

FITEX13_O      = $(FITEX_DIR)/Example13.$(ObjSuf)
FITEX13_S      = $(FITEX_DIR)/Example13.$(SrcSuf)
FITEX13_E      = $(FITEX_DIR)/Example13$(ExeSuf)

$(FITEX13_E):      $(FITEX_EXEDEP) $(FITEX13_O)
	$(LD) $(LDFLAGS) $(FITEX13_O) $(FITEX_USEDLIBS) $(OutPutOpt) $(FITEX13_E)

#------------------------------------------------------------------------------

# used in the main Makefile

FITEX_DLLS         = $(FITEX2_LIB) $(FITEX4_LIB)

FITEX_EXECS        = $(FITEX1_E) $(FITEX2_E) $(FITEX3_E) $(FITEX4_E) $(FITEX5_E) $(FITEX6_E) $(FITEX7_E) \
                     $(FITEX8_E) $(FITEX9_E) $(FITEX10_E) $(FITEX11_E) $(FITEX12_E) $(FITEX13_E)

FITEX_OBJS         = $(FITEX1_O) $(FITEX2_O) $(FITEX2_FO) \
                     $(FITEX3_O) $(FITEX3_FUNCO) \
                     $(FITEX4_O) $(FITEX4_OBJ) $(FITEX4_DICTO) \
                     $(FITEX5_O) $(FITEX6_O) $(FITEX7_O) \
                     $(FITEX8_O) $(FITEX9_O) $(FITEX10_O) $(FITEX11_O) $(FITEX12_O) $(FITEX13_O)

ifdef DOPACKAGE
FITEXDISTRFILES    += $(FITEX1_S) $(FITEX2_S) $(FITEX3_S) $(FITEX4_S) $(FITEX5_S) $(FITEX6_S) $(FITEX7_S)
FITEXDISTRFILES    += $(FITEX8_S) $(FITEX9_S) $(FITEX10_S) $(FITEX11_S) $(FITEX12_S) $(FITEX13_S)
FITEXDISTRFILES    += $(FITEX2_FS) $(FITEX3_FUNCS)
FITEXDISTRFILES    += $(FITEX4_LINKDEF) $(FITEX4_SRC) $(FITEX4_HED)
FITEXDISTRFILES    += $(FITEX_DIR)/histograms.root $(FITEX_DIR)/README
DISTRFILES         += $(FITEXDISTRFILES) $(FITEX_DIR)/Makefile.standalone
endif

all-$(FITEX_NAME):  $(FITEX_DLLS) $(FITEX_EXECS)

clean-obj-$(FITEX_NAME):
	@rm -f $(FITEX_EXECS) $(FITEX_DLLS) $(FITEX_OBJS) $(FITEX_DIR)/*.$(DllSuf) $(FITEX_DIR)/*.$(ObjSuf)
	@$(CleanLib) $(FITEX2_LIBNAME) $(FITEX_DIR)
	@$(CleanLib) $(FITEX4_LIBNAME) $(FITEX_DIR)

clean-$(FITEX_NAME): clean-obj-$(FITEX_NAME)
	@rm -f $(FITEX_DIR)/*.bak $(FITEX_DIR)/*Dict.* $(FITEX_DIR)/Example*.root 

