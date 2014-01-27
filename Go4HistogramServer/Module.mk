HISTSERV_NAME        = Go4HistogramServer

## normally should be like this for every module, but can be specific

HISTSERV_DIR         = $(HISTSERV_NAME)
HISTSERV_EXENAME     = MainGo4ObjectClient

HISTSERV_NOTLIBF     = $(HISTSERV_DIR)/ObjectClient.C

## must be similar for every module

HISTSERV_EXEO        = $(HISTSERV_DIR)/$(HISTSERV_EXENAME).$(ObjSuf)
HISTSERV_EXES        = $(HISTSERV_DIR)/$(HISTSERV_EXENAME).$(SrcSuf)
HISTSERV_EXE         = $(HISTSERV_DIR)/$(HISTSERV_EXENAME)$(ExeSuf)   

HISTSERV_H           = $(filter-out $(HISTSERV_EXEH) $(OLD_DICT) $(HISTSERV_NOTLIBF), $(wildcard $(HISTSERV_DIR)/*.$(HedSuf)))
HISTSERV_S           = $(filter-out $(HISTSERV_EXES) $(OLD_DICT) $(HISTSERV_NOTLIBF), $(wildcard $(HISTSERV_DIR)/*.$(SrcSuf)))
HISTSERV_O           = $(HISTSERV_S:.$(SrcSuf)=.$(ObjSuf))

HISTSERV_DEP         =  $(HISTSERV_O:.$(ObjSuf)=.$(DepSuf))
HISTSERV_EDEP        =  $(HISTSERV_EXEO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(HISTSERV_DIR)/%.h, include/%.h, $(HISTSERV_H))

LIBDEPENDENC       += $(HISTSERV_DEP)
EXAMPLEEXECS       += $(HISTSERV_EXE)
EXAMPDEPENDENCS    += $(HISTSERV_EDEP)

ifdef DOPACKAGE
DISTRFILES         += $(HISTSERV_S) $(HISTSERV_H) $(HISTSERV_EXEH) $(HISTSERV_EXES)
endif

##### local rules #####

include/%.h: $(HISTSERV_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

$(HISTSERV_EXE): $(HISTSERV_EXEO) $(GO4AN_LIB)
	$(LD) $(LDFLAGS) $(HISTSERV_EXEO) $(LIBS_FULLSET) $(OutPutOpt) $(HISTSERV_EXE)
	@echo "$@  done"

all:: $(HISTSERV_EXE)

clean-bin::
	@rm -f $(HISTSERV_O) $(HISTSERV_DEP) 
	@rm -f $(HISTSERV_EXEO) $(HISTSERV_EDEP)

clean::
	@rm -f $(HISTSERV_EXE)
