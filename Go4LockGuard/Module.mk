LOCKGRD_NAME        = Go4LockGuard
MODULE_NAME         = $(LOCKGRD_NAME)

## normally should be like this for every module, but can be specific

LOCKGRD_DIR         = $(GO4SYS)/$(LOCKGRD_NAME)
LOCKGRD_LINKDEF     = $(LOCKGRD_DIR)/$(LOCKGRD_NAME)LinkDef.$(HedSuf)

LOCKGRD_NOTLIBF     =

## must be similar for every module

LOCKGRD_DICT        = $(LOCKGRD_DIR)/$(DICT_PREFIX)$(LOCKGRD_NAME)
LOCKGRD_DH          = $(LOCKGRD_DICT).$(HedSuf)
LOCKGRD_DS          = $(LOCKGRD_DICT).$(SrcSuf)
LOCKGRD_DO          = $(LOCKGRD_DICT).$(ObjSuf)

LOCKGRD_H           = $(filter-out $(LOCKGRD_NOTLIBF) $(LOCKGRD_DH) $(LOCKGRD_LINKDEF), $(wildcard $(LOCKGRD_DIR)/*.$(HedSuf)))
LOCKGRD_S           = $(filter-out $(LOCKGRD_NOTLIBF) $(LOCKGRD_DS), $(wildcard $(LOCKGRD_DIR)/*.$(SrcSuf)))
LOCKGRD_O           = $(LOCKGRD_S:.$(SrcSuf)=.$(ObjSuf))

LOCKGRD_DEP         =  $(LOCKGRD_O:.$(ObjSuf)=.$(DepSuf))
LOCKGRD_DDEP        =  $(LOCKGRD_DO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(LOCKGRD_DIR)/%.h, $(GO4SYS)/include/%.h, $(LOCKGRD_H))

LIBDEPENDENC       += $(LOCKGRD_DEP) $(LOCKGRD_DDEP)

ifdef DOPACKAGE
LOCKGRD_DISTRFILES  = $(LOCKGRD_S) $(LOCKGRD_H) $(LOCKGRD_LINKDEF)
DISTRFILES         += $(LOCKGRD_DISTRFILES)
endif

##### local rules #####

$(GO4SYS)/include/%.h: $(LOCKGRD_DIR)/%.h
	@cp -f $< $@

$(LOCKGRD_DS): $(LOCKGRD_H)  $(LOCKGRD_LINKDEF)
		@$(ROOTCINTGO4) $(LOCKGRD_H) $(LOCKGRD_LINKDEF)

all-$(LOCKGRD_NAME):  $(LOCKGRD_O) $(LOCKGRD_DO)

clean-obj-$(LOCKGRD_NAME):
		@rm -f $(LOCKGRD_O) $(LOCKGRD_DO)

clean-$(LOCKGRD_NAME): clean-obj-$(LOCKGRD_NAME)
		@rm -f $(LOCKGRD_DEP) $(LOCKGRD_DDEP) $(LOCKGRD_DS) $(LOCKGRD_DH)
