GO4PROX_NAME        = Go4Proxies
MODULE_NAME         = $(GO4PROX_NAME)

## normally should be like this for every module, but can be specific

GO4PROX_DIR         = $(GO4SYS)/$(GO4PROX_NAME)
GO4PROX_LINKDEF     = $(GO4PROX_DIR)/$(GO4PROX_NAME)LinkDef.$(HedSuf)

GO4PROX_NOTLIBF     =

## must be similar for every module

GO4PROX_DICT        = $(GO4PROX_DIR)/$(DICT_PREFIX)$(GO4PROX_NAME)
GO4PROX_DH          = $(GO4PROX_DICT).$(HedSuf)
GO4PROX_DS          = $(GO4PROX_DICT).$(SrcSuf)
GO4PROX_DO          = $(GO4PROX_DICT).$(ObjSuf)

GO4PROX_H           = $(filter-out $(GO4PROX_NOTLIBF) $(GO4PROX_DH) $(GO4PROX_LINKDEF), $(wildcard $(GO4PROX_DIR)/*.$(HedSuf)))
GO4PROX_S           = $(filter-out $(GO4PROX_NOTLIBF) $(GO4PROX_DS), $(wildcard $(GO4PROX_DIR)/*.$(SrcSuf)))
GO4PROX_O           = $(GO4PROX_S:.$(SrcSuf)=.$(ObjSuf))

GO4PROX_DEP         =  $(GO4PROX_O:.$(ObjSuf)=.$(DepSuf))
GO4PROX_DDEP        =  $(GO4PROX_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(GO4PROX_DIR)/%.h, $(GO4SYS)/include/%.h, $(GO4PROX_H))

LIBDEPENDENC       += $(GO4PROX_DEP) $(GO4PROX_DDEP)

ifdef DOPACKAGE
GO4PROX_DISTRFILES = $(GO4PROX_S) $(GO4PROX_H) $(GO4PROX_LINKDEF)
DISTRFILES         += $(GO4PROX_DISTRFILES)
endif


##### local rules #####

$(GO4SYS)/include/%.h: $(GO4PROX_DIR)/%.h
	@cp -f $< $@

$(GO4PROX_DS): $(GO4PROX_H)  $(GO4PROX_LINKDEF)
		@$(ROOTCINTGO4) $(GO4PROX_H) $(GO4PROX_LINKDEF)

all-$(GO4PROX_NAME):  $(GO4PROX_O) $(GO4PROX_DO)

clean-obj-$(GO4PROX_NAME):
		@rm -f $(GO4PROX_O) $(GO4PROX_DO)

clean-$(GO4PROX_NAME): clean-obj-$(GO4PROX_NAME)
		@rm -f $(GO4PROX_DEP) $(GO4PROX_DDEP) $(GO4PROX_DS) $(GO4PROX_DH)
