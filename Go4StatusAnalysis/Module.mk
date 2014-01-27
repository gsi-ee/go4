STATANAL_NAME        = Go4StatusAnalysis

## normally should be like this for every module, but can be specific

STATANAL_DIR         = $(STATANAL_NAME)

STATANAL_NOTLIBF     =

## must be similar for every module


STATANAL_H           = $(filter-out $(STATANAL_NOTLIBF) $(OLD_DICT), $(wildcard $(STATANAL_DIR)/*.$(HedSuf)))
STATANAL_S           = $(filter-out $(STATANAL_NOTLIBF) $(OLD_DICT), $(wildcard $(STATANAL_DIR)/*.$(SrcSuf)))
STATANAL_O           = $(STATANAL_S:.$(SrcSuf)=.$(ObjSuf))

STATANAL_DEP         =  $(STATANAL_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(STATANAL_DIR)/%.h, include/%.h, $(STATANAL_H))

LIBDEPENDENC       += $(STATANAL_DEP)

ifdef DOPACKAGE
DISTRFILES         += $(STATANAL_S) $(STATANAL_H)
endif

##### local rules #####

include/%.h: $(STATANAL_DIR)/%.h
	@echo "Copy header $@ ..."
	@cp -f $< $@

clean-bin::
	@rm -f $(STATANAL_O) $(STATANAL_DEP)

