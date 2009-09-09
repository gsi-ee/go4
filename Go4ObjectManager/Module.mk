GO4OBJM_NAME     = Go4ObjectManager

## normally should be like this for every module, but can be specific

GO4OBJM_DIR         = $(GO4OBJM_NAME)
GO4OBJM_LINKDEF     = $(GO4OBJM_DIR)/$(GO4OBJM_NAME)LinkDef.$(HedSuf)

GO4OBJM_NOTLIBF     =

## must be similar for every module

GO4OBJM_DICT        = $(GO4OBJM_DIR)/$(DICT_PREFIX)$(GO4OBJM_NAME)
GO4OBJM_DH          = $(GO4OBJM_DICT).$(HedSuf)
GO4OBJM_DS          = $(GO4OBJM_DICT).$(SrcSuf)
GO4OBJM_DO          = $(GO4OBJM_DICT).$(ObjSuf)

GO4OBJM_H           = $(filter-out $(GO4OBJM_NOTLIBF) $(GO4OBJM_DH) $(GO4OBJM_LINKDEF), $(wildcard $(GO4OBJM_DIR)/*.$(HedSuf)))
GO4OBJM_S           = $(filter-out $(GO4OBJM_NOTLIBF) $(GO4OBJM_DS), $(wildcard $(GO4OBJM_DIR)/*.$(SrcSuf)))
GO4OBJM_O           = $(GO4OBJM_S:.$(SrcSuf)=.$(ObjSuf))

GO4OBJM_DEP         =  $(GO4OBJM_O:.$(ObjSuf)=.$(DepSuf))
GO4OBJM_DDEP        =  $(GO4OBJM_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(GO4OBJM_DIR)/%.h, include/%.h, $(GO4OBJM_H))

LIBDEPENDENC       += $(GO4OBJM_DDEP) $(GO4OBJM_DEP)

ifdef DOPACKAGE
DISTRFILES         += $(GO4OBJM_S) $(GO4OBJM_H) $(GO4OBJM_LINKDEF)
DISTRFILES         += $(GO4OBJM_DIR)/*.C
endif

##### local rules #####

include/%.h: $(GO4OBJM_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

$(GO4OBJM_DS): $(GO4OBJM_H)  $(GO4OBJM_LINKDEF)
	@$(ROOTCINTGO4) $(GO4OBJM_H) $(GO4OBJM_LINKDEF)

clean-bin::
	@rm -f $(GO4OBJM_O) $(GO4OBJM_DO)
	@rm -f $(GO4OBJM_DEP) $(GO4OBJM_DDEP) $(GO4OBJM_DS) $(GO4OBJM_DH)
