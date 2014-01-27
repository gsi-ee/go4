TASKHAND_NAME        = Go4TaskHandler

## normally should be like this for every module, but can be specific

TASKHAND_DIR         = $(TASKHAND_NAME)

TASKHAND_NOTLIBF     = $(TASKHAND_DIR)/Go4EmergencyCommands.h

## must be similar for every module

TASKHAND_H           = $(filter-out $(TASKHAND_NOTLIBF) $(OLD_DICT), $(wildcard $(TASKHAND_DIR)/*.$(HedSuf)))
TASKHAND_S           = $(filter-out $(TASKHAND_NOTLIBF) $(OLD_DICT), $(wildcard $(TASKHAND_DIR)/*.$(SrcSuf)))
TASKHAND_O           = $(TASKHAND_S:.$(SrcSuf)=.$(ObjSuf))

TASKHAND_DEP         = $(TASKHAND_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(TASKHAND_DIR)/%.h, include/%.h, $(TASKHAND_H) $(TASKHAND_DIR)/Go4EmergencyCommands.h)

LIBDEPENDENC       += $(TASKHAND_DEP) 

ifdef DOPACKAGE
TASKHAND_DISTRFILES = $(TASKHAND_S) $(TASKHAND_H) $(TASKHAND_NOTLIBF)
DISTRFILES         += $(TASKHAND_DISTRFILES)
endif


##### local rules #####

include/%.h: $(TASKHAND_DIR)/%.h
	@echo "Copy header $@ ..."
	@cp -f $< $@

clean-bin::
	@rm -f $(TASKHAND_O) $(TASKHAND_DEP)

