GO4QUEUE_NAME        = Go4Queue

## normally should be like this for every module, but can be specific

GO4QUEUE_DIR         = $(GO4QUEUE_NAME)
GO4QUEUE_EXENAME     = Main$(GO4QUEUE_NAME)

GO4QUEUE_NOTLIBF     = 

## must be similar for every module

GO4QUEUE_EXEO        = $(GO4QUEUE_DIR)/$(GO4QUEUE_EXENAME).$(ObjSuf)
GO4QUEUE_EXES        = $(GO4QUEUE_DIR)/$(GO4QUEUE_EXENAME).$(SrcSuf)
GO4QUEUE_EXE         = $(GO4QUEUE_DIR)/$(GO4QUEUE_EXENAME)$(ExeSuf)   

GO4QUEUE_H           = $(filter-out $(GO4QUEUE_EXEH) $(GO4QUEUE_NOTLIBF) $(GO4QUEUE_DH) $(GO4QUEUE_LINKDEF), $(wildcard $(GO4QUEUE_DIR)/*.$(HedSuf)))
GO4QUEUE_S           = $(filter-out $(GO4QUEUE_EXES) $(GO4QUEUE_NOTLIBF) $(GO4QUEUE_DS), $(wildcard $(GO4QUEUE_DIR)/*.$(SrcSuf)))
GO4QUEUE_O           = $(GO4QUEUE_S:.$(SrcSuf)=.$(ObjSuf))

GO4QUEUE_DEP         =  $(GO4QUEUE_O:.$(ObjSuf)=.$(DepSuf))
GO4QUEUE_EDEP        =  $(GO4QUEUE_EXEO:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(GO4QUEUE_DIR)/%.h, include/%.h, $(GO4QUEUE_H))

LIBDEPENDENC       += $(GO4QUEUE_DEP)
EXAMPLEEXECS       += $(GO4QUEUE_EXE)
EXAMPDEPENDENCS    += $(GO4QUEUE_EDEP)

ifdef DOPACKAGE
GO4QUEUE_DISTRFILES = $(GO4QUEUE_S) $(GO4QUEUE_H) $(GO4QUEUE_EXES)
DISTRFILES         += $(GO4QUEUE_DISTRFILES)
endif


##### local rules #####

include/%.h: $(GO4QUEUE_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

$(GO4QUEUE_EXE):  $(GO4QUEUE_EXEO) $(GO4TSKH_LIB)
	$(LD) $(LDFLAGS) $(GO4QUEUE_EXEO) $(LIBS_TASKHANDSET) $(OutPutOpt) $(GO4QUEUE_EXE)
	@echo "$@  done"

all:: $(GO4QUEUE_EXE)

clean-bin::
	@rm -f $(GO4QUEUE_O) $(GO4QUEUE_EXEO)
	@rm -f $(GO4QUEUE_DEP) $(GO4QUEUE_EDEP)

clean::
	@rm -f $(GO4QUEUE_EXE)
