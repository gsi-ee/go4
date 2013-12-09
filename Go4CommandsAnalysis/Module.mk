CMDANAL_NAME        = Go4CommandsAnalysis

## normally should be like this for every module, but can be specific

CMDANAL_DIR         = $(CMDANAL_NAME)
CMDANAL_LINKDEF     = $(CMDANAL_DIR)/$(CMDANAL_NAME)LinkDef.$(HedSuf)

CMDANAL_NOTLIBF     = $(CMDANAL_DIR)/Go4CommandsAnalysis.h

## must be similar for every module

CMDANAL_DICT        = $(CMDANAL_DIR)/$(DICT_PREFIX)$(CMDANAL_NAME)
CMDANAL_DH          = $(CMDANAL_DICT).$(HedSuf)
CMDANAL_DS          = $(CMDANAL_DICT).$(SrcSuf)
CMDANAL_DO          = $(CMDANAL_DICT).$(ObjSuf)
CMDANAL_D6          = $(CMDANAL_DICT)$(DICT_R6SUFF)

CMDANAL_H           = $(filter-out $(CMDANAL_NOTLIBF) $(CMDANAL_DH) $(CMDANAL_LINKDEF), $(wildcard $(CMDANAL_DIR)/*.$(HedSuf)))
CMDANAL_S           = $(filter-out $(CMDANAL_NOTLIBF) $(CMDANAL_DS), $(wildcard $(CMDANAL_DIR)/*.$(SrcSuf)))
CMDANAL_O           = $(CMDANAL_S:.$(SrcSuf)=.$(ObjSuf))

CMDANAL_DEP         =  $(CMDANAL_O:.$(ObjSuf)=.$(DepSuf))
CMDANAL_DDEP        =  $(CMDANAL_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(CMDANAL_DIR)/%.h, include/%.h, $(CMDANAL_DIR)/TGo4AnalysisCommandList.h)

LIBDEPENDENC       += $(CMDANAL_DEP) $(CMDANAL_DDEP)

ifdef DOPACKAGE
DISTRFILES         += $(CMDANAL_S) $(CMDANAL_H) $(CMDANAL_LINKDEF) $(CMDANAL_NOTLIBF)
endif

##### local rules #####

include/%.h: $(CMDANAL_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

$(CMDANAL_DS): $(CMDANAL_H) $(CMDANAL_LINKDEF)
	@$(ROOTCINTGO4) $(CMDANAL_H) $(CMDANAL_LINKDEF)

clean-bin::
	@rm -f $(CMDANAL_O) $(CMDANAL_DO) $(CMDANAL_D6)
	@rm -f $(CMDANAL_DEP) $(CMDANAL_DDEP) $(CMDANAL_DS) $(CMDANAL_DH)

