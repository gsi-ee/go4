DYNLIST_NAME        = Go4DynamicList

## normally should be like this for every module, but can be specific

DYNLIST_DIR         = $(DYNLIST_NAME)

DYNLIST_NOTLIBF     = $(DYNLIST_DIR)/TGo4HistoFillAction.h

## must be similar for every module

DYNLIST_H           = $(filter-out $(DYNLIST_NOTLIBF) $(OLD_DICT), $(wildcard $(DYNLIST_DIR)/*.$(HedSuf)))
DYNLIST_S           = $(filter-out $(DYNLIST_NOTLIBF) $(OLD_DICT), $(wildcard $(DYNLIST_DIR)/*.$(SrcSuf)))
DYNLIST_O           = $(DYNLIST_S:.$(SrcSuf)=.$(ObjSuf))

DYNLIST_DEP         =  $(DYNLIST_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(DYNLIST_DIR)/%.h, include/%.h, $(DYNLIST_H))

LIBDEPENDENC       += $(DYNLIST_DEP)

ifdef DOPACKAGE
DISTRFILES         += $(DYNLIST_S) $(DYNLIST_H) $(DYNLIST_NOTLIBF)
endif

##### local rules #####

include/%.h: $(DYNLIST_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean-bin::
	@rm -f $(DYNLIST_O) $(DYNLIST_DEP)

