STATBASE_NAME        = Go4StatusBase
MODULE_NAME         = $(STATBASE_NAME)

## normally should be like this for every module, but can be specific

STATBASE_DIR         = $(GO4SYS)/$(STATBASE_NAME)
STATBASE_LINKDEF     = $(STATBASE_DIR)/$(STATBASE_NAME)LinkDef.$(HedSuf)

STATBASE_NOTLIBF     =

## must be similar for every module

STATBASE_DICT        = $(STATBASE_DIR)/$(DICT_PREFIX)$(STATBASE_NAME)
STATBASE_DH          = $(STATBASE_DICT).$(HedSuf)
STATBASE_DS          = $(STATBASE_DICT).$(SrcSuf)
STATBASE_DO          = $(STATBASE_DICT).$(ObjSuf)

STATBASE_H           = $(filter-out $(STATBASE_NOTLIBF) $(STATBASE_DH) $(STATBASE_LINKDEF), $(wildcard $(STATBASE_DIR)/*.$(HedSuf)))
STATBASE_S           = $(filter-out $(STATBASE_NOTLIBF) $(STATBASE_DS), $(wildcard $(STATBASE_DIR)/*.$(SrcSuf)))
STATBASE_O           = $(STATBASE_S:.$(SrcSuf)=.$(ObjSuf))

STATBASE_DEP         =  $(STATBASE_O:.$(ObjSuf)=.$(DepSuf))
STATBASE_DDEP        =  $(STATBASE_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(STATBASE_DIR)/%.h, $(GO4SYS)/include/%.h, $(STATBASE_H))

LIBDEPENDENC       += $(STATBASE_DEP) $(STATBASE_DDEP)

ifdef DOPACKAGE
STATBASE_DISTRFILES = $(STATBASE_S) $(STATBASE_H) $(STATBASE_LINKDEF)
DISTRFILES         += $(STATBASE_DISTRFILES)
endif


##### local rules #####

$(GO4SYS)/include/%.h: $(STATBASE_DIR)/%.h
	@cp -f $< $@

$(STATBASE_DS): $(STATBASE_H)  $(STATBASE_LINKDEF)
		@$(ROOTCINTGO4) $(STATBASE_H) $(STATBASE_LINKDEF)

all-$(STATBASE_NAME):  $(STATBASE_O) $(STATBASE_DO)

clean-obj-$(STATBASE_NAME):
		@rm -f $(STATBASE_O) $(STATBASE_DO)

clean-$(STATBASE_NAME): clean-obj-$(STATBASE_NAME)
		@rm -f $(STATBASE_DEP) $(STATBASE_DDEP) $(STATBASE_DS) $(STATBASE_DH)
