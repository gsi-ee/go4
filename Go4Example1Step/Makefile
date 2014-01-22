ifndef GO4SYS
GO4SYS = $(shell go4-config --go4sys)
endif

include $(GO4SYS)/Makefile.config

## normally should be like this for every module, but can be specific

ifdef GO4PACKAGE
EXAMP1STEP_DIR         = Go4Example1Step
else
EXAMP1STEP_DIR         = .
endif

EXAMP1STEP_LINKDEF     = $(EXAMP1STEP_DIR)/XXXLinkDef.$(HedSuf)
EXAMP1STEP_LIBNAME     = $(GO4_USERLIBNAME)

## must be similar for every module

EXAMP1STEP_DICT        = $(EXAMP1STEP_DIR)/$(DICT_PREFIX)XXX
EXAMP1STEP_DH          = $(EXAMP1STEP_DICT).$(HedSuf)
EXAMP1STEP_DS          = $(EXAMP1STEP_DICT).$(SrcSuf)
EXAMP1STEP_DO          = $(EXAMP1STEP_DICT).$(ObjSuf)
EXAMP1STEP_D6          = $(EXAMP1STEP_DICT)$(DICT_R6SUFF)

EXAMP1STEP_H           = $(filter-out $(EXAMP1STEP_DH) $(EXAMP1STEP_LINKDEF), $(wildcard $(EXAMP1STEP_DIR)/*.$(HedSuf)))
EXAMP1STEP_S           = $(filter-out $(EXAMP1STEP_DS), $(wildcard $(EXAMP1STEP_DIR)/*.$(SrcSuf)))
EXAMP1STEP_O           = $(EXAMP1STEP_S:.$(SrcSuf)=.$(ObjSuf))

EXAMP1STEP_DEP         =  $(EXAMP1STEP_O:.$(ObjSuf)=.$(DepSuf))
EXAMP1STEP_DDEP        =  $(EXAMP1STEP_DO:.$(ObjSuf)=.$(DepSuf))

EXAMP1STEP_LIB         =  $(EXAMP1STEP_DIR)/$(EXAMP1STEP_LIBNAME).$(DllSuf)

# used in the main Makefile

EXAMPDEPENDENCS    += $(EXAMP1STEP_DEP) $(EXAMP1STEP_DDEP)

ifdef DOPACKAGE
DISTRFILES         += $(EXAMP1STEP_S) $(EXAMP1STEP_H) $(EXAMP1STEP_LINKDEF)
DISTRFILES         += $(EXAMP1STEP_DIR)/Readme.txt
DISTRFILES         += $(wildcard $(EXAMP1STEP_DIR)/*.C)
endif

##### local rules #####

all::     $(EXAMP1STEP_LIB)

$(EXAMP1STEP_LIB):  $(EXAMP1STEP_O) $(EXAMP1STEP_DO)
	@$(MakeLibrary) $(EXAMP1STEP_LIBNAME) "$(EXAMP1STEP_O) $(EXAMP1STEP_DO)" $(EXAMP1STEP_DIR) $(EXAMP1STEP_LINKDEF) "$(ANAL_LIB_DEP)"

$(EXAMP1STEP_DS): $(EXAMP1STEP_H)  $(EXAMP1STEP_LINKDEF)
	@$(ROOTCINTGO4) $(EXAMP1STEP_LIB) $(EXAMP1STEP_H) $(EXAMP1STEP_LINKDEF)

clean-bin::
	@rm -f $(EXAMP1STEP_O) $(EXAMP1STEP_DO) $(EXAMP1STEP_D6)
	@rm -f $(EXAMP1STEP_DEP) $(EXAMP1STEP_DDEP) $(EXAMP1STEP_DS) $(EXAMP1STEP_DH)

clean:: clean-bin
	@$(CleanLib) $(EXAMP1STEP_LIBNAME) $(EXAMP1STEP_DIR)

include $(GO4SYS)/Makefile.rules
