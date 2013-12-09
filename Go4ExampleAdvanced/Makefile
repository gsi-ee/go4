ifndef GO4SYS
GO4SYS = $(shell go4-config --go4sys)
endif

include $(GO4SYS)/Makefile.config

## normally should be like this for every module, but can be specific

ifdef GO4PACKAGE
EXAMPADVANC_DIR         = Go4ExampleAdvanced
else
EXAMPADVANC_DIR         = .
endif

EXAMPADVANC_LINKDEF     = $(EXAMPADVANC_DIR)/XXXLinkDef.$(HedSuf)
EXAMPADVANC_LIBNAME     = $(GO4_USERLIBNAME)

## must be similar for every module

EXAMPADVANC_DICT        = $(EXAMPADVANC_DIR)/$(DICT_PREFIX)XXX
EXAMPADVANC_DH          = $(EXAMPADVANC_DICT).$(HedSuf)
EXAMPADVANC_DS          = $(EXAMPADVANC_DICT).$(SrcSuf)
EXAMPADVANC_DO          = $(EXAMPADVANC_DICT).$(ObjSuf)
EXAMPADVANC_D6          = $(EXAMPADVANC_DICT)$(DICT_R6SUFF)

EXAMPADVANC_H           = $(filter-out $(EXAMPADVANC_DH) $(EXAMPADVANC_LINKDEF), $(wildcard $(EXAMPADVANC_DIR)/*.$(HedSuf)))
EXAMPADVANC_S           = $(filter-out $(EXAMPADVANC_DS), $(wildcard $(EXAMPADVANC_DIR)/*.$(SrcSuf)))
EXAMPADVANC_O           = $(EXAMPADVANC_S:.$(SrcSuf)=.$(ObjSuf))

EXAMPADVANC_DEP         =  $(EXAMPADVANC_O:.$(ObjSuf)=.$(DepSuf))
EXAMPADVANC_DDEP        =  $(EXAMPADVANC_DO:.$(ObjSuf)=.$(DepSuf))

EXAMPADVANC_LIB         =  $(EXAMPADVANC_DIR)/$(EXAMPADVANC_LIBNAME).$(DllSuf)

# used in the main Makefile

EXAMPDEPENDENCS    += $(EXAMPADVANC_DEP) $(EXAMPADVANC_DDEP)

ifdef DOPACKAGE
DISTRFILES         += $(EXAMPADVANC_S) $(EXAMPADVANC_H) $(EXAMPADVANC_LINKDEF)
DISTRFILES         += $(EXAMPADVANC_DIR)/Readme.txt $(EXAMPADVANC_DIR)/Makefile.win
DISTRFILES         += $(EXAMPADVANC_DIR)/calilines.txt
DISTRFILES         += $(EXAMPADVANC_DIR)/gaussfiles.lml
DISTRFILES         += $(EXAMPADVANC_DIR)/convertfile.C
DISTRFILES         += $(EXAMPADVANC_DIR)/findobjects.C
DISTRFILES         += $(EXAMPADVANC_DIR)/browse.C
DISTRFILES         += $(EXAMPADVANC_DIR)/plothistos.C
endif

##### local rules #####

all::  $(EXAMPADVANC_LIB)

$(EXAMPADVANC_LIB):   $(EXAMPADVANC_O) $(EXAMPADVANC_DO)
	@$(MakeLibrary) $(EXAMPADVANC_LIBNAME) "$(EXAMPADVANC_O) $(EXAMPADVANC_DO)" $(EXAMPADVANC_DIR) $(EXAMPADVANC_LINKDEF) "$(ANAL_LIB_DEP)"

$(EXAMPADVANC_DS): $(EXAMPADVANC_H)  $(EXAMPADVANC_LINKDEF)
	@$(ROOTCINTGO4) $(EXAMPADVANC_H) $(EXAMPADVANC_LINKDEF)

clean-bin::
	@rm -f $(EXAMPADVANC_O) $(EXAMPADVANC_DO) $(EXAMPADVANC_D6)
	@rm -f $(EXAMPADVANC_DEP) $(EXAMPADVANC_DDEP) $(EXAMPADVANC_DS) $(EXAMPADVANC_DH)

clean:: clean-bin
	@$(CleanLib) $(EXAMPADVANC_LIBNAME) $(EXAMPADVANC_DIR)

include $(GO4SYS)/Makefile.rules
