ifndef GO4SYS
GO4SYS = $(shell go4-config --go4sys)
endif

include $(GO4SYS)/Makefile.config

EXAMPSIMP_NAME        = Go4ExampleSimple

## normally should be like this for every module, but can be specific

ifdef GO4PACKAGE
EXAMPSIMP_DIR         = $(EXAMPSIMP_NAME)
else
EXAMPSIMP_DIR         = .
endif

EXAMPSIMP_LINKDEF     = $(EXAMPSIMP_DIR)/Go4UserAnalysisLinkDef.$(HedSuf)
EXAMPSIMP_LIBNAME     = libGo4UserAnalysis
EXAMPSIMP_EXENAME     = MainUserAnalysis

EXAMPSIMP_NOTLIBF     = 

## must be similar for every module

EXAMPSIMP_EXEO        = $(EXAMPSIMP_DIR)/$(EXAMPSIMP_EXENAME).$(ObjSuf)
#EXAMPSIMP_EXEH        = $(EXAMPSIMP_DIR)/$(EXAMPSIMP_EXENAME).$(HedSuf)
EXAMPSIMP_EXES        = $(EXAMPSIMP_DIR)/$(EXAMPSIMP_EXENAME).$(SrcSuf)
EXAMPSIMP_EXE         = $(EXAMPSIMP_DIR)/$(EXAMPSIMP_EXENAME)$(ExeSuf)

EXAMPSIMP_DICT        = $(EXAMPSIMP_DIR)/$(DICT_PREFIX)$(EXAMPSIMP_NAME)
EXAMPSIMP_DH          = $(EXAMPSIMP_DICT).$(HedSuf)
EXAMPSIMP_DS          = $(EXAMPSIMP_DICT).$(SrcSuf)
EXAMPSIMP_DO          = $(EXAMPSIMP_DICT).$(ObjSuf)

EXAMPSIMP_H           = $(filter-out $(EXAMPSIMP_EXEH) $(EXAMPSIMP_NOTLIBF) $(EXAMPSIMP_DH) $(EXAMPSIMP_LINKDEF), $(wildcard $(EXAMPSIMP_DIR)/*.$(HedSuf)))
EXAMPSIMP_S           = $(filter-out $(EXAMPSIMP_EXES) $(EXAMPSIMP_NOTLIBF) $(EXAMPSIMP_DS), $(wildcard $(EXAMPSIMP_DIR)/*.$(SrcSuf)))
EXAMPSIMP_O           = $(EXAMPSIMP_S:.$(SrcSuf)=.$(ObjSuf))

EXAMPSIMP_DEP         =  $(EXAMPSIMP_O:.$(ObjSuf)=.$(DepSuf))
EXAMPSIMP_DDEP        =  $(EXAMPSIMP_DO:.$(ObjSuf)=.$(DepSuf))
EXAMPSIMP_EDEP        =  $(EXAMPSIMP_EXEO:.$(ObjSuf)=.$(DepSuf))

EXAMPSIMP_LIB         =  $(EXAMPSIMP_DIR)/$(EXAMPSIMP_LIBNAME).$(DllSuf)

# used in the main Makefile

EXAMPDEPENDENCS    += $(EXAMPSIMP_DEP) $(EXAMPSIMP_DDEP) $(EXAMPSIMP_EDEP)

ifdef DOPACKAGE
DISTRFILES         += $(EXAMPSIMP_S) $(EXAMPSIMP_H) $(EXAMPSIMP_LINKDEF) $(EXAMPSIMP_EXEH) $(EXAMPSIMP_EXES)
DISTRFILES         += $(EXAMPSIMP_DIR)/Readme.txt
DISTRFILES         += $(EXAMPSIMP_DIR)/AnalysisStart.sh $(EXAMPSIMP_DIR)/rename.sh 
DISTRFILES         += $(EXAMPSIMP_DIR)/*.C 
endif

##### local rules #####

all::  $(EXAMPSIMP_LIB) $(EXAMPSIMP_EXE)

$(EXAMPSIMP_EXE):   $(BUILDGO4LIBS) $(EXAMPSIMP_EXEO) $(EXAMPSIMP_LIB)
	$(LD) $(LDFLAGS) $(EXAMPSIMP_EXEO) $(LIBS_FULLSET) $(EXAMPSIMP_LIB) $(OutPutOpt) $(EXAMPSIMP_EXE)
	@echo "$@  done"

$(EXAMPSIMP_LIB):   $(EXAMPSIMP_O) $(EXAMPSIMP_DO)
	@$(MakeLibrary) $(EXAMPSIMP_LIBNAME) "$(EXAMPSIMP_O) $(EXAMPSIMP_DO)" $(EXAMPSIMP_DIR) $(EXAMPSIMP_LINKDEF) "$(ANAL_LIB_DEP)"

$(EXAMPSIMP_DS): $(EXAMPSIMP_H)  $(EXAMPSIMP_LINKDEF)
	@$(ROOTCINTGO4) $(EXAMPSIMP_H) $(EXAMPSIMP_LINKDEF)

clean-bin::
	@rm -f $(EXAMPSIMP_O) $(EXAMPSIMP_DO)
	@rm -f $(EXAMPSIMP_EXEO) $(EXAMPSIMP_EDEP)
	@rm -f $(EXAMPSIMP_DEP) $(EXAMPSIMP_DDEP) $(EXAMPSIMP_DS) $(EXAMPSIMP_DH)

clean:: clean-bin
	@$(CleanLib) $(EXAMPSIMP_LIBNAME) $(EXAMPSIMP_DIR)
	@rm -f $(EXAMPSIMP_EXE)

include $(GO4SYS)/Makefile.rules
