TASCA_NAME        = Comp
MODULE_NAME            = $(TASCA_NAME)

## normally should be like this for every module, but can be specific

ifdef GO4PACKAGE
TASCA_DIR         = $(GO4SYS)/$(TASCA_NAME)
else
TASCA_DIR         = .
endif

TASCA_LINKDEF     = $(TASCA_DIR)/Go4UserAnalysisLinkDef.$(HedSuf)
TASCA_LIBNAME     = libGo4UserAnalysis
TASCA_EXENAME     = MainUserAnalysis
TASCA_ONLYMAP     = $(TASCA_DIR)/.localmap
TASCA_MAP         = $(TASCA_DIR)/$(ROOTMAPNAME)

TASCA_NOTLIBF     = 

## must be similar for every module

TASCA_EXEO        = $(TASCA_DIR)/$(TASCA_EXENAME).$(ObjSuf)
#TASCA_EXEH        = $(TASCA_DIR)/$(TASCA_EXENAME).$(HedSuf)
TASCA_EXES        = $(TASCA_DIR)/$(TASCA_EXENAME).$(SrcSuf)
TASCA_EXE         = $(TASCA_DIR)/$(TASCA_EXENAME)$(ExeSuf)   

TASCA_DICT        = $(TASCA_DIR)/$(DICT_PREFIX)$(TASCA_NAME)
TASCA_DH          = $(TASCA_DICT).$(HedSuf)
TASCA_DS          = $(TASCA_DICT).$(SrcSuf)
TASCA_DO          = $(TASCA_DICT).$(ObjSuf)

TASCA_H           = $(filter-out $(TASCA_EXEH) $(TASCA_NOTLIBF) $(TASCA_DH) $(TASCA_LINKDEF), $(wildcard $(TASCA_DIR)/*.$(HedSuf)))
TASCA_S           = $(filter-out $(TASCA_EXES) $(TASCA_NOTLIBF) $(TASCA_DS), $(wildcard $(TASCA_DIR)/*.$(SrcSuf)))
TASCA_O           = $(TASCA_S:.$(SrcSuf)=.$(ObjSuf))

TASCA_DEP         =  $(TASCA_O:.$(ObjSuf)=.$(DepSuf))
TASCA_DDEP        =  $(TASCA_DO:.$(ObjSuf)=.$(DepSuf))
TASCA_EDEP        =  $(TASCA_EXEO:.$(ObjSuf)=.$(DepSuf))

TASCA_SLIB        =  $(TASCA_DIR)/$(TASCA_LIBNAME).$(DllSuf)
TASCA_LIB         =  $(TASCA_DIR)/$(TASCA_LIBNAME).$(DllSuf).$(VERSSUF)

# used in the main Makefile

EXAMPDEPENDENCS    += $(TASCA_DEP) $(TASCA_DDEP) $(TASCA_EDEP)

ifdef DOPACKAGE
DISTRFILES         += $(TASCA_S) $(TASCA_H) $(TASCA_LINKDEF) $(TASCA_EXEH) $(TASCA_EXES)
DISTRFILES         += $(TASCA_DIR)/Readme.txt $(TASCA_DIR)/Makefile.win
DISTRFILES         += $(TASCA_DIR)/AnalysisStart.sh  $(TASCA_DIR)/rename.sh 
DISTRFILES         += $(TASCA_DIR)/calilines.txt
DISTRFILES         += $(TASCA_DIR)/gaussfiles.lml
DISTRFILES         += $(TASCA_DIR)/convertfile.C
DISTRFILES         += $(TASCA_DIR)/findobjects.C
DISTRFILES         += $(TASCA_DIR)/browse.C
DISTRFILES         += $(TASCA_DIR)/plothistos.C
#DISTRFILES         += $(wildcard $(TASCA_DIR)/*.C)
endif

##### local rules #####

$(TASCA_EXE):      $(BUILDGO4LIBS) $(TASCA_EXEO) $(TASCA_LIB)
	$(LD) $(LDFLAGS) $(TASCA_EXEO) $(LIBS_FULLSET) $(TASCA_LIB) $(OutPutOpt) $(TASCA_EXE)
	@echo "$@  done"

$(TASCA_LIB):   $(TASCA_O) $(TASCA_DO)
	$(MakeLib) $(TASCA_LIBNAME) "$(TASCA_O) $(TASCA_DO)" $(TASCA_DIR)
	echo "Making lib"

$(TASCA_DS): $(TASCA_H)  $(TASCA_LINKDEF)
		@$(ROOTCINTGO4) $(TASCA_H) $(TASCA_LINKDEF)

$(TASCA_ONLYMAP): $(TASCA_LINKDEF) $(TASCA_LIB)
	@rm -f $(TASCA_ONLYMAP)
	@$(MakeMap) $(TASCA_ONLYMAP) $(TASCA_SLIB) $(TASCA_LINKDEF) "$(ANAL_LIB_DEP)"

all-$(TASCA_NAME):     $(TASCA_LIB) $(TASCA_EXE) map-$(TASCA_NAME)
	echo "Making all"

clean-obj-$(TASCA_NAME):
	@rm -f $(TASCA_O) $(TASCA_DO)
	@$(CleanLib) $(TASCA_LIBNAME) $(TASCA_DIR)
	@rm -f $(TASCA_EXEO) $(TASCA_EXE)

clean-$(TASCA_NAME): clean-obj-$(TASCA_NAME)
	@rm -f $(TASCA_DEP) $(TASCA_DDEP) $(TASCA_DS) $(TASCA_DH)
	@rm -f $(TASCA_EDEP)
	@rm -f $(TASCA_ONLYMAP) $(TASCA_MAP)

ifdef DOMAP
map-$(TASCA_NAME): $(GO4MAP) $(TASCA_ONLYMAP)
	@rm -f $(TASCA_MAP)
	@cat $(GO4MAP) $(TASCA_ONLYMAP) > $(TASCA_MAP)
else
map-$(TASCA_NAME):

endif
