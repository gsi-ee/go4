CONDBASE_NAME        = Go4ConditionsBase

## normally should be like this for every module, but can be specific

CONDBASE_DIR         = $(GO4SYS)/$(CONDBASE_NAME)
CONDBASE_LINKDEF     = $(CONDBASE_DIR)/$(CONDBASE_NAME)LinkDef.$(HedSuf)

CONDBASE_NOTLIBF     =

## must be similar for every module

CONDBASE_DICT        = $(CONDBASE_DIR)/$(DICT_PREFIX)$(CONDBASE_NAME)
CONDBASE_DH          = $(CONDBASE_DICT).$(HedSuf)
CONDBASE_DS          = $(CONDBASE_DICT).$(SrcSuf)
CONDBASE_DO          = $(CONDBASE_DICT).$(ObjSuf)

CONDBASE_H           = $(filter-out $(CONDBASE_NOTLIBF) $(CONDBASE_DH) $(CONDBASE_LINKDEF) $(CONDBASE_OTHERF), $(wildcard $(CONDBASE_DIR)/*.$(HedSuf)))
CONDBASE_S           = $(filter-out $(CONDBASE_NOTLIBF) $(CONDBASE_DS), $(wildcard $(CONDBASE_DIR)/*.$(SrcSuf)))
CONDBASE_O           = $(CONDBASE_S:.$(SrcSuf)=.$(ObjSuf))

CONDBASE_DEP         =  $(CONDBASE_O:.$(ObjSuf)=.$(DepSuf))
CONDBASE_DDEP        =  $(CONDBASE_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(CONDBASE_DIR)/%.h, $(GO4SYS)/include/%.h, $(CONDBASE_H))

LIBDEPENDENC       += $(CONDBASE_DEP) $(CONDBASE_DDEP)

ifdef DOPACKAGE
DISTRFILES         += $(CONDBASE_S) $(CONDBASE_H) $(CONDBASE_LINKDEF)
endif

##### local rules #####

$(GO4SYS)/include/%.h: $(CONDBASE_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

$(CONDBASE_DS): $(CONDBASE_H)  $(CONDBASE_LINKDEF)
	@$(ROOTCINTGO4) $(CONDBASE_H) $(CONDBASE_LINKDEF)

clean::
	@rm -f $(CONDBASE_O) $(CONDBASE_DO)
	@rm -f $(CONDBASE_DEP) $(CONDBASE_DDEP) $(CONDBASE_DS) $(CONDBASE_DH)

