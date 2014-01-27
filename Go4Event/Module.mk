GO4EVENT_NAME       = Go4Event
GO4EVENTPAR_NAME    = Go4EventPar

## normally should be like this for every module, but can be specific

GO4EVENT_DIR         = $(GO4EVENT_NAME)

GO4EVENT_NOTLIBF     =

## must be similar for every module

GO4EVENTPAR_H        = $(wildcard $(GO4EVENT_DIR)/*Parameter.$(HedSuf))
GO4EVENTPAR_S        = $(wildcard $(GO4EVENT_DIR)/*Parameter.$(SrcSuf))
GO4EVENTPAR_O        = $(GO4EVENTPAR_S:.$(SrcSuf)=.$(ObjSuf))

GO4EVENT_H           = $(filter-out $(GO4EVENT_NOTLIBF) $(GO4EVENTPAR_H) $(OLD_DICT), $(wildcard $(GO4EVENT_DIR)/*.$(HedSuf)))
GO4EVENT_S           = $(filter-out $(GO4EVENT_NOTLIBF) $(GO4EVENTPAR_S) $(OLD_DICT), $(wildcard $(GO4EVENT_DIR)/*.$(SrcSuf)))
GO4EVENT_O           = $(GO4EVENT_S:.$(SrcSuf)=.$(ObjSuf))

GO4EVENT_DEP         =  $(GO4EVENT_O:.$(ObjSuf)=.$(DepSuf))

GO4EVENTPAR_DEP      =  $(GO4EVENTPAR_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(GO4EVENT_DIR)/%.h, include/%.h, $(GO4EVENT_H))
ALLHDRS +=  $(patsubst $(GO4EVENT_DIR)/%.h, include/%.h, $(GO4EVENTPAR_H))

LIBDEPENDENC       += $(GO4EVENT_DEP) $(GO4EVENTPAR_DEP)

ifdef DOPACKAGE
DISTRFILES         += $(GO4EVENT_S) $(GO4EVENT_H) 
DISTRFILES         += $(GO4EVENTPAR_S) $(GO4EVENTPAR_H)
endif


##### local rules #####

include/%.h: $(GO4EVENT_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean-bin::
	@rm -f $(GO4EVENT_O) $(GO4EVENT_DEP)
	@rm -f $(GO4EVENTPAR_O) $(GO4EVENTPAR_DEP)
