COMBASE_NAME        = Go4CommandsBase

## normally should be like this for every module, but can be specific

COMBASE_DIR         = $(GO4SYS)/$(COMBASE_NAME)
COMBASE_LINKDEF     = $(COMBASE_DIR)/$(COMBASE_NAME)LinkDef.$(HedSuf)

COMBASE_NOTLIBF     = 

## must be similar for every module

COMBASE_DICT        = $(COMBASE_DIR)/$(DICT_PREFIX)$(COMBASE_NAME)
COMBASE_DH          = $(COMBASE_DICT).$(HedSuf)
COMBASE_DS          = $(COMBASE_DICT).$(SrcSuf)
COMBASE_DO          = $(COMBASE_DICT).$(ObjSuf)

COMBASE_H           = $(filter-out $(COMBASE_NOTLIBF) $(COMBASE_DH) $(COMBASE_LINKDEF) $(COMBASE_OTHERF), $(wildcard $(COMBASE_DIR)/*.$(HedSuf)))
COMBASE_S           = $(filter-out $(COMBASE_NOTLIBF) $(COMBASE_DS), $(wildcard $(COMBASE_DIR)/*.$(SrcSuf)))
COMBASE_O           = $(COMBASE_S:.$(SrcSuf)=.$(ObjSuf))

COMBASE_DEP         =  $(COMBASE_O:.$(ObjSuf)=.$(DepSuf))
COMBASE_DDEP        =  $(COMBASE_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(COMBASE_DIR)/%.h, $(GO4SYS)/include/%.h, $(COMBASE_H))

LIBDEPENDENC       += $(COMBASE_DEP) $(COMBASE_DDEP)

ifdef DOPACKAGE
COMBASE_DISTRFILES  = $(COMBASE_S) $(COMBASE_H) $(COMBASE_LINKDEF) $(COMBASE_NOTLIBF)
DISTRFILES         += $(COMBASE_DISTRFILES)
endif


##### local rules #####

$(GO4SYS)/include/%.h: $(COMBASE_DIR)/%.h
	@cp -f $< $@

$(COMBASE_DS): $(COMBASE_H)  $(COMBASE_LINKDEF)
		@$(ROOTCINTGO4) $(COMBASE_H) $(COMBASE_LINKDEF)

all-$(COMBASE_NAME):  $(COMBASE_O) $(COMBASE_DO)

clean-obj-$(COMBASE_NAME):
		@rm -f $(COMBASE_O) $(COMBASE_DO)

clean-$(COMBASE_NAME): clean-obj-$(COMBASE_NAME)
		@rm -f $(COMBASE_DEP) $(COMBASE_DDEP) $(COMBASE_DS) $(COMBASE_DH)
