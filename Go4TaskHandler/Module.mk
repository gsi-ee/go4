TASKHAND_NAME        = Go4TaskHandler

## normally should be like this for every module, but can be specific

TASKHAND_DIR         = $(GO4SYS)/$(TASKHAND_NAME)
TASKHAND_LINKDEF     = $(TASKHAND_DIR)/$(TASKHAND_NAME)LinkDef.$(HedSuf)

TASKHAND_NOTLIBF     = $(TASKHAND_DIR)/Go4EmergencyCommands.h

## must be similar for every module

TASKHAND_DICT        = $(TASKHAND_DIR)/$(DICT_PREFIX)$(TASKHAND_NAME)
TASKHAND_DH          = $(TASKHAND_DICT).$(HedSuf)
TASKHAND_DS          = $(TASKHAND_DICT).$(SrcSuf)
TASKHAND_DO          = $(TASKHAND_DICT).$(ObjSuf)

TASKHAND_H           = $(filter-out $(TASKHAND_NOTLIBF) $(TASKHAND_DH) $(TASKHAND_LINKDEF), $(wildcard $(TASKHAND_DIR)/*.$(HedSuf)))
TASKHAND_S           = $(filter-out $(TASKHAND_NOTLIBF) $(TASKHAND_DS), $(wildcard $(TASKHAND_DIR)/*.$(SrcSuf)))
TASKHAND_O           = $(TASKHAND_S:.$(SrcSuf)=.$(ObjSuf))

TASKHAND_DEP         =  $(TASKHAND_O:.$(ObjSuf)=.$(DepSuf))
TASKHAND_DDEP        =  $(TASKHAND_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(TASKHAND_DIR)/%.h, $(GO4SYS)/include/%.h, $(TASKHAND_H) $(TASKHAND_DIR)/Go4EmergencyCommands.h)

LIBDEPENDENC       += $(TASKHAND_DEP) $(TASKHAND_DDEP)

ifdef DOPACKAGE
TASKHAND_DISTRFILES = $(TASKHAND_S) $(TASKHAND_H) $(TASKHAND_LINKDEF) $(TASKHAND_NOTLIBF)
DISTRFILES         += $(TASKHAND_DISTRFILES)
endif


##### local rules #####

$(GO4SYS)/include/%.h: $(TASKHAND_DIR)/%.h
	@echo "Copy header $@ ..."
	@cp -f $< $@

$(TASKHAND_DS): $(TASKHAND_H) $(TASKHAND_LINKDEF)
	@$(ROOTCINTGO4) $(TASKHAND_H) $(TASKHAND_LINKDEF)

clean::
	@rm -f $(TASKHAND_O) $(TASKHAND_DO)
	@rm -f $(TASKHAND_DEP) $(TASKHAND_DDEP) $(TASKHAND_DS) $(TASKHAND_DH)

