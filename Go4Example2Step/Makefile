ifndef GO4SYS
GO4SYS = $(shell go4-config --go4sys)
endif

include $(GO4SYS)/Makefile.config

##ANAL_LIB_DEP+=$(ROOTLIBPATH)/libMathMore.so
#LDFLAGS += $(LIBS_FULLSET) -lMathMore

## normally should be like this for every module, but can be specific

ifdef GO4PACKAGE
EXAMP2STEP_DIR         = Go4Example2Step
else
EXAMP2STEP_DIR         = .
endif

EXAMP2STEP_LINKDEF     = $(EXAMP2STEP_DIR)/XXXLinkDef.$(HedSuf)
EXAMP2STEP_LIBNAME     = $(GO4_USERLIBNAME)

## must be similar for every module

EXAMP2STEP_DICT        = $(EXAMP2STEP_DIR)/$(DICT_PREFIX)XXX
EXAMP2STEP_DH          = $(EXAMP2STEP_DICT).$(HedSuf)
EXAMP2STEP_DS          = $(EXAMP2STEP_DICT).$(SrcSuf)
EXAMP2STEP_DO          = $(EXAMP2STEP_DICT).$(ObjSuf)

EXAMP2STEP_H           = $(filter-out $(EXAMP2STEP_DH) $(EXAMP2STEP_LINKDEF), $(wildcard $(EXAMP2STEP_DIR)/*.$(HedSuf)))
EXAMP2STEP_S           = $(filter-out $(EXAMP2STEP_DS), $(wildcard $(EXAMP2STEP_DIR)/*.$(SrcSuf)))
EXAMP2STEP_O           = $(EXAMP2STEP_S:.$(SrcSuf)=.$(ObjSuf))

EXAMP2STEP_DEP         =  $(EXAMP2STEP_O:.$(ObjSuf)=.$(DepSuf))
EXAMP2STEP_DDEP        =  $(EXAMP2STEP_DO:.$(ObjSuf)=.$(DepSuf))

EXAMP2STEP_LIB         =  $(EXAMP2STEP_DIR)/$(EXAMP2STEP_LIBNAME).$(DllSuf)

# used in the main Makefile

EXAMPDEPENDENCS    += $(EXAMP2STEP_DEP) $(EXAMP2STEP_DDEP)

ifdef DOPACKAGE
DISTRFILES         += $(EXAMP2STEP_S) $(EXAMP2STEP_H) $(EXAMP2STEP_LINKDEF)
DISTRFILES         += $(EXAMP2STEP_DIR)/Readme.md
DISTRFILES         += $(EXAMP2STEP_DIR)/set_Par.C
endif

##### local rules #####

all::  $(EXAMP2STEP_LIB)


$(EXAMP2STEP_LIB):   $(EXAMP2STEP_O) $(EXAMP2STEP_DO) $(ANAL_LIB_DEP)
	@$(MakeLibrary) $(EXAMP2STEP_LIBNAME) "$(EXAMP2STEP_O) $(EXAMP2STEP_DO)" $(EXAMP2STEP_DIR) $(EXAMP2STEP_LINKDEF) "$(ANAL_LIB_DEP)" $(EXAMP2STEP_DS) "$(EXAMP2STEP_H)"

$(EXAMP2STEP_DS): $(EXAMP2STEP_H)  $(EXAMP2STEP_LINKDEF)
	@$(ROOTCINTGO4) $(EXAMP2STEP_LIB) $(EXAMP2STEP_H) $(EXAMP2STEP_LINKDEF)

clean-bin::
	@$(RM) $(EXAMP2STEP_O) $(EXAMP2STEP_DO)
	@$(RM) $(EXAMP2STEP_DEP) $(EXAMP2STEP_DDEP) $(EXAMP2STEP_DS) $(EXAMP2STEP_DH)

clean:: clean-bin
	@$(CleanLib) $(EXAMP2STEP_LIBNAME) $(EXAMP2STEP_DIR)

include $(GO4SYS)/Makefile.rules
