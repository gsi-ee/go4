GO4DISPL_NAME        = Go4Display

## normally should be like this for every module, but can be specific

GO4DISPL_DIR         = $(GO4DISPL_NAME)

GO4DISPL_NOTLIBF     =

## must be similar for every module

GO4DISPL_H           = $(filter-out $(GO4DISPL_NOTLIBF) $(OLD_DICT), $(wildcard $(GO4DISPL_DIR)/*.$(HedSuf)))
GO4DISPL_S           = $(filter-out $(GO4DISPL_NOTLIBF) $(OLD_DICT), $(wildcard $(GO4DISPL_DIR)/*.$(SrcSuf)))
GO4DISPL_O           = $(GO4DISPL_S:.$(SrcSuf)=.$(ObjSuf))

GO4DISPL_DEP         =  $(GO4DISPL_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(GO4DISPL_DIR)/%.h, include/%.h, $(GO4DISPL_H))

LIBDEPENDENC       += $(GO4DISPL_DEP)

ifdef DOPACKAGE
DISTRFILES         += $(GO4DISPL_S) $(GO4DISPL_H)
endif

##### local rules #####

include/%.h: $(GO4DISPL_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean-bin::
	@rm -f $(GO4DISPL_O) $(GO4DISPL_DEP)
