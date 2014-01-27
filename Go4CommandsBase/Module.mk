COMBASE_NAME        = Go4CommandsBase

## normally should be like this for every module, but can be specific

COMBASE_DIR         = $(COMBASE_NAME)

COMBASE_NOTLIBF     = 

## must be similar for every module

COMBASE_H           = $(filter-out $(COMBASE_NOTLIBF) $(OLD_DICT), $(wildcard $(COMBASE_DIR)/*.$(HedSuf)))
COMBASE_S           = $(filter-out $(COMBASE_NOTLIBF) $(OLD_DICT), $(wildcard $(COMBASE_DIR)/*.$(SrcSuf)))
COMBASE_O           = $(COMBASE_S:.$(SrcSuf)=.$(ObjSuf))

COMBASE_DEP         =  $(COMBASE_O:.$(ObjSuf)=.$(DepSuf))

# used in the main Makefile

ALLHDRS +=  $(patsubst $(COMBASE_DIR)/%.h, include/%.h, $(COMBASE_H))

LIBDEPENDENC       += $(COMBASE_DEP) 

ifdef DOPACKAGE
COMBASE_DISTRFILES  = $(COMBASE_S) $(COMBASE_H) $(COMBASE_NOTLIBF)
DISTRFILES         += $(COMBASE_DISTRFILES)
endif


##### local rules #####

include/%.h: $(COMBASE_DIR)/%.h
	@echo "Copy header $@ ..." 
	@cp -f $< $@

clean-bin::
	@rm -f $(COMBASE_O) $(COMBASE_DEP)
