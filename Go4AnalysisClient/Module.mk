ANALCL_NAME        = Go4AnalysisClient

## normally should be like this for every module, but can be specific

ANALCL_DIR         = $(GO4SYS)/$(ANALCL_NAME)
ANALCL_LINKDEF     = $(ANALCL_DIR)/$(ANALCL_NAME)LinkDef.$(HedSuf)
ANALCL_EXENAME     = Main$(ANALCL_NAME)
ANALCL_EXECINTNAME  = go4root

ANALCL_NOTLIBF     = $(ANALCL_DIR)/TGo4AnalysisClient.h \
                     $(ANALCL_DIR)/go4root.cxx \
                     $(ANALCL_DIR)/Readme_CINTServer.txt

## must be similar for every module

ANALCL_EXEO        = $(ANALCL_DIR)/$(ANALCL_EXENAME).$(ObjSuf)
ANALCL_EXES        = $(ANALCL_DIR)/$(ANALCL_EXENAME).$(SrcSuf)
ANALCL_EXE         = $(ANALCL_DIR)/$(ANALCL_EXENAME)$(ExeSuf)   

ANALCL_EXECINTO    = $(ANALCL_DIR)/$(ANALCL_EXECINTNAME).$(ObjSuf)
ANALCL_EXECINTS    = $(ANALCL_DIR)/$(ANALCL_EXECINTNAME).$(SrcSuf)
ANALCL_EXECINT     = $(GO4SYS)/bin/$(ANALCL_EXECINTNAME)$(ExeSuf)  

ANALCL_DICT        = $(ANALCL_DIR)/$(DICT_PREFIX)$(ANALCL_NAME)
ANALCL_DH          = $(ANALCL_DICT).$(HedSuf)
ANALCL_DS          = $(ANALCL_DICT).$(SrcSuf)
ANALCL_DO          = $(ANALCL_DICT).$(ObjSuf)

ANALCL_H           = $(filter-out $(ANALCL_NOTLIBF) $(ANALCL_DH) $(ANALCL_LINKDEF), $(wildcard $(ANALCL_DIR)/*.$(HedSuf)))
ANALCL_S           = $(filter-out $(ANALCL_EXES) $(ANALCL_NOTLIBF) $(ANALCL_DS), $(wildcard $(ANALCL_DIR)/*.$(SrcSuf)))
ANALCL_O           = $(ANALCL_S:.$(SrcSuf)=.$(ObjSuf))

ANALCL_DEP         =  $(ANALCL_O:.$(ObjSuf)=.$(DepSuf))
ANALCL_DDEP        =  $(ANALCL_DO:.$(ObjSuf)=.$(DepSuf))
ANALCL_EDEP        =  $(ANALCL_EXEO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(ANALCL_DIR)/%.h, $(GO4SYS)/include/%.h, $(ANALCL_H) $(ANALCL_DIR)/TGo4AnalysisClient.h)

LIBDEPENDENC       += $(ANALCL_DEP) $(ANALCL_DDEP)
EXAMPLEEXECS       += $(ANALCL_EXE) $(ANALCL_EXECINT)
EXAMPDEPENDENCS    += $(ANALCL_EDEP)

ifdef DOPACKAGE
DISTRFILES         += $(ANALCL_S) $(ANALCL_H) $(ANALCL_LINKDEF) $(ANALCL_EXES) $(ANALCL_NOTLIBF) $(ANALCL_DIR)/Readme_CINTServer.txt
endif


##### local rules #####

$(GO4SYS)/include/%.h: $(ANALCL_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

$(ANALCL_EXE): $(BUILDGO4LIBS) $(ANALCL_EXEO)
	$(LD) $(LDFLAGS) $(ANALCL_EXEO) $(LIBS_FULLSET) $(ANALCL_LIB) $(OutPutOpt) $(ANALCL_EXE)
	@echo "$@  done"

$(ANALCL_EXECINT): $(BUILDGO4LIBS) $(ANALCL_EXECINTO)
	$(LD) $(LDFLAGS) $(ANALCL_EXECINTO) $(LIBS_FULLSET) $(ANALCL_LIB) $(OutPutOpt) $(ANALCL_EXECINT)
	@echo "$@  done"

$(ANALCL_DS): $(ANALCL_H)  $(ANALCL_LINKDEF)
	@$(ROOTCINTGO4) $(ANALCL_H) $(ANALCL_LINKDEF)

all::  $(ANALCL_EXE) $(ANALCL_EXECINT)

clean-bin::
	@rm -f $(ANALCL_O) $(ANALCL_DO)
	@rm -f $(ANALCL_EXEO) $(ANALCL_EXECINTO)
	@rm -f $(ANALCL_DEP) $(ANALCL_DDEP) $(ANALCL_DS) $(ANALCL_DH)
	@rm -f $(ANALCL_EDEP)

clean::
	@rm -f $(ANALCL_EXE) $(ANALCL_EXECINT)
