ifndef GO4SYS
GO4SYS = $(shell go4-config --go4sys)
endif

include $(GO4SYS)/Makefile.config

## normally should be like this for every module, but can be specific

ifdef GO4PACKAGE
EXAMPSIMP_DIR         = Go4ExampleSimple
else
EXAMPSIMP_DIR         = .
endif

EXAMPSIMP_LINKDEF     = $(EXAMPSIMP_DIR)/XXXLinkDef.$(HedSuf)
EXAMPSIMP_LIBNAME     = $(GO4_USERLIBNAME)

## must be similar for every module

EXAMPSIMP_DICT        = $(EXAMPSIMP_DIR)/$(DICT_PREFIX)XXX
EXAMPSIMP_DH          = $(EXAMPSIMP_DICT).$(HedSuf)
EXAMPSIMP_DS          = $(EXAMPSIMP_DICT).$(SrcSuf)
EXAMPSIMP_DO          = $(EXAMPSIMP_DICT).$(ObjSuf)

EXAMPSIMP_H           = $(filter-out $(EXAMPSIMP_DH) $(EXAMPSIMP_LINKDEF), $(wildcard $(EXAMPSIMP_DIR)/*.$(HedSuf)))
EXAMPSIMP_S           = $(filter-out $(EXAMPSIMP_DS), $(wildcard $(EXAMPSIMP_DIR)/*.$(SrcSuf)))
EXAMPSIMP_O           = $(EXAMPSIMP_S:.$(SrcSuf)=.$(ObjSuf))

EXAMPSIMP_DEP         = $(EXAMPSIMP_O:.$(ObjSuf)=.$(DepSuf))
EXAMPSIMP_DDEP        = $(EXAMPSIMP_DO:.$(ObjSuf)=.$(DepSuf))

EXAMPSIMP_LIB         = $(EXAMPSIMP_DIR)/$(EXAMPSIMP_LIBNAME).$(DllSuf)

# used in the main Makefile

EXAMPDEPENDENCS    += $(EXAMPSIMP_DEP) $(EXAMPSIMP_DDEP) $(EXAMPSIMP_EDEP)

ifdef DOPACKAGE
DISTRFILES         += $(EXAMPSIMP_S) $(EXAMPSIMP_H) $(EXAMPSIMP_LINKDEF)
DISTRFILES         += $(EXAMPSIMP_DIR)/Readme.txt
endif

##### local rules #####

all::  $(EXAMPSIMP_LIB)

$(EXAMPSIMP_LIB):   $(EXAMPSIMP_O) $(EXAMPSIMP_DO)
	@$(MakeLibrary) $(EXAMPSIMP_LIBNAME) "$(EXAMPSIMP_O) $(EXAMPSIMP_DO)" $(EXAMPSIMP_DIR) $(EXAMPSIMP_LINKDEF) "$(ANAL_LIB_DEP)" $(EXAMPSIMP_DS) "$(EXAMPSIMP_H)"

$(EXAMPSIMP_DS): $(EXAMPSIMP_H)  $(EXAMPSIMP_LINKDEF)
	@$(ROOTCINTGO4) $(EXAMPSIMP_LIB) $(EXAMPSIMP_H) $(EXAMPSIMP_LINKDEF)

clean-bin::
	@rm -f $(EXAMPSIMP_O) $(EXAMPSIMP_DO)
	@rm -f $(EXAMPSIMP_DEP) $(EXAMPSIMP_DDEP) $(EXAMPSIMP_DS) $(EXAMPSIMP_DH)

clean:: clean-bin
	@$(CleanLib) $(EXAMPSIMP_LIBNAME) $(EXAMPSIMP_DIR)

include $(GO4SYS)/Makefile.rules
