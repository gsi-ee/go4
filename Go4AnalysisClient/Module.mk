ANALCL_NAME        = Go4AnalysisClient

## normally should be like this for every module, but can be specific

ANALCL_DIR         = $(ANALCL_NAME)
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
ANALCL_EXECINT     = bin/$(ANALCL_EXECINTNAME)$(ExeSuf)  

ANALCL_H           = $(filter-out $(ANALCL_NOTLIBF) $(OLD_DICT), $(wildcard $(ANALCL_DIR)/*.$(HedSuf)))
ANALCL_S           = $(filter-out $(ANALCL_EXES) $(ANALCL_NOTLIBF) $(OLD_DICT), $(wildcard $(ANALCL_DIR)/*.$(SrcSuf)))
ANALCL_O           = $(ANALCL_S:.$(SrcSuf)=.$(ObjSuf))

ANALCL_DEP         =  $(ANALCL_O:.$(ObjSuf)=.$(DepSuf)) $(ANALCL_EXECINTO:.$(ObjSuf)=.$(DepSuf))
ANALCL_EDEP        =  $(ANALCL_EXEO:.$(ObjSuf)=.$(DepSuf)) 

# used in the main Makefile

ALLHDRS +=  $(patsubst $(ANALCL_DIR)/%.h, include/%.h, $(ANALCL_H) $(ANALCL_DIR)/TGo4AnalysisClient.h)

LIBDEPENDENC       += $(ANALCL_DEP)
EXAMPLEEXECS       += $(ANALCL_EXE) $(ANALCL_EXECINT)
EXAMPDEPENDENCS    += $(ANALCL_EDEP)

ifdef DOPACKAGE
DISTRFILES         += $(ANALCL_S) $(ANALCL_H) $(ANALCL_EXES) $(ANALCL_NOTLIBF) $(ANALCL_DIR)/Readme_CINTServer.txt
endif


##### local rules #####

include/%.h: $(ANALCL_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

$(ANALCL_EXE): $(ANALCL_EXEO) $(GO4AN_LIB)
	$(LD) $(LDFLAGS) $(ANALCL_EXEO) $(LIBS_FULLSET) $(ANALCL_LIB) $(OutPutOpt) $(ANALCL_EXE)
	@echo "$@  done"

$(ANALCL_EXECINT): $(ANALCL_EXECINTO) $(GO4AN_LIB)
	$(LD) $(LDFLAGS) $(ANALCL_EXECINTO) $(LIBS_FULLSET) $(ANALCL_LIB) $(OutPutOpt) $(ANALCL_EXECINT)
	@echo "$@  done"

all::  $(ANALCL_EXE) $(ANALCL_EXECINT)

clean-bin::
	@rm -f $(ANALCL_O) $(ANALCL_DEP)
	@rm -f $(ANALCL_EXEO) $(ANALCL_EXECINTO) $(ANALCL_EDEP)

clean::
	@rm -f $(ANALCL_EXE) $(ANALCL_EXECINT)
