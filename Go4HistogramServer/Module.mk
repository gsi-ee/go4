HISTSERV_NAME        = Go4HistogramServer

## normally should be like this for every module, but can be specific

HISTSERV_DIR         = $(GO4SYS)/$(HISTSERV_NAME)
HISTSERV_LINKDEF     = $(HISTSERV_DIR)/$(HISTSERV_NAME)LinkDef.$(HedSuf)
HISTSERV_EXENAME     = MainGo4ObjectClient

HISTSERV_NOTLIBF     = $(HISTSERV_DIR)/ObjectClient.C

## must be similar for every module

HISTSERV_EXEO        = $(HISTSERV_DIR)/$(HISTSERV_EXENAME).$(ObjSuf)
HISTSERV_EXES        = $(HISTSERV_DIR)/$(HISTSERV_EXENAME).$(SrcSuf)
HISTSERV_EXE         = $(HISTSERV_DIR)/$(HISTSERV_EXENAME)$(ExeSuf)   

HISTSERV_DICT        = $(HISTSERV_DIR)/$(DICT_PREFIX)$(HISTSERV_NAME)
HISTSERV_DH          = $(HISTSERV_DICT).$(HedSuf)
HISTSERV_DS          = $(HISTSERV_DICT).$(SrcSuf)
HISTSERV_DO          = $(HISTSERV_DICT).$(ObjSuf)

HISTSERV_H           = $(filter-out $(HISTSERV_EXEH) $(HISTSERV_NOTLIBF) $(HISTSERV_DH) $(HISTSERV_LINKDEF), $(wildcard $(HISTSERV_DIR)/*.$(HedSuf)))
HISTSERV_S           = $(filter-out $(HISTSERV_EXES) $(HISTSERV_NOTLIBF) $(HISTSERV_DS), $(wildcard $(HISTSERV_DIR)/*.$(SrcSuf)))
HISTSERV_O           = $(HISTSERV_S:.$(SrcSuf)=.$(ObjSuf))

HISTSERV_DEP         =  $(HISTSERV_O:.$(ObjSuf)=.$(DepSuf))
HISTSERV_DDEP        =  $(HISTSERV_DO:.$(ObjSuf)=.$(DepSuf))
HISTSERV_EDEP        =  $(HISTSERV_EXEO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(HISTSERV_DIR)/%.h, $(GO4SYS)/include/%.h, $(HISTSERV_H))

LIBDEPENDENC       += $(HISTSERV_DEP) $(HISTSERV_DDEP)
EXAMPLEEXECS       += $(HISTSERV_EXE)
EXAMPDEPENDENCS    += $(HISTSERV_EDEP)

ifdef DOPACKAGE
DISTRFILES         += $(HISTSERV_S) $(HISTSERV_H) $(HISTSERV_LINKDEF) $(HISTSERV_EXEH) $(HISTSERV_EXES)
endif

##### local rules #####

$(GO4SYS)/include/%.h: $(HISTSERV_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

$(HISTSERV_EXE): $(BUILDGO4LIBS) $(HISTSERV_EXEO)
	$(LD) $(LDFLAGS) $(HISTSERV_EXEO) $(LIBS_FULLSET) $(OutPutOpt) $(HISTSERV_EXE)
	@echo "$@  done"

$(HISTSERV_DS): $(HISTSERV_H)  $(HISTSERV_LINKDEF)
	@$(ROOTCINTGO4) $(HISTSERV_H) $(HISTSERV_LINKDEF)

all:: $(HISTSERV_EXE)

clean-bin::
	@rm -f $(HISTSERV_O) $(HISTSERV_DO)
	@rm -f $(HISTSERV_EXEO) $(HISTSERV_EDEP)
	@rm -f $(HISTSERV_DEP) $(HISTSERV_DDEP) $(HISTSERV_DS) $(HISTSERV_DH)

clean::
	@rm -f $(HISTSERV_EXE)
