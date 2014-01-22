GO4FIT_NAME        = Go4Fit

## normally should be like this for every module, but can be specific

GO4FIT_DIR         = $(GO4FIT_NAME)
GO4FIT_LINKDEF     = $(GO4FIT_DIR)/$(GO4FIT_NAME)LinkDef.$(HedSuf)

GO4FIT_LIBNAME  = $(LIB_PREFIX)Go4Fit
GO4FIT_LIB      = $(GO4DLLPATH)/$(GO4FIT_LIBNAME).$(DllSuf)

GO4FIT_NOTLIBF     =

## must be similar for every module

GO4FIT_DICT        = $(GO4FIT_DIR)/$(DICT_PREFIX)$(GO4FIT_NAME)
GO4FIT_DH          = $(GO4FIT_DICT).$(HedSuf)
GO4FIT_DS          = $(GO4FIT_DICT).$(SrcSuf)
GO4FIT_DO          = $(GO4FIT_DICT).$(ObjSuf)

GO4FIT_H           = $(filter-out $(GO4FIT_NOTLIBF) $(GO4FIT_DH) $(GO4FIT_LINKDEF), $(wildcard $(GO4FIT_DIR)/*.$(HedSuf)))
GO4FIT_S           = $(filter-out $(GO4FIT_NOTLIBF) $(GO4FIT_DS), $(wildcard $(GO4FIT_DIR)/*.$(SrcSuf)))
GO4FIT_O           = $(GO4FIT_S:.$(SrcSuf)=.$(ObjSuf))

GO4FIT_DEP         =  $(GO4FIT_O:.$(ObjSuf)=.$(DepSuf))
GO4FIT_DDEP        =  $(GO4FIT_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(GO4FIT_DIR)/%.h, include/%.h, $(GO4FIT_H))

LIBDEPENDENC       += $(GO4FIT_DEP) $(GO4FIT_DDEP)

ifdef DOPACKAGE
FITDISTRFILES      += $(GO4FIT_S) $(GO4FIT_H) $(GO4FIT_LINKDEF) $(GO4FIT_DIR)/f_find_peaks.c
FITDISTRFILES      += $(GO4FIT_DIR)/README $(GO4FIT_DIR)/changes.txt
DISTRFILES         += $(FITDISTRFILES)
endif

##### local rules #####

include/%.h: $(GO4FIT_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@


GO4FIT_DEPLIB = $(BASIC_LIB_DEP)

$(GO4FIT_LIB):   MAKELIB_SET = $(LIBS_BASESET)

$(GO4FIT_DS): $(GO4FIT_H)  $(GO4FIT_LINKDEF)
	@$(ROOTCINTGO4) $(GO4FIT_LIB) $(GO4FIT_H) $(GO4FIT_LINKDEF)

$(GO4FIT_LIB):   $(GO4FIT_O) $(GO4FIT_DO) $(GO4FIT_DEPLIB)
	@$(MakeLibrary) $(GO4FIT_LIBNAME) "$(GO4FIT_O) $(GO4FIT_DO)" $(GO4DLLPATH) $(GO4FIT_LINKDEF) "$(GO4FIT_DEPLIB)" $(GO4FIT_DS) "$(GO4FIT_H)"


clean-bin::
	@rm -f $(GO4FIT_O) $(GO4FIT_DO)
	@rm -f $(GO4FIT_DEP) $(GO4FIT_DDEP) $(GO4FIT_DS) $(GO4FIT_DH)
