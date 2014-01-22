THRDMNGR_NAME        = Go4ThreadManager

## normally should be like this for every module, but can be specific

THRDMNGR_DIR         = $(THRDMNGR_NAME)
THRDMNGR_LINKDEF     = $(THRDMNGR_DIR)/$(THRDMNGR_NAME)LinkDef.$(HedSuf)

THRDMNGR_LIBNAME     = $(LIB_PREFIX)Go4ThreadManager
THRDMNGR_LIB         = $(GO4DLLPATH)/$(THRDMNGR_LIBNAME).$(DllSuf)

THRDMNGR_NOTLIBF     = $(THRDMNGR_DIR)/Go4ThreadManagerExceptions.h

## must be similar for every module

THRDMNGR_DICT        = $(THRDMNGR_DIR)/$(DICT_PREFIX)$(THRDMNGR_NAME)
THRDMNGR_DH          = $(THRDMNGR_DICT).$(HedSuf)
THRDMNGR_DS          = $(THRDMNGR_DICT).$(SrcSuf)
THRDMNGR_DO          = $(THRDMNGR_DICT).$(ObjSuf)
THRDMNGR_D6          = $(THRDMNGR_DICT)$(DICT_R6SUFF)

THRDMNGR_H           = $(filter-out $(THRDMNGR_NOTLIBF) $(THRDMNGR_DH) $(THRDMNGR_LINKDEF), $(wildcard $(THRDMNGR_DIR)/*.$(HedSuf)))
THRDMNGR_S           = $(filter-out $(THRDMNGR_NOTLIBF) $(THRDMNGR_DS), $(wildcard $(THRDMNGR_DIR)/*.$(SrcSuf)))
THRDMNGR_O           = $(THRDMNGR_S:.$(SrcSuf)=.$(ObjSuf))

THRDMNGR_DEP         =  $(THRDMNGR_O:.$(ObjSuf)=.$(DepSuf))
THRDMNGR_DDEP        =  $(THRDMNGR_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(THRDMNGR_DIR)/%.h, include/%.h, $(THRDMNGR_H))

LIBDEPENDENC       += $(THRDMNGR_DEP) $(THRDMNGR_DDEP)

ifdef DOPACKAGE
THRDMNGR_DISTRFILES = $(THRDMNGR_S) $(THRDMNGR_H) $(THRDMNGR_LINKDEF) $(THRDMNGR_NOTLIBF)
DISTRFILES         += $(THRDMNGR_DISTRFILES)
endif

##### local rules #####

include/%.h: $(THRDMNGR_DIR)/%.h
	@echo "Copy header $@ ..."
	@cp -f $< $@

$(THRDMNGR_DS): $(THRDMNGR_H)  $(THRDMNGR_LINKDEF)
	@$(ROOTCINTGO4) $(THRDMNGR_LIB) $(THRDMNGR_H) $(THRDMNGR_LINKDEF)

$(THRDMNGR_LIB):   $(THRDMNGR_O) $(THRDMNGR_DO)
	@$(MakeLibrary) $(THRDMNGR_LIBNAME) "$(THRDMNGR_O) $(THRDMNGR_DO)" $(GO4DLLPATH) $(THRDMNGR_LINKDEF) "$(GO4BASE_LIB) $(BASIC_LIB_DEP)"


clean-bin::
	@rm -f $(THRDMNGR_O) $(THRDMNGR_DO) $(THRDMNGR_D6)
	@rm -f $(THRDMNGR_DEP) $(THRDMNGR_DDEP) $(THRDMNGR_DS) $(THRDMNGR_DH)

