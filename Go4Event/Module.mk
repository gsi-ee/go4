GO4EVENT_NAME       = Go4Event
GO4EVENTPAR_NAME    = Go4EventPar

## normally should be like this for every module, but can be specific

GO4EVENT_DIR         = $(GO4EVENT_NAME)
GO4EVENT_LINKDEF     = $(GO4EVENT_DIR)/$(GO4EVENT_NAME)LinkDef.$(HedSuf)
GO4EVENTPAR_LINKDEF  = $(GO4EVENT_DIR)/$(GO4EVENTPAR_NAME)LinkDef.$(HedSuf)

GO4EVENT_NOTLIBF     =

## must be similar for every module

GO4EVENT_DICT        = $(GO4EVENT_DIR)/$(DICT_PREFIX)$(GO4EVENT_NAME)
GO4EVENT_DH          = $(GO4EVENT_DICT).$(HedSuf)
GO4EVENT_DS          = $(GO4EVENT_DICT).$(SrcSuf)
GO4EVENT_DO          = $(GO4EVENT_DICT).$(ObjSuf)

GO4EVENTPAR_DICT     = $(GO4EVENT_DIR)/$(DICT_PREFIX)$(GO4EVENTPAR_NAME)
GO4EVENTPAR_DH       = $(GO4EVENTPAR_DICT).$(HedSuf)
GO4EVENTPAR_DS       = $(GO4EVENTPAR_DICT).$(SrcSuf)
GO4EVENTPAR_DO       = $(GO4EVENTPAR_DICT).$(ObjSuf)

GO4EVENTPAR_H        = $(wildcard $(GO4EVENT_DIR)/*Parameter.$(HedSuf))
GO4EVENTPAR_S        = $(wildcard $(GO4EVENT_DIR)/*Parameter.$(SrcSuf))
GO4EVENTPAR_O        = $(GO4EVENTPAR_S:.$(SrcSuf)=.$(ObjSuf))

GO4EVENT_H           = $(filter-out $(GO4EVENT_NOTLIBF) $(GO4EVENT_DH) $(GO4EVENT_LINKDEF) $(GO4EVENT_OTHERF) $(GO4EVENTPAR_H) $(GO4EVENTPAR_DH) $(GO4EVENTPAR_LINKDEF), $(wildcard $(GO4EVENT_DIR)/*.$(HedSuf)))
GO4EVENT_S           = $(filter-out $(GO4EVENT_NOTLIBF) $(GO4EVENT_DS) $(GO4EVENTPAR_S) $(GO4EVENTPAR_DS), $(wildcard $(GO4EVENT_DIR)/*.$(SrcSuf)))
GO4EVENT_O           = $(GO4EVENT_S:.$(SrcSuf)=.$(ObjSuf))

GO4EVENT_DEP         =  $(GO4EVENT_O:.$(ObjSuf)=.$(DepSuf))
GO4EVENT_DDEP        =  $(GO4EVENT_DO:.$(ObjSuf)=.$(DepSuf))

GO4EVENTPAR_DEP      =  $(GO4EVENTPAR_O:.$(ObjSuf)=.$(DepSuf))
GO4EVENTPAR_DDEP     =  $(GO4EVENTPAR_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(GO4EVENT_DIR)/%.h, include/%.h, $(GO4EVENT_H))
ALLHDRS +=  $(patsubst $(GO4EVENT_DIR)/%.h, include/%.h, $(GO4EVENTPAR_H))

LIBDEPENDENC       += $(GO4EVENT_DEP) $(GO4EVENT_DDEP)
LIBDEPENDENC       += $(GO4EVENTPAR_DEP) $(GO4EVENTPAR_DDEP)

ifdef DOPACKAGE
DISTRFILES         += $(GO4EVENT_S) $(GO4EVENT_H) $(GO4EVENT_LINKDEF)
DISTRFILES         += $(GO4EVENTPAR_S) $(GO4EVENTPAR_H) $(GO4EVENTPAR_LINKDEF)
endif


##### local rules #####

include/%.h: $(GO4EVENT_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

$(GO4EVENT_DS): $(GO4EVENT_H)  $(GO4EVENT_LINKDEF)
	@$(ROOTCINTGO4) $(GO4EVENT_H) $(GO4EVENT_LINKDEF)

$(GO4EVENTPAR_DS): $(GO4EVENTPAR_H)  $(GO4EVENTPAR_LINKDEF)
	@$(ROOTCINTGO4) $(GO4EVENTPAR_H) $(GO4EVENTPAR_LINKDEF)

clean-bin::
	@rm -f $(GO4EVENT_O) $(GO4EVENT_DO)
	@rm -f $(GO4EVENTPAR_O) $(GO4EVENTPAR_DO)
	@rm -f $(GO4EVENT_DEP) $(GO4EVENT_DDEP) $(GO4EVENT_DS) $(GO4EVENT_DH)
	@rm -f $(GO4EVENTPAR_DEP) $(GO4EVENTPAR_DDEP) $(GO4EVENTPAR_DS) $(GO4EVENTPAR_DH)
