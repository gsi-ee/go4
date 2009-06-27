GO4LOG_NAME        = Go4Log

## normally should be like this for every module, but can be specific

GO4LOG_DIR         = $(GO4SYS)/$(GO4LOG_NAME)
GO4LOG_LINKDEF     = $(GO4LOG_DIR)/$(GO4LOG_NAME)LinkDef.$(HedSuf)

GO4LOG_NOTLIBF     =

## must be similar for every module

GO4LOG_DICT        = $(GO4LOG_DIR)/$(DICT_PREFIX)$(GO4LOG_NAME)
GO4LOG_DH          = $(GO4LOG_DICT).$(HedSuf)
GO4LOG_DS          = $(GO4LOG_DICT).$(SrcSuf)
GO4LOG_DO          = $(GO4LOG_DICT).$(ObjSuf)

GO4LOG_H           = $(filter-out $(GO4LOG_NOTLIBF) $(GO4LOG_DH) $(GO4LOG_LINKDEF), $(wildcard $(GO4LOG_DIR)/*.$(HedSuf)))
GO4LOG_S           = $(filter-out $(GO4LOG_NOTLIBF) $(GO4LOG_DS), $(wildcard $(GO4LOG_DIR)/*.$(SrcSuf)))
GO4LOG_O           = $(GO4LOG_S:.$(SrcSuf)=.$(ObjSuf))

GO4LOG_DEP         =  $(GO4LOG_O:.$(ObjSuf)=.$(DepSuf))
GO4LOG_DDEP        =  $(GO4LOG_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile


ALLHDRS +=  $(patsubst $(GO4LOG_DIR)/%.h, $(GO4SYS)/include/%.h, $(GO4LOG_H))

LIBDEPENDENC       += $(GO4LOG_DEP) $(GO4LOG_DDEP)

ifdef DOPACKAGE
GO4LOG_DISTRFILES   = $(GO4LOG_S) $(GO4LOG_H) $(GO4LOG_LINKDEF)
DISTRFILES         += $(GO4LOG_DISTRFILES)
endif

##### local rules #####

$(GO4SYS)/include/%.h: $(GO4LOG_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

$(GO4LOG_DS): $(GO4LOG_H)  $(GO4LOG_LINKDEF)
	@$(ROOTCINTGO4) $(GO4LOG_H) $(GO4LOG_LINKDEF)

clean-bin::
	@rm -f $(GO4LOG_O) $(GO4LOG_DO)
	@rm -f $(GO4LOG_DEP) $(GO4LOG_DDEP) $(GO4LOG_DS) $(GO4LOG_DH)

