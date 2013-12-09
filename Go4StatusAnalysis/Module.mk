STATANAL_NAME        = Go4StatusAnalysis

## normally should be like this for every module, but can be specific

STATANAL_DIR         = $(STATANAL_NAME)
STATANAL_LINKDEF     = $(STATANAL_DIR)/$(STATANAL_NAME)LinkDef.$(HedSuf)

STATANAL_NOTLIBF     =

## must be similar for every module

STATANAL_DICT        = $(STATANAL_DIR)/$(DICT_PREFIX)$(STATANAL_NAME)
STATANAL_DH          = $(STATANAL_DICT).$(HedSuf)
STATANAL_DS          = $(STATANAL_DICT).$(SrcSuf)
STATANAL_DO          = $(STATANAL_DICT).$(ObjSuf)
STATANAL_D6          = $(STATANAL_DICT)$(DICT_R6SUFF)

STATANAL_H           = $(filter-out $(STATANAL_NOTLIBF) $(STATANAL_DH) $(STATANAL_LINKDEF), $(wildcard $(STATANAL_DIR)/*.$(HedSuf)))
STATANAL_S           = $(filter-out $(STATANAL_NOTLIBF) $(STATANAL_DS), $(wildcard $(STATANAL_DIR)/*.$(SrcSuf)))
STATANAL_O           = $(STATANAL_S:.$(SrcSuf)=.$(ObjSuf))

STATANAL_DEP         =  $(STATANAL_O:.$(ObjSuf)=.$(DepSuf))
STATANAL_DDEP        =  $(STATANAL_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(STATANAL_DIR)/%.h, include/%.h, $(STATANAL_H))

LIBDEPENDENC       += $(STATANAL_DEP) $(STATANAL_DDEP)

ifdef DOPACKAGE
DISTRFILES         += $(STATANAL_S) $(STATANAL_H) $(STATANAL_LINKDEF)
endif

##### local rules #####

include/%.h: $(STATANAL_DIR)/%.h
	@echo "Copy header $@ ..."
	@cp -f $< $@

$(STATANAL_DS): $(STATANAL_H)  $(STATANAL_LINKDEF)
	@$(ROOTCINTGO4) $(STATANAL_H) $(STATANAL_LINKDEF)

clean-bin::
	@rm -f $(STATANAL_O) $(STATANAL_DO) $(STATANAL_D6)
	@rm -f $(STATANAL_DEP) $(STATANAL_DDEP) $(STATANAL_DS) $(STATANAL_DH)

